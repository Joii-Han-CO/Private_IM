#include "pch.h"
#include "test_msg.h"
#include "im_msg.h"
#include "im_log.h"

#include "base/debug.hpp"

#include <condition_variable>
#include <conio.h>

#pragma region namespace
namespace test {
#pragma endregion

im::pCMqttClient InitMqtt() {
  auto mt = std::make_shared<im::CMqttClient>();

  im::SMqttConnectInfo info;
  info.host = "127.0.0.1";

  std::condition_variable con;
  std::mutex con_lock;
  bool con_flag = false;
  bool suc = true;

  auto func_status = [&con, &con_flag, &suc]
  (im::EMqttOnlineStatus status) {
    if (status == im::EMqttOnlineStatus::connected) {
      con_flag = true;
      con.notify_all();
    }
    else if (status == im::EMqttOnlineStatus::error) {
      suc = false;
      con_flag = true;
      con.notify_all();
    }
  };
  info.cb_status_change = func_status;

  if (mt->Connect(info) == false) {
    con.notify_all();
    return nullptr;
  }

  std::unique_lock<std::mutex> lock(con_lock);
  con.wait(lock, [&con_flag]() {return con_flag; });

  if (suc == false) {
    //mt->Disconnect();
    return nullptr;
  }

  return mt;
}

void TestMsg() {
  auto mq1 = InitMqtt();
  auto mq2 = InitMqtt();

  im::msg::CMsg m1, m2;
  im::msg::SMsg_InitArgs init1, init2;

  base::async::Event if1, if2;

  init1.caller = true;
  init1.user_name = L"u1";
  init1.channel_name = L"EDB8EE02-6683-436E-8673-700107707AFD";
  init1.mqtt = mq1;

  // 1 初始化完成
  init1.init_finished = [&if1](bool suc) {
    if1.Notify();

    if (suc == false) {
      base::debug::OutPut("init 1 callback failed");
      return;
    }
  };

  // 1 连接完成
  init1.connect_finished = [](bool suc) {
    if (suc) {
      base::debug::OutPut("Init 1 success!");
    }
    else {
      base::debug::OutPut("connect 1 false");
    }
  };

  init2.caller = false;
  init2.user_name = L"u2";
  init2.channel_name = L"EDB8EE02-6683-436E-8673-700107707AFD";
  init2.mqtt = mq2;

  // 2 初始化完成
  init2.init_finished = [&if2](bool suc) {
    if2.Notify();

    if (suc == false) {
      base::debug::OutPut("init 2 callback failed");
      return;
    }
  };

  // 2 连接完成
  init2.connect_finished = [](bool suc) {
    if (suc) {
      base::debug::OutPut("Init 2 success!");
    }
    else {
      base::debug::OutPut("connect 2 false");
    }
  };

  if (m1.Init(&init1) == false) {
    base::debug::OutPut("msg 1 init failed");
    return;
  }
  base::debug::OutPut("msg 1 init ok");

  if (m2.Init(&init2) == false) {
    base::debug::OutPut("msg 2 init failed");
    return;
  }
  base::debug::OutPut("msg 2 init ok");

  if1.Wait();
  if2.Wait();

  if (m1.SendTestMsg() == false) {
    base::debug::OutPut("1 send test msg failed");
  }

  auto ss = _getch();
  m1.Uninit();
  m2.Uninit();
  mq1->Disconnect();
  mq2->Disconnect();
  return;
}

#pragma region namespace
}
#pragma endregion

int main() {
  im::log::InitTestLog();

  test::TestMsg();

  base::debug::OutPut("Will Exit");
  auto ss = _getch();
  return 0;
}
