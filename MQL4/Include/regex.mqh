//+------------------------------------------------------------------+
//|                                                        regex.mqh |
//|                              Copyright (c) 2019, Serge Aleynikov |
//|                                           https://www.mqtllc.com |
//+------------------------------------------------------------------+
#property copyright "Copyright (c) 2019, Serge Aleynikov"
#property link      "https://www.mqtllc.com"
#property strict

#ifndef _REGEX_MQH_
#define _REGEX_MQH_

//+------------------------------------------------------------------+
//| defines                                                          |
//+------------------------------------------------------------------+

// See http://www.cplusplus.com/reference/regex/basic_regex/basic_regex/
enum ENUM_REX_INIT {
  REX_NONE          = 0,
  REX_ECMA_SCRIPT   = 0x0001,
  REX_BASIC         = 0x0002,
  REX_EXTENDED      = 0x0004,
  REX_AWK           = 0x0008,
  REX_GREP          = 0x0010,
  REX_EGREP         = 0x0020,
  REX_ICASE         = 0x0100,
  REX_NOSUBS        = 0x0200,
  REX_OPTIMIZE      = 0x0400,
  REX_COLLATE       = 0x0800
};

enum ENUM_REX_MATCH {
  REX_DEF           = 0x0000,
  REX_NOT_BOL       = 0x0001, // The first character is not considered a beginning of line ("^" doesn't match)
  REX_NOT_EOL       = 0x0002, // The last character is not considered an end of line ("$" does not match).
  REX_NOT_BOW       = 0x0004, // The escape sequence "\b" does not match as a beginning-of-word.
  REX_NOT_EOW       = 0x0008, // The escape sequence "\b" does not match as an end-of-word.
  REX_ANY           = 0x0010, // Any match is acceptable if more than one match is possible.
  REX_NOT_NULL      = 0x0020, // Empty sequences do not match.
  REX_CONTINUOUS    = 0x0040, // The expression must match a sub-sequence that begins at the first character.
  REX_PREV_AVAIL    = 0x0100, // One or more characters exist before the first one. (match_not_bol and match_not_bow are ignored)
  REX_FMT_SED       = 0x0400,
  REX_FMT_NO_COPY   = 0x0800,
  REX_FMT_FIRST_ONLY= 0x1000,
};

#define RexMatchObj int

//+------------------------------------------------------------------+
//| DLL imports                                                      |
//+------------------------------------------------------------------+
#import "mt4-regex.dll"

RexMatchObj ReInit     (string regex, int flags=REX_ECMA_SCRIPT);
bool        ReUpdate   (RexMatchObj regex, string rex, int flags=REX_ECMA_SCRIPT);
int         ReMatch    (RexMatchObj regex, string subject, int flags=REX_DEF);
int         ReMatchNext(RexMatchObj regex);
// Get match number "num" string
// Return number of characters copied to match. If that number is greater or equal than "max_len"
// no data is copied, and the function needs to be called again with a larger "match" buffer.
int         ReGetMatch (RexMatchObj regex, int num, int& position, string& match, int max_len);
// Get replacement string
// Return number of characters copied to match. If that number is greater or equal than "max_len"
// no data is copied, and the function needs to be called again with a larger "match" buffer.
int         ReGetReplacement(RexMatchObj regex, int& position, string& output, int max_len);
int         ReReplace (RexMatchObj regex, string subject, string replacement, int flags);
void        ReFinalize(RexMatchObj regex);

#import

#endif // _REGEX_MQH_