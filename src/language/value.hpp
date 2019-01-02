#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <cstdint>

#include "encoding/buffer.hpp"


enum ValueType : int
{
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

    //frombytes
    Value(const unsigned char *b, uint32_t &pos);

    std::string GetContent() const;
    std::string GetTag() const;
    std::string GetRawContent() const;
    std::vector<Value> GetValues() const;
    ValueType GetType() const;

    bool IsPlain() const;

    Value Flattened() const;

    uint32_t ByteSize() const;
    void WriteOut(unsigned char *b, uint32_t &pos) const;
  private:
    ValueType type;
    std::string tag;
    std::string content;
    std::vector<Value> multicontent;
};

std::string ValueTypeName(ValueType v);
