#include "buffer.hpp"

const uint8_t t_table[] = {
    43, 45, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
    65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
    77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88,
    89, 90, 97, 98, 99, 100, 101, 102, 103, 104, 105,
    106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 
    116, 117, 118, 119, 120, 121, 122
};

std::map<uint8_t, uint8_t> r_table {
    {43, 0}, {45, 1}, {48, 2}, {49, 3}, {50, 4}, {51, 5},
    {52, 6}, {53, 7}, {54, 8}, {55, 9}, {56, 10}, {57, 11},
    {65, 12}, {66, 13}, {67, 14}, {68, 15}, {69, 16}, {70, 17},
    {71, 18}, {72, 19}, {73, 20}, {74, 21}, {75, 22}, {76, 23},
    {77, 24}, {78, 25}, {79, 26}, {80, 27}, {81, 28}, {82, 29},
    {83, 30}, {84, 31}, {85, 32}, {86, 33}, {87, 34}, {88, 35},
    {89, 36}, {90, 37}, {97, 38}, {98, 39}, {99, 40}, {100, 41},
    {101, 42}, {102, 43}, {103, 44}, {104, 45}, {105, 46}, {106, 47},
    {107, 48}, {108, 49}, {109, 50}, {110, 51}, {111, 52}, {112, 53},
    {113, 54}, {114, 55}, {115, 56}, {116, 57}, {117, 58}, {118, 59},
    {119, 60}, {120, 61}, {121, 62}, {122, 63}
};

void serialize_byte(unsigned char *buffer, uint32_t &pos, uint8_t value)
{
    buffer[pos++] = t_table[value];
}

uint8_t deserialize_byte(const unsigned char *buffer, uint32_t &pos)
{
    return r_table[(uint8_t)buffer[pos++]];
}

void serialize_uint32_t(unsigned char *buffer, uint32_t &pos, uint32_t value)
{
    buffer[pos++] = t_table[((value >> 18) & 63)];
    buffer[pos++] = t_table[((value >> 12) & 63)];
    buffer[pos++] = t_table[((value >> 6) & 63)];
    buffer[pos++] = t_table[((value >> 0) & 63)];
}

uint32_t deserialize_uint32_t(const unsigned char *buffer, uint32_t &pos)
{
    uint32_t ret = 0;
    ret |= r_table[buffer[pos++]] << 18;
    ret |= r_table[buffer[pos++]] << 12;
    ret |= r_table[buffer[pos++]] << 6;
    ret |= r_table[buffer[pos++]];
    return ret;
}

void serialize_sized_string(unsigned char *buffer, uint32_t &pos, const char *src, const uint32_t len)
{
    serialize_uint32_t(buffer, pos, len);
    memcpy((char *)buffer + pos, src, len);
    pos += len;
}

std::string deserialize_sized_string(const unsigned char *buffer, uint32_t &pos)
{
    const uint32_t len = deserialize_uint32_t(buffer, pos);
    std::string res((char *)buffer + pos, len);
    pos += len;
    return res;
}