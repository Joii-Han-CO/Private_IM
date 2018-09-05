#include "stdafx.h"
#include "test_msg_mqtt_client.h"
#include "base/character_conversion.hpp"

#include "base/debug.hpp"

#include "mqtt_client_base.h"

#ifdef WIN32
#include <Windows.h>
#endif

#pragma region namespace
namespace test {
#pragma endregion

std::string g_topic_ = "test_1";

im::pCMqttClientBase gmsg;

#pragma region Callback

void MqttLog(const base::log::SBaseLog &l) {
  base::debug::OutputLogInfo(l);
}

#pragma endregion

#pragma region I/R

// 连接...
bool Test_Init() {
  base::debug::OutPut("Begin...");
  gmsg = std::make_shared<im::CMqttClientBase>(MqttLog);

  // 同步连接
  std::condition_variable wait_conncet;
  std::mutex wait_conncet_mutex;
  bool wait_conncet_flag = false;

  // 初始化连接参数
  im::SMqttConnectInfo connect_info;
  connect_info.host = "127.0.0.1";
  connect_info.port = 1883;

  // 回调
  connect_info.cb_status_change =
    [&wait_conncet, &wait_conncet_flag] (im::EMqttOnlineStatus status) {
    base::debug::OutPut(
      "[StatusChange]--%s",
      im::CMqttClientBase::FormatOnlineStatusA(status).c_str());
    if (status == im::EMqttOnlineStatus::connected) {
      wait_conncet_flag = true;
      wait_conncet.notify_all();
    }
  };

  if (gmsg->Connect(connect_info) == false) {
    base::debug::OutPut(L"Connect failed, des:%S",
                        gmsg->GetLastErr_Astd().c_str());
    return false;
  }

  std::unique_lock<std::mutex> lock(wait_conncet_mutex);
  wait_conncet.wait(lock,
                    [&wait_conncet_flag] () {return wait_conncet_flag; });

  base::debug::OutPut("Connect finished");
  return true;
}

void Test_Release() {
  gmsg->Disconnect();
  base::debug::OutPut("Disconnect finished");
}

bool Test_Sub(int argc, char* argv[]) {
  auto args = base::ArgsToMap(argc, argv);
  auto arg_topic = args["t"];
  if (!arg_topic.empty())
    g_topic_ = arg_topic;

  gmsg->Subscribe(g_topic_, [] (std::vector<char>) {});

}

bool Test_UnSub() {

}

#pragma endregion

void Test_SendMsg() {
  base::debug::OutPut("Loop");
  while (true) {
    std::string cmd, data;
    std::cin >> cmd >> data;
    if (cmd == "q")
      break;
    if (gmsg->Publish(cmd, std::vector<char>(data.begin(), data.end()),
                      [] () {
      base::debug::OutPut(L"Publish success");
    })
        == false) {
      base::debug::OutPut(L"Publish failed, des:%S",
                          gmsg->GetLastErr_Astd().c_str());
    }
  }
}

void TestMqtt_Client(int argc, char* argv[]) {
  if (Test_Init(argc, argv) == false)
    return;

  // 测试消息
  Test_SendMsg();

  // 断开连接
  Test_Release();

  base::debug::OutPut("End press enter exit...");
  getchar();
}

#pragma region namespace
}
#pragma endregion

int main(int argc, char* argv[]) {

  test::TestMqtt_Client(argc, argv);
  system("pause");
  return 0;
}
