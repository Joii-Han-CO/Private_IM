#include "pch.h"
#include "server_main.h"
#include "frame/server_frame_work.h"
#include "frame/publish_channel.h"

// TODO 第一个版本全部的模块都在同一个进程中执行，以后考虑做成多进程的

// 初始化，等待MQTT连接成功之后才可以开始业务
int Init() {
  if (server::CSFarmeWork::Get()->Init() == false) {
    PrintLogErro("init failed");
    return -1;
  }

  base::async::Event wait_pubch;
  bool pubch_init_suc = false;
  auto func = [&wait_pubch, &pubch_init_suc](bool suc) {
    pubch_init_suc = suc;
    wait_pubch.Notify();
  };
  if (server::CSPubChannel::Get()->Init(func) == false) {
    PrintLogErro("Init pubchannel failed");
    return -1;
  }
  wait_pubch.Wait();
  if (pubch_init_suc == false) {
    PrintLogErro("Init pubchannel failed 2");
    return -1;
  }
  PrintLogInfo("Init success");
  return 0;
}

void Uninit() {
  server::CSPubChannel::Get()->Uninit();
  server::CSFarmeWork::Get()->Uninit();
}

int Run() {
  int exit_code = Init();
  if (exit_code != 0) {
    return exit_code;
  }

  //_getch();

  Uninit();
  return 0;
}

int main() {
  base::log::PrintModleHeader();

  auto exit_code = Run();

  //_getch();
  return exit_code;
}
