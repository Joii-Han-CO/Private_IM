#include "pch.h"
#include "server_main.h"
#include "management.h"
#include "login/server_login.h"

// TODO 第一个版本全部的模块都在同一个进程中执行，以后考虑做成多进程的

server::pServerLogin g_li;

// 初始化，等待MQTT连接成功之后才可以开始业务
int Init() {
  if (server::CManagement::Get()->Init() == false) {
    std::cout << "init failed" << std::endl;
    return -1;
  }

  std::condition_variable wait_init_finished;
  std::mutex wait_init_finished_async;
  bool is_init = false;
  bool is_success = false;
  auto func_init_res = [&wait_init_finished, &is_init, &is_success]
  (bool suc) {
    is_success = suc;
    is_init = true;
    wait_init_finished.notify_all();
  };

  g_li = std::make_shared<server::ServerLogin>();
  if (g_li->Init(func_init_res) == false) {
    std::cout << "Init login module failed, will exit" << std::endl;
    return -1;
  }
  std::unique_lock<std::mutex> lock(wait_init_finished_async);
  wait_init_finished.wait(lock, [&is_init]() { return is_init; });

  if (is_success == false) {
    PrintLogErro("Server init failed and exit.");
    return 1;
  }

  return 0;
}

int Run() {
  int exit_code = Init();
  if (exit_code != 0) {
    return exit_code;
  }

  g_li->Run();
  return 0;
}

int main() {
  base::log::PrintModleHeader();

  auto exit_code = Run();
  if (g_li)
    g_li->Uninit();

  _getch();
  return exit_code;
}
