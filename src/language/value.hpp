#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <cstdint>

#include "util/buffer.hpp"


enum ValueType : int
{
    t_null = 0,
    t_string = 1,
    t_ampersand = 2,
    t_break = 3,
    t_multi = 4,
    t_html = 5,
    t_attr = 6,
    t_info = 7
};

class Value
{
  public:
    Value(ValueType, std::string, std::string);
    Value(ValueType, std::string);
    Value(ValueType);
    
    //multi
    Value(std::vector<Value>);

    //null
    Value();

    //html
    Value(std::string, Value);
    Value(std::string, std::vector<Value>);

    //info
    Value(ValueType, std::string, std::vector<Value>);

    //attr
    Value(std::string, std::string);

    //frombytes
    Value(const unsigned char *b, uint32_t &pos);

    // disable copy
    Value(const Value&) = delete;
    Value& operator=(const Value&) = delete;
    // enable move
    Value(Value&&) = default;
    Value& operator=(Value&&) = default;

    std::string GetContent() const;
    std::string GetTag() const;
    std::string GetRawContent() const;
    ValueType GetType() const;

    bool IsPlain() const;

    uint32_t ByteSize() const;
    void WriteOut(unsigned char *b, uint32_t &pos) const;
    void SanitizeMulti();

    Value explicit_copy() const;

    static Value asString(const Value& v);
    static Value asString(const std::vector<Value>& vs);
  private:
    ValueType type;
    std::string tag;
    std::string content;
  public:
    std::vector<Value> multicontent;
};

std::string ValueTypeName(ValueType v);
