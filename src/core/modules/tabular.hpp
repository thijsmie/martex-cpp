#pragma once

#include "core/util/cppenvironment.hpp"
#include "core/util/string_util.hpp"
#include "core/util/regex_util.hpp"
#include "core/language/runtime_error.hpp"
#include "core/util/buffer.hpp"

#include <memory>
#include <vector>

static const std::vector<std::string> tabular_commands(
    {"hline", "multicolumn", "multirow"});

enum ColspecType
{
    ct_left,
    ct_center,
    ct_right,
    ct_b_left,
    ct_bb_left,
    ct_b_center,
    ct_bb_center,
    ct_b_right,
    ct_bb_right,
    ct_,
    ct_b,
    ct_bb
};

enum RowspecType
{
    rt_none,
    rt_t,
    rt_b,
    rt_tt,
    rt_bb,
};

class TabularEnvironment;
typedef util::CppEnvironment<TabularEnvironment> ParentTabular;
class TabularEnvironment : public ParentTabular
{
  private:
    std::vector<ColspecType> m_colspec;
    std::vector<RowspecType> m_rowspec;
    std::vector<Value> m_tableattr;

  public:
    TabularEnvironment(std::shared_ptr<Environment> parent) : ParentTabular(parent)
    {
        AddMethod("hline", &TabularEnvironment::hline);
        AddMethod("multicolumn", &TabularEnvironment::multicolumn);
        AddMethod("multirow", &TabularEnvironment::multirow);
        AddMethod("width", &TabularEnvironment::width);
        AddMethod("height", &TabularEnvironment::height);
    }

    Value hline(Token command, std::vector<Value> arguments)
    {
        if (arguments.size() != 0)
            throw RuntimeError(command, "takes no arguments");
        return Value(t_info, command.GetLexeme(), std::move(arguments));
    }

    Value multicolumn(Token command, std::vector<Value> arguments)
    {
        if (arguments.size() != 3)
            throw RuntimeError(command, "takes three arguments");

        // Check if numerical number of columns
        std::string numcols = arguments[0].GetContent();
        util::trim(numcols);
        int cols = 0;
        try
        {
            cols = std::stoi(numcols);
        }
        catch (...)
        {
            throw RuntimeError(command, "'" + numcols + "' is not an acceptable integer");
        }
        if (cols < 1 or cols >= 32)
            throw RuntimeError(command, "number of columns should be in range [1,31]");

        // Validate colspec
        std::string colspec = arguments[1].GetContent();
        if (!validateColspec(colspec))
            throw RuntimeError(command, "'" + colspec + "' is not an acceptable column specification");

        return Value(t_info, command.GetLexeme(), std::move(arguments));
    }

    Value multirow(Token command, std::vector<Value> arguments)
    {
        if (arguments.size() != 2)
            throw RuntimeError(command, "takes three arguments");

        // Check if numerical number of columns
        std::string numrows = arguments[0].GetContent();
        util::trim(numrows);
        int rows = 0;
        try
        {
            rows = std::stoi(numrows);
        }
        catch (...)
        {
            throw RuntimeError(command, numrows + " is not an acceptable integer.");
        }
        if (rows < 1 or rows >= 32)
            throw RuntimeError(command, "number of rows should be in range [1,31]");

        return Value(t_info, command.GetLexeme(), std::move(arguments));
    }
    
    Value width(Token command, std::vector<Value> arguments)
    {
        if (arguments.size() != 1)
            throw RuntimeError(command, "takes one argument");
            
        for(const Value &v : m_tableattr)
            if (v.GetTag() == "width")
                throw RuntimeError(command, "attribute already set!");
                
        std::string width = arguments[0].GetContent();
        
        if (!util::is_valid_sizing(width))
            throw RuntimeError(command, width + " is not a valid size");
        else if (util::dgonly(width))
            width += "%";
            
        m_tableattr.emplace_back("width", width);
        return Value();
    }
    
    Value height(Token command, std::vector<Value> arguments)
    {
        if (arguments.size() != 1)
            throw RuntimeError(command, "takes one argument");
            
        for(const Value &v : m_tableattr)
            if (v.GetTag() == "height")
                throw RuntimeError(command, "attribute already set!");
                
        std::string height = arguments[0].GetContent();
        
        if (!util::is_valid_sizing(height))
            throw RuntimeError(command, height + " is not a valid size");
        else if (util::dgonly(height))
            height += "%";
            
        m_tableattr.emplace_back("height", height);
        return Value();
    }

    void StartEnvironment(Token begin, Value colspec)
    {
        std::string spec = colspec.GetContent();
        if (!validateColspec(spec))
            throw RuntimeError(begin, "'" + spec + "' is not an acceptable column specification");
        m_colspec = parseColspec(spec);
    }

    Value EndEnvironment(Token end, Value content)
    {
        m_rowspec = parseRowspec(content);

        std::vector<Value> rows;
        std::vector<Value> row;
        std::vector<Value> cell;
        
        // This leaves m_tableattr a mess but this class getś GC'ed after the EndEnvironment call anyway
        rows.insert(rows.end(), std::make_move_iterator(m_tableattr.begin()), 
                    std::make_move_iterator(m_tableattr.end()));

        int tablewidth = m_colspec.size() - 1;
        int tableheight = m_rowspec.size() - 1;

        std::vector<int> skipcol(tablewidth, 0);

        bool skip = false;
        int x = 0;
        int y = 0;

        rows.emplace_back("class", "martex");

        if (x < tablewidth && y < tableheight)
        {
            generateCellMarkup(m_colspec[x], m_colspec[x + 1], m_rowspec[y], m_rowspec[y + 1], cell);
        }

        for (Value &v : content.multicontent)
        {
            switch (v.GetType())
            {
            case t_null:
            case t_multi:
                break;
            case t_string:
            case t_html:
            case t_attr:
                if (!skip)
                    cell.push_back(std::move(v));
                break;
            case t_ampersand:
                skip = false;
                row.push_back(Value("td", std::move(cell)));
                cell.clear();
                x++;
                while (x < tablewidth && skipcol[x] > 0)
                {
                    skipcol[x]--;
                    x++;
                }

                if (x < tablewidth && y < tableheight)
                {
                    generateCellMarkup(m_colspec[x], m_colspec[x + 1], m_rowspec[y], m_rowspec[y + 1], cell);
                }
                break;
            case t_break:
                skip = false;
                row.push_back(Value("td", std::move(cell)));
                cell.clear();

                // if we break the table before the end we need to gen some empty cells
                while (x < tablewidth - 1)
                {
                    if (skipcol[++x])
                        skipcol[x]--;
                    else
                    {
                        generateCellMarkup(m_colspec[x], m_colspec[x + 1], m_rowspec[y], m_rowspec[y + 1], cell);
                        row.push_back(Value("td", std::move(cell)));
                        cell.clear();
                    }
                }

                rows.push_back(Value("tr", std::move(row)));
                row.clear();

                x = 0;
                y++;

                // if we have any skipped left cols
                while (x < tablewidth && skipcol[x] > 0)
                {
                    skipcol[x]--;
                    x++;
                }

                if (x < tablewidth && y < tableheight)
                {
                    generateCellMarkup(m_colspec[x], m_colspec[x + 1], m_rowspec[y], m_rowspec[y + 1], cell);
                }

                break;
            case t_info:
                switch ((int)std::distance(tabular_commands.begin(), std::find(tabular_commands.begin(),
                                                                               tabular_commands.end(), v.GetTag())))
                {
                case 0: //hline
                    /// This will have been handled already by parsing the rowspec
                    break;
                case 1: //multicol
                {
                    /// throw away anything already in here
                    cell.clear();

                    /// get colspan (was already validated in cmd)
                    std::string numcols = v.multicontent[0].GetContent();
                    util::trim(numcols);
                    int cols = std::stoi(numcols);

                    cell.emplace_back("colspan", numcols);

                    // detect collision
                    for (int i = x; i < x + cols; i++)
                        if (i < tablewidth && skipcol[i] > 0)
                            throw RuntimeError(end, "collision of multirow and multicolumn detected");
                    x += cols - 1; // we get one from the ampersand

                    // parse colspec
                    std::vector<ColspecType> _colspec = parseColspec(v.multicontent[1].GetContent());

                    // aply markup
                    if (y < tableheight)
                    {
                        generateCellMarkup(_colspec[0], _colspec[1], m_rowspec[y], m_rowspec[y + 1], cell);
                    }

                    // move in content
                    cell.push_back(std::move(v.multicontent[2]));

                    // skip rest of cell content till amp/break, hopefully only ws
                    skip = true;
                    break;
                }
                case 2: //multirow
                {
                    /// throw away anything already in here
                    cell.clear();

                    /// get rowspan (was already validated in command)
                    std::string numrows = v.multicontent[0].GetContent();
                    util::trim(numrows);
                    int rows = std::stoi(numrows);

                    cell.emplace_back("rowspan", numrows);

                    // apply skipcols
                    if (x < tablewidth)
                        skipcol[x] += rows - 1;

                    // aply markup
                    if (x < tablewidth && y + rows <= tableheight)
                    {
                        generateCellMarkup(m_colspec[x], m_colspec[x + 1], m_rowspec[y], m_rowspec[y + rows], cell);
                    }

                    // move in content
                    cell.push_back(std::move(v.multicontent[1]));

                    // skip rest of cell content till amp/break, probably only ws
                    skip = true;
                    break;
                }
                }
            }
        }

        return Value("table", std::move(rows));
    }

  private:
    static bool validateColspec(std::string colspec)
    {
        int numstaves = 0;
        bool hadone = false;
        for (char c : colspec)
        {
            switch (c)
            {
            case 'l':
            case 'c':
            case 'r':
                hadone = true;
                numstaves = 0;
                break;
            case ' ':
            case '\t':
                break;
            case '|':
                if (++numstaves == 3)
                    return false;
                break;
            default:
                return false;
            }
        }
        return hadone;
    }

    static std::vector<ColspecType> parseColspec(std::string colspec)
    {
        std::vector<ColspecType> spec;

        int numstaves = 0;
        for (char c : colspec)
        {
            switch (c)
            {
            case 'l':
                switch (numstaves)
                {
                case 0:
                    spec.push_back(ct_left);
                    break;
                case 1:
                    spec.push_back(ct_b_left);
                    break;
                case 2:
                    spec.push_back(ct_bb_left);
                    break;
                }
                numstaves = 0;
                break;
            case 'c':
                switch (numstaves)
                {
                case 0:
                    spec.push_back(ct_center);
                    break;
                case 1:
                    spec.push_back(ct_b_center);
                    break;
                case 2:
                    spec.push_back(ct_bb_center);
                    break;
                }
                numstaves = 0;
                break;
            case 'r':
                switch (numstaves)
                {
                case 0:
                    spec.push_back(ct_right);
                    break;
                case 1:
                    spec.push_back(ct_b_right);
                    break;
                case 2:
                    spec.push_back(ct_bb_right);
                    break;
                }
                numstaves = 0;
                break;
            case ' ':
            case '\t':
                break;
            case '|':
                ++numstaves;
                break;
            }
        }
        switch (numstaves)
        {
        case 0:
            spec.push_back(ct_);
            break;
        case 1:
            spec.push_back(ct_b);
            break;
        case 2:
            spec.push_back(ct_bb);
            break;
        }
        return spec;
    }

    static std::vector<RowspecType> parseRowspec(const Value &content)
    {
        std::vector<RowspecType> ret;
        int hlines = 0;

        for (const Value &v : content.multicontent)
        {
            switch (v.GetType())
            {
            case t_break:
                switch (hlines)
                {
                case 0:
                    ret.push_back(rt_none);
                    break;
                case 1:
                    ret.push_back(rt_t);
                    break;
                default:
                    ret.push_back(rt_tt);
                    break;
                }
                hlines = 0;
                break;
            case t_info:
                if (v.GetTag() == "hline")
                    hlines++;
                break;
            default:
                break;
            }
        }

        switch (hlines)
        {
        case 0:
            ret.push_back(rt_none);
            break;
        case 1:
            ret.push_back(rt_b);
            break;
        default:
            ret.push_back(rt_bb);
            break;
        }

        return ret;
    }

    void generateCellMarkup(ColspecType maincs, ColspecType modcs, RowspecType mainrs, RowspecType modrs, std::vector<Value> &cell)
    {
        std::string cls = "";

        // text alignment
        switch (maincs)
        {
        case ct_right:
        case ct_b_right:
        case ct_bb_right:
            cls += "a-r ";
            break;
        case ct_left:
        case ct_b_left:
        case ct_bb_left:
            cls += "a-l ";
            break;
        default:
            break;
        }

        // left border
        switch (maincs)
        {
        case ct_b_right:
        case ct_b_left:
        case ct_b_center:
            cls += "b-l ";
            break;
        case ct_bb_right:
        case ct_bb_left:
        case ct_bb_center:
            cls += "b-ll ";
            break;
        default:
            break;
        }

        // right border
        switch (modcs)
        {
        case ct_b:
            cls += "b-r ";
            break;
        case ct_bb:
            cls += "b-rr ";
            break;
        default:
            break;
        }

        // top border
        switch (mainrs)
        {
        case rt_t:
            cls += "b-t ";
            break;
        case rt_tt:
            cls += "b-tt ";
            break;
        default:
            break;
        }

        // bottom border
        switch (modrs)
        {
        case rt_b:
            cls += "b-b ";
            break;
        case rt_bb:
            cls += "b-bb ";
            break;
        default:
            break;
        }

        // class?

        if (cls != "")
        {
            util::trim(cls);
            cell.emplace_back("class", cls);
        }
    }
};