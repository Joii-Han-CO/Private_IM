#include "pch.h"
#include "test_msg.h"
#include "im_msg.h"
#include "im_log.h"
#include "im_global_val.h"

#include "base/debug.hpp"

#include <condition_variable>

#ifdef _WIN32
#include <conio.h>
#else
#endif

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

std::wstring g_channel_name = L"EDB8EE02-6683-436E-8673-700107707AFD";
std::wstring g_user1 = L"u1";
std::wstring g_user2 = L"u2";

im::pCMqttClient g_mqt_client, g_mqt_server;

void TestPubMsg() {

}

void TestMsg() {
  auto mq_client = InitMqtt();
  auto mq_server = InitMqtt();


  //msg_client.Init();

  mq_client->Disconnect();
  mq_server->Disconnect();
  return;
}

#pragma region namespace
}
#pragma endregion

int main() {
  im::log::InitTestLog();

  test::g_mqt_client = test::InitMqtt();
  test::g_mqt_server = test::InitMqtt();

  test::TestPubMsg();
  //test::TestMsg();

  base::debug::OutPut("Will Exit");
  //auto ss = _getch();
  return 0;
}
