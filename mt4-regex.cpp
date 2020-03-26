// mt4-regex.cpp : Defines the entry point for the application.
//

#include "mt4-regex.hpp"
#include <regex>

#ifndef _WIN32 // MSC_VER

uint64_t WINAPI GetTimeOfDay() {
  timeval tv;
  gettimeofday(&tv, nullptr);
  return uint64_t(tv.tv_sec)*1000000 + tv.tv_usec;
}

#else

#include <winsock2.h>
#include <Windows.h>
#include <ws2ipdef.h>
#include <libloaderapi.h>

typedef void(__stdcall* PgGetSystemTimeFn) (LPFILETIME);

static PgGetSystemTimeFn init_gettimeofday()
{
  /*
   * Because it's guaranteed that kernel32.dll will be linked into our
   * address space already, we don't need to LoadLibrary it and worry about
   * closing it afterwards, so we're not using Pg's dlopen/dlsym() wrapper.
   *
   * We'll just look up the address of GetSystemTimePreciseAsFileTime if
   * present.
   *
   * While we could look up the Windows version and skip this on Windows
   * versions below Windows 8 / Windows Server 2012 there isn't much point,
   * and determining the windows version is its self somewhat Windows
   * version and development SDK specific...
   */
  auto lib = GetModuleHandleA("kernel32.dll");
  if (lib) {
    auto p = GetProcAddress(lib, "GetSystemTimePreciseAsFileTime");
    if (p)  return (PgGetSystemTimeFn)p;
  }
  /*
   * The expected error from GetLastError() is ERROR_PROC_NOT_FOUND, if
   * the function isn't present. No other error should occur.
   * It's not a serious problem, so just silently fall back to
   * GetSystemTimeAsFileTime irrespective of why the failure occurred.
   */
  return &GetSystemTimeAsFileTime;
}

uint64_t __stdcall GetTimeOfDay()
{
  static PgGetSystemTimeFn s_get_system_time_fun = init_gettimeofday();

  static const uint64_t EPOCH = 116444736000000000ULL;
  ULARGE_INTEGER largeInt;
  largeInt.QuadPart = 0;
  FILETIME fileTime;
  s_get_system_time_fun(&fileTime);
  largeInt.HighPart = fileTime.dwHighDateTime;
  largeInt.LowPart = fileTime.dwLowDateTime;
  return (largeInt.QuadPart - EPOCH) / 10;
}
#endif

using namespace std;

struct RegEx {
  RegEx(const wchar_t* str, regex::flag_type flags = regex::flag_type::ECMAScript)
  {
    Update(str, flags);
  }

  wregex&           Rex()                       { return m_rex; }
  const wregex&     Rex()                 const { return m_rex; }
  wcmatch&          SM()                        { return m_sm;  }
  wcregex_iterator& It()                        { return m_it;  }
  void              It(wcregex_iterator&& it)   { m_it = move(it); if (!IsMatchEnd()) m_sm = *m_it;  }
  wstring&          Subject()                   { return m_subject; }
  const wstring&    Subject()             const { return m_subject; }
  void              Subject(const wchar_t* str) { m_subject = str;  }

  bool              IsMatchEnd()          const { return m_it == s_empty; }
  bool              ItNext() { ++m_it; auto b = !IsMatchEnd(); if (b) m_sm = *m_it; return b; }

  bool              Update(const wchar_t* str, regex::flag_type flags = regex::flag_type::ECMAScript) {
                      try {
                        wregex re(str, flags);
                        m_rex.swap(re);
                        m_err = static_cast<regex_constants::error_type>(-1);
                      }
                      catch (regex_error & e) {
                        m_err = e.code();
                      }
                      return int(m_err) < 0;
                    }

  void              Reset() {
                      wcmatch m; m_sm.swap(m);
                      m_it  = wcregex_iterator();
                      m_err = static_cast<regex_constants::error_type>(-1);
                    }
  const wstring&    ReplOutput() { return m_repl_output;    }
  void              ReplOutput(wstring&& str) { m_repl_output = std::move(str); }

  regex_constants::error_type Error() const { return m_err; }
private:
  wregex  m_rex;
  wcmatch m_sm;
  wstring m_subject;
  wcregex_iterator m_it;
  wstring m_repl_output;
  regex_constants::error_type m_err;

  static const wcregex_iterator s_empty;
};

const wcregex_iterator RegEx::s_empty;

extern "C" {

RegExObj __stdcall ReInit(const wchar_t* regex, regex::flag_type flags) {
  auto p = new RegEx(regex, flags);
  if (int(p->Error()) >= 0) {
    delete p;
    return nullptr;
  }
  return reinterpret_cast<RegExObj>(p);
}

bool __stdcall ReUpdate(RegExObj rex, const wchar_t* regex, regex::flag_type flags) {
  RegEx* re = reinterpret_cast<RegEx*>(rex);
  if (!re)
    return false;
  return re->Update(regex, flags);
}

int __stdcall ReMatch(RegExObj regex, const wchar_t* subject, regex_constants::match_flag_type flags) {
  RegEx* re = reinterpret_cast<RegEx*>(regex);
  if (!re)
    return -1;
  re->Subject(subject);
  re->It(wcregex_iterator(re->Subject().c_str(), re->Subject().c_str()+re->Subject().size(), re->Rex(), flags));
  return re->IsMatchEnd() ? 0 : int(re->SM().size());
  //return regex_match(subject, re->SM(), re->Rex(), flags) ? re->SM().size() : 0;
}

int __stdcall ReMatchNext(RegExObj regex) {
  RegEx* re = reinterpret_cast<RegEx*>(regex);
  if (!re)
    return false;

  return re->ItNext() ? int(re->SM().size()) : 0;
}

int __stdcall ReGetMatch(RegExObj regex, int num, int& position, wchar_t* match, int max_len) {
  RegEx* re = reinterpret_cast<RegEx*>(regex);
  if (!re || num >= int(re->SM().size()))
    return 0;
  position = int(re->SM().position(num));
  auto&  s = re->SM().str(num);
  if (int(s.size()) < max_len)
    wcsncpy_s(match, max_len, s.c_str(), s.size());
  return int(s.size());
}

int __stdcall ReReplace(RegExObj regex, const wchar_t* subject, const wchar_t* replacement, regex_constants::match_flag_type flags) {
  RegEx* re = reinterpret_cast<RegEx*>(regex);
  if (!re)
    return -1;
  re->ReplOutput(std::regex_replace(subject, re->Rex(), replacement, static_cast<regex_constants::match_flag_type>(flags)));
  return int(re->ReplOutput().length());
}

int __stdcall ReGetReplacement(RegExObj regex, wchar_t* output, int max_len) {
  RegEx* re = reinterpret_cast<RegEx*>(regex);
  if (!re)
    return 0;
  if (int(re->ReplOutput().length()) < max_len)
    wcsncpy_s(output, max_len, re->ReplOutput().c_str(), re->ReplOutput().length());
  return int(re->ReplOutput().length());
}

void __stdcall ReFinalize(RegExObj regex) {
  delete reinterpret_cast<RegEx*>(regex);
}

} // extern "C"