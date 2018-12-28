#pragma once

#include <cstdint>
#include <cstring>
#include <string>

void serialize_byte(unsigned char *buffer, int &pos, int value)
{
    buffer[pos++] = value + 32;
}

int deserialize_byte(const unsigned char *buffer, int &pos)
{
    return buffer[pos++] - 32;
}

void serialize_uint32_t(unsigned char *buffer, int &pos, uint32_t value)
{
    buffer[pos++] = (value >> 21) + 32;
    buffer[pos++] = (value >> 14) + 32;
    buffer[pos++] = (value >> 7) + 32;
    buffer[pos++] = (value >> 0) + 32;
}

uint32_t deserialize_uint32_t(const unsigned char *buffer, int &pos)
{
    uint32_t ret = 0;
    ret |= (buffer[pos++] - 32) << 21;
    ret |= (buffer[pos++] - 32) << 14;
    ret |= (buffer[pos++] - 32) << 7;
    ret |= (buffer[pos++] - 32);
    return ret;
}

void serialize_sized_string(unsigned char *buffer, int &pos, const char* src, const uint32_t len)
{
    serialize_uint32_t(buffer, pos, len);
    memcpy((char*)buffer+pos, src, len);
    pos += len;
}

std::string deserialize_sized_string(const unsigned char *buffer, int &pos)
{
    const uint32_t len = deserialize_uint32_t(buffer, pos);
    std::string res((char*)buffer + pos, len);
    pos += len;
    return res;
}