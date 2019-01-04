#include "pch.h"
#include "test_c_login.h"
#include "im_c_login.h"

#include "lib/im_log.h"

#pragma region namespace
namespace test {
#pragma endregion

void TestLogin() {
  im::c_login::ClientLogin cl;
  if (cl.Init() == false) {
    std::cout << "Init failed" << std::endl;
    return;
  }

}

#pragma region namespace
}
#pragma endregion


int main() {
  im::log::SLog_InitArgs log_args;
  log_args.log_path = L"test_client_login.log";
  log_args.print_dbg = true;
  log_args.print_info = true;
  log_args.print_warn = true;
  log_args.print_erro = true;
  log_args.print_ctrl = true;
  im::log::CLog::Get()->Init(&log_args);

  test::TestLogin();

  return 0;
}
