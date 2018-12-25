#include "pch.h"
#include "server_main.h"
#include "management.h"
#include "login/server_login.h"

// TODO 第一个版本全部的模块都在同一个进程中执行，以后考虑做成多进程的
 
int Init() {
  if (server::CManagement::Get()->Init() == false) {
    std::cout << "init failed" << std::endl;
    return -1;
  }

  std::condition_variable wait_init_finished;
  std::mutex wait_init_finished_async;
  bool is_init = false;
  auto func_init_res = [&wait_init_finished, &is_init](bool suc) {
    is_init = true;
    wait_init_finished.notify_all();
  };

  server::ServerLogin li;
  if (li.Init(func_init_res) == false) {
    std::cout << "init login module failed, will exit" << std::endl;
    return -1;
  }
  std::unique_lock<std::mutex> lock(wait_init_finished_async);
  wait_init_finished.wait(lock, [is_init]() {return is_init; });

  return 0;
}

int Run() {
  int exit_code = Init();
  if (exit_code != 0) {
    return exit_code;
  }

  return 0;
}

int main() {
  return Run();
}
