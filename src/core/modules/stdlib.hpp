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
    bool m_labeling, m_haspage, m_hasdocument, m_adminmode;
    
  public:
    StdLib(bool adminmode);

    std::vector<std::string> GetEnvs();
    std::shared_ptr<Environment> MakeEnv(std::string, std::shared_ptr<Environment>);

    Value easy(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value full(std::shared_ptr<Environment>, Token, std::vector<Value>);

    Value textbf(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value textit(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value textul(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value textsc(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value textms(std::shared_ptr<Environment>, Token, std::vector<Value>);

    Value ref(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value newline(std::shared_ptr<Environment>, Token, std::vector<Value>);

    Value command(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value href(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value define(std::shared_ptr<Environment>, Token, std::vector<Value>);

    Value colour(std::shared_ptr<Environment>, Token, std::vector<Value>);
    
    Value sizew(std::shared_ptr<Environment>, Token, std::vector<Value>);
    Value sizeh(std::shared_ptr<Environment>, Token, std::vector<Value>);
};
