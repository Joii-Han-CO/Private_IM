#include "pch.h"
#include "test_c_login.h"
#include "im_c_login.h"

#include "lib/im_log.h"
#include "im_c_framework.h"

#include <vector>
#include <condition_variable>

#pragma region namespace
namespace test {
#pragma endregion

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
    std::condition_variable wait_init;
    std::mutex wait_init_lock;
    bool wait_init_flag = false;
    auto func_init = [&wait_init, &wait_init_flag](bool suc) {
      wait_init_flag = true;
      wait_init.notify_all();
    };

    if (cl->Init(g_users[0].first, g_users[0].second, func_init) == false) {
      std::cout << "Init failed" << std::endl;
      return;
    }
    std::unique_lock<std::mutex> lock(wait_init_lock);
    wait_init.wait(lock, [&wait_init_flag]() {return wait_init_flag; });
  }

  system("pause");

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

#pragma region namespace
}
#pragma endregion


int main() {
  test::TestLogin();

  system("pause");
  return 0;
}
