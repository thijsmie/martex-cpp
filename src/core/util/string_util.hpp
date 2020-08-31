#pragma once

#include <algorithm>
#include <cctype>
#include <locale>

namespace util
{

// trim from start (in place)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                return !std::isspace(ch);
            }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
                return !std::isspace(ch);
            })
                .base(),
            s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s)
{
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s)
{
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s)
{
    trim(s);
    return s;
}

// wsonly
static inline bool wsonly(const std::string &s)
{
    auto f = [](unsigned char const c) { return std::isspace(c); };
    return std::all_of(s.begin(), s.end(), f);
}

// dgonly
static inline bool dgonly(const std::string &s)
{
    auto f = [](unsigned char const c) { return std::isdigit(c); };
    return std::all_of(s.begin(), s.end(), f);
}

// cmdonly
static inline bool cmdonly(const char &c)
{
    return (c > 47 && c < 58) || (c > 64 && c < 91) || (c > 96 && c < 123) || c == 95;
}

static inline bool cmdonly(const std::string &s)
{
    auto f = [](unsigned char const c) { return cmdonly(c); };
    return std::all_of(s.begin(), s.end(), f);
}

// hexonly
static inline bool hexonly(const char &c)
{
    return (c > 47 && c < 58) || (c > 64 && c < 71) || (c > 96 && c < 103);
}

static inline bool hexonly(const std::string &s)
{
    auto f = [](unsigned char const c) { return hexonly(c); };
    return std::all_of(s.begin(), s.end(), f);
}

static inline bool startswith(const std::string &string, const std::string &prefix)
{
    return (string.rfind(prefix, 0) == 0);
}

} // namespace util