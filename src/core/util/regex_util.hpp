#pragma once

#include <string>
#include <regex>

namespace util
{

// Copyright (c) 2010-2018 Diego Perini (http://www.iport.it)
// Modified by Thijs Miedema for C+,
const std::string uripattern[] = {
    "^",
    // protocol identifier (optional)
    // short syntax still required
    "(?:(?:(?:https?|ftp):)?\\/\\/)?",
    // user:pass BasicAuth (optional)
    "(?:\\S+(?::\\S*)?@)?",
    "(?:",
    // IP address exclusion
    // private & local networks
    "(?!(?:10|127)(?:\\.\\d{1,3}){3})",
    "(?!(?:169\\.254|192\\.168)(?:\\.\\d{1,3}){2})",
    "(?!172\\.(?:1[6-9]|2\\d|3[0-1])(?:\\.\\d{1,3}){2})",
    // IP address dotted notation octets
    // excludes loopback network 0.0.0.0
    // excludes reserved space >= 224.0.0.0
    // excludes network & broacast addresses
    // (first & last IP address of each class)
    "(?:[1-9]\\d?|1\\d\\d|2[01]\\d|22[0-3])",
    "(?:\\.(?:1?\\d{1,2}|2[0-4]\\d|25[0-5])){2}",
    "(?:\\.(?:[1-9]\\d?|1\\d\\d|2[0-4]\\d|25[0-4]))",
    "|",
    // host & domain names, may end with dot
    // can be replaced by a shortest alternative
    // (?![-_])(?:[-\\w\\u00a1-\\uffff]{0,63}[^-_]\\.),
    "(?:",
    "(?:",
    "[a-z0-9\\u00a1-\\uffff]",
    "[a-z0-9\\u00a1-\\uffff_-]{0,62}",
    ")?",
    "[a-z0-9\\u00a1-\\uffff]\\.",
    ")+",
    // TLD identifier name, may end with dot
    "(?:[a-z\\u00a1-\\uffff]{2,}\\.?)",
    ")",
    // port number (optional)
    "(?::\\d{2,5})?",
    // resource path (optional)
    "(?:[/?#]\\S*)?",
    "$"};

bool is_valid_url(const std::string &url)
{
    std::string pat;
    for (const std::string &s : uripattern)
        pat += s;

    std::regex r(pat);

    return std::regex_match(url.begin(), url.end(), r);
}

bool is_valid_subpath(const std::string &path)
{
    std::regex r("^/?\\w+(?:/\\w*)*(\\.\\w*)?$");
    return std::regex_match(path.begin(), path.end(), r);
}

bool is_valid_sizing(const std::string &sizing)
{
    std::regex r("^\\d*(cm|mm|in|pt|px|pc|em|ex|ch|rem|vw|vh|vmin|vmax)?$");
    return std::regex_match(sizing.begin(), sizing.end(), r);
}

} // namespace util
