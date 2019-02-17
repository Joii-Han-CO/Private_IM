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
