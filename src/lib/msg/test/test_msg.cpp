#include "pch.h"
#include "test_msg.h"
#include "im_msg.h"
#include "im_log.h"
#include "im_global_val.h"

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

std::wstring g_channel_name = L"EDB8EE02-6683-436E-8673-700107707AFD";
std::wstring g_user1 = L"u1";
std::wstring g_user2 = L"u2";

im::pCMqttClient g_mqt_client, g_mqt_server;

void TestPubMsg() {
  im::msg::CPriChannel server_pc, client_pc;

  auto func_client_recv = [](cMsgBuf buf) {};
  auto func_server_recv = [](cMsgBuf buf) {};

  im::msg::CPriChannel::SInitArgs client_args;
  client_args.client = true;
  client_args.mqtt = g_mqt_client;
  client_args.pub_name = L"abcdefgh";
  client_args.func_recv = func_client_recv;

  im::msg::CPriChannel::SInitArgs server_args;
  server_args.client = false;
  server_args.mqtt = g_mqt_client;
  server_args.pub_name = L"abcdefgh";
  server_args.func_recv = func_server_recv;

#pragma region Init server pub

  base::async::Event sync_server_init;
  bool suc_server_init = false;
  auto func_server_pub_init =
    [&sync_server_init, &suc_server_init]
  (bool suc) {
    suc_server_init = suc;
    sync_server_init.Notify();
  };
  auto func_pub_recv = [](cMsgBuf) {};

  im::msg::CPubChannel_Server server_pub(
    g_mqt_server, im::gv::g_mqtt_pub_sub_);
  if (server_pub.Init(func_server_pub_init, func_pub_recv) == false) {
    base::debug::OutPut("Init global public failed");
    return;
  }
  sync_server_init.Wait();
  if (suc_server_init == false) {
    base::debug::OutPut("Init global public failed 2");
    return;
  }

#pragma endregion

#pragma region Init client pub

  im::msg::CPubChannel_Client client_pub(
    g_mqt_client, im::gv::g_mqtt_pub_sub_);



#pragma endregion


  client_pc.Init(&client_args, [](bool suc) {});

}

void TestMsg() {
  auto mq_client = InitMqtt();
  auto mq_server = InitMqtt();

  auto server_manager = im::msg::CServerMsgManager::Get();

  im::msg::CClientMsg msg_client;

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
  auto ss = _getch();
  return 0;
}
