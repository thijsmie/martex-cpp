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
    t_multi = 4,
    t_html = 5,
    t_attr = 6
};

class Value
{
  public:
    Value(ValueType, std::string);
    
    //multi
    Value(std::vector<Value>);
    Value(std::initializer_list<Value>);

    //null
    Value();

    //html
    Value(std::string, std::vector<Value>);
    Value(std::string, std::initializer_list<Value>);

    //attr
    Value(std::string, std::string);

    std::string GetContent() const;
    std::vector<Value> GetValues() const;
    ValueType GetType() const;

    Value Flattened();
  private:
    ValueType type;
    std::string content;
    std::vector<Value> multicontent;
};
