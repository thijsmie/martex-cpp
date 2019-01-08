#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <map>

void serialize_byte(unsigned char *buffer, uint32_t &pos, uint8_t value);
uint8_t deserialize_byte(const unsigned char *buffer, uint32_t &pos);

void serialize_uint32_t(unsigned char *buffer, uint32_t &pos, uint32_t value);
uint32_t deserialize_uint32_t(const unsigned char *buffer, uint32_t &pos);

void serialize_sized_string(unsigned char *buffer, uint32_t &pos, const char* src, const uint32_t len);
std::string deserialize_sized_string(const unsigned char *buffer, uint32_t &pos);