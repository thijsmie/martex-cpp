#include <phpcpp.h>

#include "phpglobal.hpp"
#include "phpmartex.hpp"
#include "phpvaluecasts.hpp"

/**
 *  Php modules need to expose a pure-C function get_module
 */
extern "C"
{

    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  structure with all the details and features of martex
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module()
    {
        // The extension will stay in memory for the duration of the php process
        static Php::Extension extension("martex", "0.2");

        // Create MarTeX namespace
        Php::Namespace texspace("MarTeX");

        // Register the MarTeX object and expose methods
        Php::Class<MarTeX> martex("MarTeX");
        martex.method<&MarTeX::Parse>("parse", {Php::ByVal("text", Php::Type::String)});
        martex.method<&MarTeX::RegisterModule>("registerModule", {Php::ByVal("module", Php::Type::String)});
        martex.method<&MarTeX::AllowPage>("allowPage");
        martex.method<&MarTeX::HasError>("hasError");
        martex.method<&MarTeX::GetErrors>("getError");
        martex.method<&MarTeX::GetResult>("getResult");

        Php::Class<PhpModuleBase> base1("Module");
        base1.method<&PhpModuleBase::globals>("globals");
        base1.method<&PhpModuleBase::environments>("environments");

        Php::Class<PhpEnvironmentBase> base2("Environment");
        base2.method<&PhpEnvironmentBase::__construct>("__construct", {Php::ByVal("module", Php::Type::Object)});
        base2.method<&PhpEnvironmentBase::locals>("locals");
        base2.method<&PhpEnvironmentBase::begin>("begin", {Php::ByVal("bracket_argument", Php::Type::Resource)});
        base2.method<&PhpEnvironmentBase::end>("end", {Php::ByVal("content", Php::Type::Resource)});

        Php::Class<PhpGlobalEnvironment> base3("GlobalEnv");

        // Add classes to namespace
        texspace.add(std::move(martex));
        texspace.add(std::move(base1));
        texspace.add(std::move(base2));
        texspace.add(std::move(base3));

        // Helpers
        texspace.add<html>("html");
        texspace.add<mstr>("mstr");
        texspace.add<attr>("attribute");
        texspace.add<attr>("attr");
        texspace.add<ampersand>("ampersand");
        texspace.add<ampersand>("amp");
        texspace.add<newline>("newline");
        texspace.add<PhpAsBytes>("bytes");

        // Add vartype constants to namespace
        texspace.add(Php::Constant("TypeNull", ValueType::t_null));
        texspace.add(Php::Constant("TypeString", ValueType::t_string));
        texspace.add(Php::Constant("TypeAmpersand", ValueType::t_ampersand));
        texspace.add(Php::Constant("TypeBreak", ValueType::t_break));
        texspace.add(Php::Constant("TypeMulti", ValueType::t_multi));
        texspace.add(Php::Constant("TypeHtml", ValueType::t_html));
        texspace.add(Php::Constant("TypeAttr", ValueType::t_attr));

        // Add argtype constants to namespace
        texspace.add(Php::Constant("Full", ArgType::Full));
        texspace.add(Php::Constant("Text", ArgType::Text));
        texspace.add(Php::Constant("Bytes", ArgType::Bytes));
        texspace.add(Php::Constant("PlainFull", ArgType::PlainFull));
        texspace.add(Php::Constant("PlainText", ArgType::PlainText));
        texspace.add(Php::Constant("PlainBytes", ArgType::PlainBytes));
        texspace.add(Php::Constant("MoreFull", ArgType::MoreFull));
        texspace.add(Php::Constant("MoreText", ArgType::MoreText));
        texspace.add(Php::Constant("MoreBytes", ArgType::MoreBytes));
        texspace.add(Php::Constant("MorePlainFull", ArgType::MorePlainFull));
        texspace.add(Php::Constant("MorePlainText", ArgType::MorePlainText));
        texspace.add(Php::Constant("MorePlainBytes", ArgType::MorePlainBytes));

        // Add namespace MarTeX to extension
        extension.add(std::move(texspace));

        return extension;
    }
}
