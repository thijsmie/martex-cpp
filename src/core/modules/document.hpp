#pragma once

#include "core/util/cppenvironment.hpp"
#include <string>
#include <memory>

class DocumentEnvironment : public util::CppEnvironment<DocumentEnvironment>
{
  private:
    bool m_adminmode;
    int m_labeling, m_chapter, m_section, m_subsection, m_subsubsection;

  public:
    DocumentEnvironment(std::shared_ptr<Environment> parent, bool adminmode);
    void StartEnvironment(Token, Value) {}
    Value EndEnvironment(Token, Value content);

    Value title(Token, std::vector<Value>);
    Value chapter(Token, std::vector<Value>);
    Value section(Token, std::vector<Value>);
    Value subsection(Token, std::vector<Value>);
    Value subsubsection(Token, std::vector<Value>);
    
    Value label(Token, std::vector<Value>);
    Value labeling(Token, std::vector<Value>);
};

