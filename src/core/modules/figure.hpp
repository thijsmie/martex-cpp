#pragma once

#include "core/util/cppenvironment.hpp"
#include "core/util/regex_util.hpp"
#include "core/util/string_util.hpp"
#include "core/language/runtime_error.hpp"
#include <string>
#include <memory>

class FigureEnvironment : public util::CppEnvironment<FigureEnvironment>
{
  private:
    std::string label_;
    std::string alttext_;
    std::string graphic_;
    std::string caption_;
    std::string width_;
    std::string height_;

    void setLabel(std::shared_ptr<Environment> env)
    {
        if (label_ != "")
            return;

        int ind = 1;
        try
        {
            ind = std::stoi(env->Get("figurecounter").GetContent());
        }
        catch (RuntimeError e)
        {
            // no figures yet, this is 1
        }
        env->Set("figurecounter", Value(t_string, std::to_string(ind + 1)));
        label_ = std::to_string(ind);
    }

  public:
    FigureEnvironment(std::shared_ptr<Environment> parent) : util::CppEnvironment<FigureEnvironment>(parent)
    {
        AddMethod("includegraphics", &FigureEnvironment::includegraphics);
        AddMethod("width", &FigureEnvironment::width);
        AddMethod("height", &FigureEnvironment::height);
        AddMethod("caption", &FigureEnvironment::caption);
        AddMethod("alttext", &FigureEnvironment::alttext);
        AddMethod("label", &FigureEnvironment::label);
    }

    Value includegraphics(std::shared_ptr<Environment> env, Token cmd, std::vector<Value> args)
    {
        setLabel(env);

        if (args.size() != 1)
            throw RuntimeError(cmd, "takes one arguments");

        std::string uri = args[0].GetContent();

        if (!(util::is_valid_subpath(uri) || util::is_valid_url(uri)))
            throw RuntimeError(cmd, "image url invalid");

        graphic_ = uri;

        return Value();
    }

    Value width(std::shared_ptr<Environment> env, Token cmd, std::vector<Value> args)
    {
        setLabel(env);

        if (args.size() != 1)
            throw RuntimeError(cmd, "takes one arguments");

        std::string size = args[0].GetContent();

        if (!util::is_valid_sizing(size))
            throw RuntimeError(cmd, "formatting invalid");

        if (util::dgonly(size))
            size += "%";

        width_ = size;

        return Value();
    }

    Value height(std::shared_ptr<Environment> env, Token cmd, std::vector<Value> args)
    {
        setLabel(env);

        if (args.size() != 1)
            throw RuntimeError(cmd, "takes one arguments");

        std::string size = args[0].GetContent();

        if (!util::is_valid_sizing(size))
            throw RuntimeError(cmd, "formatting invalid");

        if (util::dgonly(size))
            size += "%";

        height_ = size;

        return Value();
    }

    Value caption(std::shared_ptr<Environment> env, Token cmd, std::vector<Value> args)
    {
        setLabel(env);

        if (args.size() != 1)
            throw RuntimeError(cmd, "takes one arguments");

        caption_ = args[0].GetContent();
        return Value();
    }

    Value alttext(std::shared_ptr<Environment> env, Token cmd, std::vector<Value> args)
    {
        setLabel(env);

        if (args.size() != 1)
            throw RuntimeError(cmd, "takes one arguments");

        alttext_ = args[0].GetContent();
        return Value();
    }

    Value label(std::shared_ptr<Environment> env, Token cmd, std::vector<Value> args)
    {
        setLabel(env);

        if (args.size() != 1)
            throw RuntimeError(cmd, "takes one arguments");

        env->SetGlobal(args[0].GetContent(), Value(t_string, label_));
        return Value();
    }

    void StartEnvironment(Token, Value){};
    Value EndEnvironment(Token, Value)
    {
        /// Attributes
        std::vector<Value> attributes;

        if (width_ == "" && height_ == "")
        {
            attributes.emplace_back("width", "100%");
        }

        if (width_ != "")
        {
            attributes.emplace_back("width", width_);
        }

        if (height_ != "")
        {
            attributes.emplace_back("height", height_);
        }

        attributes.emplace_back("alt", alttext_);
        attributes.emplace_back("src", graphic_);

        Value image("img", std::move(attributes));

        // Figure env
        if (caption_ != "")
        {
            std::string header = "Figure ";

            try {
                header = Get("header").GetContent();
            }
            catch(...) {}

            std::vector<Value> figcaption;
            figcaption.emplace_back(t_string, header + label_ + ": " + caption_);

            std::vector<Value> figure;
            figure.push_back(std::move(image));
            figure.emplace_back("figcaption", std::move(figcaption));

            return Value("figure", std::move(figure));
        }
        else
        {
            return std::move(image);
        }
    }
};
