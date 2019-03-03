#include "pch.h"
#include "test_c_login.h"
#include "im_c_login.h"

#include "lib/im_log.h"
#include "lib/base/debug.hpp"
#include "im_c_framework.h"

#include <vector>
#include <condition_variable>
//#include <conio.h>

namespace test {

const std::vector<std::pair<std::wstring, std::wstring>> g_users =
{{L"user1", L"11111111"},
{L"user2", L"22222222"},
};

void TestLogin() {
  if (ClientInitGlobal == false) {
    std::cout << "Init framework failed" << std::endl;
    return;
  }

  auto cl = ClientFW->GetLoginPtr();

  // 初始化登陆
  {
    base::async::Event wait_init;
    auto func_init = [&wait_init](bool suc) {
      wait_init.Notify();
    };

    if (cl->Init(g_users[0].first, g_users[0].second, func_init) == false) {
      base::debug::OutPut(L"Init failed");
      return;
    }
    wait_init.Wait();
  }

  //_getch();

  // 登出
  {
    auto func_uninit = [](bool suc) {

    };
    if (cl->Uninit(func_uninit) == false) {
      std::cout << "Uninit failed" << std::endl;
      return;
    }
  }
}

}

int main() {
  base::log::PrintModleHeader();

  // 初始化日志
  im::log::SLog_InitArgs log_init;
  log_init.print_dbg = true;
  log_init.print_info = true;
  log_init.print_warn = true;
  log_init.print_erro = true;
  log_init.save_file = false;
  log_init.output_ctrl = true;
  im::log::CLog::Get()->Init(&log_init);

  test::TestLogin();

  system("pause");
  return 0;
}
