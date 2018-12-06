#include <phpcpp.h>
#include "martex.hpp"

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
        static Php::Extension extension("martex", "0.1");/*

        // Create MarTeX namespace
        Php::Namespace texspace("MarTeX");

        // Register the MarTeX object and expose methods
        Php::Class<MarTeX> martex("MarTeX");
        martex.method<&MarTeX::__construct>("__construct", {
            Php::ByRef("tex", Php::Type::Object), 
            Php::ByRef("module", Php::Type::Object)
        });
        martex.method<&MarTeX::parse>("parse", {Php::ByVal("text", Php::Type::String)});
        martex.method<&MarTeX::registerModule>("registerModule", {Php::ByRef("module", Php::Type::Object)});
        martex.method<&MarTeX::hasError>("hasError");
        martex.method<&MarTeX::getError>("getError");
        martex.method<&MarTeX::getResult>("getResult");

        Php::Class<PhpEnvironmentBase> base("Environment");

        // Add classes to namespace
        texspace.add(std::move(martex));
        texspace.add(std::move(base));

        // Add constants to namespace
        texspace.add(Php::Constant("TypeAny"), ValueType::t_any);
        texspace.add(Php::Constant("TypeNull"), ValueType::t_null);
        texspace.add(Php::Constant("TypeString"), ValueType::t_string);
        texspace.add(Php::Constant("TypeAmpersand"), ValueType::t_ampersand);
        texspace.add(Php::Constant("TypeBreak"), ValueType::t_break);
        texspace.add(Php::Constant("TypeMulti"), ValueType::t_multi);

        // Add namespace MarTeX to extension
        extension.add(std::move(texspace));
        */
        return extension;
    }
}
