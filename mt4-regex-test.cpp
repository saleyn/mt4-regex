#include "test.hpp"
#include "mt4-regex.hpp"

using namespace std;

void UnhandledException() {
  auto p = current_exception();
  try { rethrow_exception(p); }
  catch (exception & e) { cerr << e.what() << endl; }
  catch (...) { cerr << "Unknown exception" << endl; }
  if (s_exit_test_on_failure)
    exit(1);
}

void TestBasic()
{
  auto p = ReInit(L"<!--([^-]+-->");
  ASSERT_TRUE(p == nullptr);

  p = ReInit(L"<!--([^-]+)-->");
  ASSERT_TRUE(p != nullptr);

  int count=0;
  
  const auto rex = L"abc<!--ABC-->efg<!--EFG-->xxxx";

  for (int res = ReMatch(p, rex); res; res = ReMatchNext(p), count++) {
    CHECK_EQUAL(2, res);
    wchar_t match[80];
    int     pos;
    auto rc = ReGetMatch(p, 1, pos, match, 80);
    CHECK_EQUAL(3, rc);
    auto exppos = count == 0 ? 7 : 20;
    CHECK_EQUAL(exppos, pos);
    auto expect = count == 0 ? L"ABC" : L"EFG";
    CHECK_TRUE(expect == std::wstring(match, rc));
  }

  CHECK_EQUAL(2, count);

  ReFinalize(p);
}

void TestGetTimeOfDay()
{
  auto now1 = GetTimeOfDay() / 1000000;
  auto now2 = time(nullptr);
  CHECK_EQUAL(now1, now2);
}

int main(int, char**)
{
  set_terminate(&UnhandledException);

  TestBasic();
  TestGetTimeOfDay();
}