#include "stdafx.h"
#include "test_net_communication.h"
#include "base/debug.hpp"
#include "net_communication.h"
#include <base/character_conversion.hpp>

#pragma region namespace
namespace test {
#pragma endregion

im::nc::pCNetCom g_nc1, g_nc2;

int g_port1 = 0;
int g_port2 = 0;

bool Init_1() {
  im::nc::SNetCom_InitArgs args;
  args.host = "127.0.0.1";
  args.port = -1;
  args.listener = true;

  // 同步等待
  std::condition_variable wait_init_finished;
  std::mutex wait_init_finished_sync;
  bool wait_init_finished_flag = false;
  auto cb_finished =
    [&wait_init_finished, &wait_init_finished_flag] (int port) {

    g_port1 = port;
    wait_init_finished_flag = true;
    wait_init_finished.notify_one();
  };

  g_nc1->Init(args, cb_finished);

  // 同步等待初始化完成
  std::unique_lock<std::mutex> lock(wait_init_finished_sync);
  wait_init_finished.wait(lock,
                          [wait_init_finished_flag] () {
    return wait_init_finished_flag;
  });

  return true;
}

bool Init_2() {
  im::nc::SNetCom_InitArgs args;
  args.host = "127.0.0.1";
  args.port = g_port1;
  args.listener = false;

  // 同步等待
  std::condition_variable wait_init_finished;
  std::mutex wait_init_finished_sync;
  bool wait_init_finished_flag = false;
  auto cb_finished =
    [&wait_init_finished, &wait_init_finished_flag] (int port) {

    g_port2 = port;
    wait_init_finished_flag = true;
    wait_init_finished.notify_one();
  };

  g_nc2->Init(args, cb_finished);

  // 同步等待初始化完成
  std::unique_lock<std::mutex> lock(wait_init_finished_sync);
  wait_init_finished.wait(lock,
                          [wait_init_finished_flag] () {
    return wait_init_finished_flag;
  });

  return true;
}

void TestNC(int argc, char *argv[]) {
  // 直接在同一个进程测试
  if (Init_1() == false) {
    base::debug::OutPut("Init listener failed");
    return;
  }
  if (Init_2() == false) {
    base::debug::OutPut("Init connector failed");
    return;
  }

  // 打开监听


};

#pragma region namespace
}
#pragma endregion

