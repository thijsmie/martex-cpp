#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <vector>


enum ValueType : int
{
    t_any = -1,
    t_null = 0,
    t_string = 1,
    t_ampersand = 2,
    t_break = 3,
    t_multi = 4
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
