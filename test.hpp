#pragma once

#include <string>
#include <sstream>
#include <stdexcept>

#ifdef _WIN32
# define __PRETTY_FUNCTION__ __FUNCTION__
#endif

static bool s_exit_test_on_failure = true;

#define ASSERT_TRUE( condition )                                    \
{                                                                   \
  s_exit_test_on_failure = true;                                    \
  if( !( condition ) )                                              \
  {                                                                 \
    std::stringstream s;                                            \
    s << __FILE__ << ':' << __LINE__ << ':'                         \
      << std::string(__PRETTY_FUNCTION__)                           \
      << ": false condition";                                       \
    throw std::runtime_error(s.str());                              \
  }                                                                 \
}

#define ASSERT_EQUAL( x, y )                                        \
{                                                                   \
  s_exit_test_on_failure = true;                                    \
  if( ( x ) != ( y ) )                                              \
  {                                                                 \
    std::stringstream s;                                            \
    s << __FILE__ << ':' << __LINE__ << ':'                         \
      << std::string(__PRETTY_FUNCTION__)                           \
      << ": " << (x) << " != " << (y);                              \
    throw std::runtime_error(s.str());                              \
  }                                                                 \
}

#define CHECK_TRUE( condition )                                     \
{                                                                   \
  s_exit_test_on_failure = false;                                   \
  if( !( condition ) )                                              \
  {                                                                 \
    std::stringstream s;                                            \
    s << __FILE__ << ':' << __LINE__ << ':'                         \
      << std::string(__PRETTY_FUNCTION__)                           \
      << ": false condition";                                       \
    throw std::runtime_error(s.str());                              \
  }                                                                 \
}

#define CHECK_EQUAL( x, y )                                         \
{                                                                   \
  s_exit_test_on_failure = false;                                   \
  if( ( x ) != ( y ) )                                              \
  {                                                                 \
    std::stringstream s;                                            \
    s << __FILE__ << ':' << __LINE__ << ':'                         \
      << std::string(__PRETTY_FUNCTION__)                           \
      << ": " << (x) << " != " << (y);                              \
    throw std::runtime_error(s.str());                              \
  }                                                                 \
}
