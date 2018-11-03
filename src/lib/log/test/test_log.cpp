#include "stdafx.h"
#include "test_log.h"
#include "im_log.h"
#include "base/time.hpp"
#include "base/debug.hpp"


#pragma region namespace
namespace test {
#pragma endregion

void TestPerformance() {
  base::time::Timer_ms ms;

  const int count = 10000;

  for (int i = 0; i < count; i++) {
    PrintLogDbg("Test Log %s - %d", "1", 1);
    PrintLogDbg(L"Test Log %s - %d", "1", 2);

    PrintLogInfo("Test Log %s - %d", "1", 3);
    PrintLogInfo(L"Test Log %s - %d", "1", 4);

    PrintLogWarn("Test Log %s - %d", "1", 5);
    PrintLogWarn(L"Test Log %s - %d", "1", 6);

    PrintLogErro("Test Log %s - %d", "1", 7);
    PrintLogErro(L"Test Log %s - %d", "1", 8);
  }

  auto x = ms.End();
  base::debug::OutPut(L"%d", x);
  base::debug::OutPut(L"Time spent per log :%d microsecond", x / count / 8);
}

#pragma region namespace
}
#pragma endregion


int main() {
  im::log::SLog_InitArgs log_args;
  log_args.print_dbg = true;
  log_args.print_info = true;
  log_args.print_warn = true;
  log_args.print_erro = true;

  im::log::CLog::Get()->Init(&log_args);

  test::TestPerformance();

  system("pause");
  return 0;
}
