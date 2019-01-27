#include <string>
#include <iostream>
#include <cstdlib>

#include "core/modules/stdlib.hpp"
#include "core/language/transpiler.hpp"


int main()
{
    std::string text;
    for (std::string line; std::getline(std::cin, line);) {
        text += line + "\n";
    }

    std::vector<std::shared_ptr<Module>> module_inst;
    module_inst.reserve(1);
    module_inst.push_back(std::make_shared<StdLib>(true));

    // Run the transpiler
    Transpiler transpiler(module_inst);
    transpiler.Parse(text);

    if (transpiler.HasError())
    {
        std::cerr << transpiler.GetErrors() << std::endl;
        std::cerr.flush();
    }
    
    std::cout << transpiler.GetResult();
    std::cout.flush();
}

