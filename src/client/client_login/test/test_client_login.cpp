﻿#include "pch.h"
#include "test_client_login.h"
#include "im_client_login.h"
#include "base/debug.hpp"
#include "im_log.h"

#pragma region namespace
namespace test {
#pragma endregion

const std::wstring gc_user_name = L"test_1";
const std::wstring gc_user_pwd = L"123456";

void Test_MqttCallback(im::EMqttOnlineStatus status) {}

void TestLogin() {
  im::login::Login lg;

  auto func_init_end = [](bool suc) {};

  /*if (lg.Init(gc_user_name,
              im::ssl_base::GetStrSHA256(gc_user_pwd.c_str()),
              func_init_end) == false) {
    base::debug::OutPut(L"Login Init falied, des:%s", lg.GetLastErr_Wc());
    return;
  }*/

}

#pragma region namespace
}
#pragma endregion

int main() {
  im::log::InitTestLog();

  test::TestLogin();

  system("pause");
  return 0;
}
