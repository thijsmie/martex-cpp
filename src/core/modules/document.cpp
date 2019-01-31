#include "document.hpp"
#include "core/util/string_util.hpp"
#include <set>

DocumentEnvironment::DocumentEnvironment(std::shared_ptr<Environment> parent, bool adminmode) : util::CppEnvironment<DocumentEnvironment>(parent), m_adminmode(adminmode)
{
    m_chapter = 0;
    m_section = 0;
    m_subsection = 0;
    m_subsubsection = 0;
    m_labeling = false;

    AddMethod("title", &DocumentEnvironment::title);
    AddMethod("chapter", &DocumentEnvironment::chapter);
    AddMethod("section", &DocumentEnvironment::section);
    AddMethod("subsection", &DocumentEnvironment::subsection);
    AddMethod("subsubsection", &DocumentEnvironment::subsubsection);

    AddMethod("label", &DocumentEnvironment::label);
    AddMethod("labeling", &DocumentEnvironment::labeling);
}

const std::set<std::string> sectiontags{"h1", "h2", "h3", "h4", "h5", "h6", "p"};
Value DocumentEnvironment::EndEnvironment(Token, Value content)
{
    std::vector<Value> &multicontent = content.multicontent;
    for (size_t i = 0; i < multicontent.size(); i++)
    {
        const Value &v = multicontent[i];

        if (v.GetType() == t_string && util::wsonly(v.GetContent()))
        {
            if (v.GetContent() == "\n")
            {
                // line
                bool tagflag = false;
                size_t j = i + 1;
                for (; j < multicontent.size(); j++)
                {
                    const Value &vi = multicontent[j];
                    if (vi.GetType() != t_string || vi.GetContent() != "\n")
                    {
                        if (vi.GetType() == t_html && sectiontags.find(vi.GetTag()) != sectiontags.end())
                            tagflag = true;
                        break;
                    }
                }

                multicontent.erase(multicontent.begin() + i, multicontent.begin() + j);
                // multiline?
                if (j > i + 1)
                {
                    if (tagflag)
                        i--;
                    else
                        multicontent.emplace(multicontent.begin() + i, t_info);
                }
                else
                {
                    i--;
                }
            }
            else
            {
                // other ws
                multicontent.erase(multicontent.begin() + i);
                i--;
            }
        }
        else if (v.GetType() == t_html)
        {
            if (sectiontags.find(v.GetTag()) != sectiontags.end())
            {
                size_t j = i + 1;
                for (; j < multicontent.size(); j++)
                {
                    const Value &vi = multicontent[j];
                    if (vi.GetContent() != "\n")
                        break;
                }

                multicontent.erase(multicontent.begin() + i + 1, multicontent.begin() + j);
            }
        }
    }

    for (size_t i = 0; i < multicontent.size(); i++)
    {
        const Value &v = multicontent[i];

        if (((v.GetType() == t_html && sectiontags.find(v.GetTag()) != sectiontags.end())) || v.GetType() == t_info)
        {
            if (v.GetType() == t_info)
            {
                multicontent.erase(multicontent.begin() + i);
            }
            else 
            {
                i++;
            }

            size_t j = i;
            for (; j < multicontent.size(); j++)
            {
                const Value &vi = multicontent[j];
                if ((vi.GetType() == t_html && sectiontags.find(vi.GetTag()) != sectiontags.end()) || vi.GetType() == t_info)
                    break;
            }
            
            std::vector<Value> current_paragraph;
            std::move(multicontent.begin() + i, multicontent.begin() + j, std::back_inserter(current_paragraph));
            multicontent.erase(multicontent.begin() + i, multicontent.begin() + j);
            multicontent.emplace(multicontent.begin() + i, "p", std::move(current_paragraph));
        }
    }


    if (m_adminmode)
        return Value("body", Value::asString(content));
    else
        return content;
}

Value DocumentEnvironment::title(Token, std::vector<Value> args)
{
    return Value("h1", std::move(args));
}

Value DocumentEnvironment::chapter(Token, std::vector<Value> args)
{
    m_chapter++;
    m_section = 0;
    m_subsection = 0;
    m_subsubsection = 0;

    if (m_labeling)
        args.emplace(args.begin(), t_string, std::to_string(m_chapter) + "&emsp;");

    return Value("h2", std::move(args));
}

Value DocumentEnvironment::section(Token, std::vector<Value> args)
{
    m_section++;
    m_subsection = 0;
    m_subsubsection = 0;
    if (m_labeling)
        args.emplace(args.begin(), t_string, std::to_string(m_section) + "&emsp;");

    return Value("h3", std::move(args));
}

Value DocumentEnvironment::subsection(Token, std::vector<Value> args)
{
    m_subsection++;
    m_subsubsection = 0;
    if (m_labeling)
        args.emplace(args.begin(), t_string,
                     std::to_string(m_section) + "." + std::to_string(m_subsection) + "&emsp;");

    return Value("h4", std::move(args));
}

Value DocumentEnvironment::subsubsection(Token, std::vector<Value> args)
{
    m_subsubsection++;
    if (m_labeling)
        args.emplace(args.begin(), t_string,
                     std::to_string(m_section) + "." + std::to_string(m_subsection) +
                         std::to_string(m_subsubsection) + "&emsp;");

    return Value("h5", std::move(args));
}

Value DocumentEnvironment::label(Token, std::vector<Value> args)
{
    std::string l = "";
    if (m_subsubsection > 0)
        l = std::to_string(m_section) + "." + std::to_string(m_subsection) + "." + std::to_string(m_subsubsection);
    else if (m_subsection > 0)
        l = std::to_string(m_section) + "." + std::to_string(m_subsection);
    else if (m_section > 0)
        l = std::to_string(m_section);
    else
        l = std::to_string(m_chapter);

    SetGlobal(args[0].GetContent(), Value(t_string, l));
    return Value();
}

Value DocumentEnvironment::labeling(Token, std::vector<Value> args)
{
    std::string a = args[0].GetContent();

    if (a == "true" || a == "1")
        m_labeling = true;
    else
        m_labeling = false;

    return Value();
}