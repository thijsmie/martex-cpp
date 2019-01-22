#pragma once

#include "core/util/cppmodule.hpp"
#include "core/language/module.hpp"

#include <vector>
#include <map>
#include <string>
#include <algorithm>

class StdLib : public util::CppModule<StdLib>
{
  private:
    int _chapter, _section, _subsection, _subsubsection;
    bool _labeling, _haspage, _hasdocument;
    
  public:
    StdLib(bool allow_page);

    std::vector<std::string> GetEnvs();
    std::shared_ptr<Environment> MakeEnv(std::string, std::shared_ptr<Environment>);

    Value easy(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value full(std::shared_ptr<Environment>, Token, std::vector<Value>);

    Value title(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value chapter(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value section(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value subsection(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value subsubsection(std::shared_ptr<Environment>, Token, std::vector<Value>);

    Value textbf(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value textit(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value textul(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value textsc(std::shared_ptr<Environment>, Token, std::vector<Value>);

    Value ref(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value label(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value labeling(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value newline(std::shared_ptr<Environment>, Token, std::vector<Value>);

    Value command(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value href(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value define(std::shared_ptr<Environment>, Token, std::vector<Value>);

    Value colour(std::shared_ptr<Environment>, Token, std::vector<Value>);
};
