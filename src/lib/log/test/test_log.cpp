#include "stdafx.h"
#include "test_log.h"
#include "im_log.h"


#pragma region namespace
namespace test {
#pragma endregion

#pragma region namespace
}
#pragma endregion

int main() {
  im::log::CLog::Get()->Init();

  PrintBaseDbg("Test Log %s - %d", "1", 1);
  PrintBaseDbg("Test Log %s - %d", "1", 1);

  return 0;
}
