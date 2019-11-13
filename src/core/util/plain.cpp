#include "plain.hpp"


std::string ToPlain(const Value &v)
{
    switch (v.GetType())
    {
        case t_info:
        case t_null:
        case t_ampersand:
            return "";
        break;

        case t_break:
            return "\n";
            break;

        case t_string:
            return v.GetContent();
            break;

        case t_multi:
        {
            std::string ret = "";
            for (const Value &vi : v.multicontent)
                ret += ToPlain(vi);
            return ret;
        }

        case t_html:
        {
            std::string res;
            std::string tag = v.GetTag();

            if (tag == "img")
            {
                res += "\nImage: ";
            }

            if (tag == "tr")
            {
                res += "\n";
            }

            if (tag == "td")
            {
                res += "\t";
            }

            for (const Value &vi : v.multicontent)
                res += ToPlain(vi) + " ";

            if (tag == "img" || tag == "p" || tag == "br" || tag == "li" || tag == "ul" || tag == "ol")
                res += "\n";

            return res;
        }
        case t_attr:
        {
            std::string tag = v.GetTag();

            if (tag == "href" || tag == "src" || tag == "href")
            {
                return "(" + v.GetRawContent() + ") ";
            }

            return "";
        }
    }
    
    // Cant get here
    return "";
}
