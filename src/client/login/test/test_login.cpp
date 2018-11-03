#include "stdafx.h"
#include "test_login.h"
#include "im_login.h"
#include "base/debug.hpp"
#include "im_log.h"

#pragma region namespace
namespace test {
#pragma endregion

void TestLogin() {
  im::login::Login lg;

  auto func_init_end = [](bool suc) {};

  if (lg.Init(func_init_end) == false) {
    base::debug::OutPut(L"Login Init falied, des:%s", lg.GetLastErr_Wc());
    return;
  }
}

#pragma region namespace
}
#pragma endregion

int main() {
  im::log::InitTestLog();

  test::TestLogin();
  return 0;
}
