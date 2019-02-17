#include "pch.h"
#include "server_main.h"
#include "frame/server_frame_work.h"

// TODO 第一个版本全部的模块都在同一个进程中执行，以后考虑做成多进程的

// 初始化，等待MQTT连接成功之后才可以开始业务
int Init() {
  if (server::CSFarmeWork::Get()->Init() == false) {
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
  base::log::PrintModleHeader();

  auto exit_code = Run();

  _getch();
  return exit_code;
}
