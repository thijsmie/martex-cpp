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
        static Php::Extension extension("martex", "0.1");

        // Register the MarTeX object and expose methods
        Php::Class<MarTeX> martex("MarTeX");

        // void MarTeX->parse(string $text)
        martex.method<&MarTeX::parse>("parse", {Php::ByVal("text", Php::Type::String)});

        // void MarTex->registerModule(object $module)
        martex.method<&MarTeX::registerModule>("registerModule", {Php::ByRef("module", Php::Type::Object)});

        // bool MarTeX->hasError()
        martex.method<&MarTeX::hasError>("hasError");

        // string MarTeX->getError()
        martex.method<&MarTeX::getError>("getError");

        // string MarTeX->getResult()
        martex.method<&MarTeX::getResult>("getResult");

        return extension;
    }
}
