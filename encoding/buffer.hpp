#pragma once

#include <cstdint>
#include <cstring>
#include <string>

void serialize_uint32_t(unsigned char *buffer, int &pos, uint32_t value)
{
    buffer[pos++] = value >> 24;
    buffer[pos++] = value >> 16;
    buffer[pos++] = value >> 8;
    buffer[pos++] = value >> 0;
}

uint32_t deserialize_uint32_t(const unsigned char *buffer, int &pos)
{
    uint32_t ret = 0;
    ret |= buffer[pos++] << 24;
    ret |= buffer[pos++] << 16;
    ret |= buffer[pos++] << 8;
    ret |= buffer[pos++];
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