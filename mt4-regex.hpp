// mt4-regex.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
/*
// See http://www.cplusplus.com/reference/regex/basic_regex/basic_regex/
enum RegexInitOpts : int {
  NONE,
  ICASE,
  NOSUBS,
  OPTIMIZE,
  COLLATE,
  ECMA_SCRIPT,
  BASIC,
  EXTENDED,
  AWK,
  GREP,
  EGREP,
};

enum RegexOpts : int {
  NONE,
  NOT_BOL,    // The first character is not considered a beginning of line ("^" doesn't match)
  NOT_EOL,    // The last character is not considered an end of line ("$" does not match).
  NOT_BOW,    // The escape sequence "\b" does not match as a beginning-of-word.
  NOT_EOW,    // The escape sequence "\b" does not match as an end-of-word.
  ANY,        // Any match is acceptable if more than one match is possible.
  NOT_NULL,   // Empty sequences do not match.
  CONTINUOUS, // The expression must match a sub-sequence that begins at the first character.
  PREV_AVAIL, // One or more characters exist before the first one. (match_not_bol and match_not_bow are ignored)
};
*/
#include <regex>

using RegExObj = void*;

// When building curl-mt4.dll, add BUILDING_MT4CURL pre-processor define
#ifdef BUILDING_LIB
#define MT4EXPORT __declspec(dllexport)
#else
#define MT4EXPORT __declspec(dllimport)
#endif

#define APIDECL(type) MT4EXPORT type __stdcall

extern "C" {

APIDECL(RegExObj) ReInit     (const wchar_t* regex, std::regex::flag_type flags = std::regex::flag_type::ECMAScript);
APIDECL(bool)     ReUpdate   (RegExObj re, const wchar_t* regex, std::regex::flag_type flags = std::regex::flag_type::ECMAScript);
APIDECL(int     ) ReMatch    (RegExObj regex, const wchar_t* subject,
                              std::regex_constants::match_flag_type flags = std::regex_constants::match_flag_type::format_default);
APIDECL(int     ) ReMatchNext(RegExObj regex);

// Get match number "num" string
// Return number of characters copied to match. If that number is greater or equal than "max_len"
// no data is copied, and the function needs to be called again with a larger "match" buffer.
APIDECL(int     ) ReGetMatch (RegExObj regex, int num, int& position, wchar_t* match, int max_len);

// Get replacement string
// Return number of characters copied to match. If that number is greater or equal than "max_len"
// no data is copied, and the function needs to be called again with a larger "match" buffer.
APIDECL(int     ) ReGetReplacement(RegExObj regex, wchar_t* output, int max_len);
APIDECL(int     ) ReReplace  (RegExObj regex, const wchar_t* subject, const wchar_t* replacement,
                              std::regex_constants::match_flag_type flags = std::regex_constants::match_flag_type::format_default);
APIDECL(void    ) ReFinalize (RegExObj regex);

}