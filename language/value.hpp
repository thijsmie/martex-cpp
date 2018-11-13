#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <memory>
#include <variant>
#include <iostream>


enum ValueType
{
    t_null,
    t_string,
    t_ampersand,
    t_break,
    t_multi
};

class Value
{
  public:
    Value(ValueType, std::string);
    Value(std::vector<Value>);
    Value();

    std::string GetContent() const;
    std::vector<Value> GetValues() const;
    ValueType GetType() const;
  private:
    ValueType type;
    std::string content;
    std::vector<Value> multicontent;
};

#endif //VALUE_H