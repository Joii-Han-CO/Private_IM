#include "stdafx.h"
#include "test_msg_mqtt_client.h"
#include "base/character_conversion.hpp"
#include "im_msg_mqtt_client.h"
#include "im_msg_mqtt_client.hpp"

#include "mqtt_client.h"

#ifdef WIN32
#include <Windows.h>
#endif

#pragma region namespace
namespace test {
#pragma endregion
/*
// log
void TestMqtt_Log(base::SBaseLog log) {
  std::wstring t = base::Log::FormatTypeW(log.type);
  std::wstring func;
  if (log.func)
    func = base::GB2312ToUtf16(log.func);
  std::wstring file;
  if (log.file)
    file = base::GB2312ToUtf16(log.file);
  std::wstring l;
  if (log.log)
    l = log.log;

  std::wstring info = L"-MQTT--" +
    t + L"--" + func + L":" + std::to_wstring(log.line) + L"--" + l;

#ifdef WIN32
  OutputDebugStringW(info.c_str());
#endif
}

void TestMqtt_Faild(std::wstring des) {
  std::wcout << L"MqttFaild:" << des.c_str() << std::endl;
}

void TestMqtt_Connected() {
  std::cout << "Connected" << std::endl;
}

void TestMqtt_Client(int argc, char* argv[]) {
  auto args = base::ArgsToMap(argc, argv);

  GetArgsVal(host, "host", "tcp://127.0.0.1:1883");
  GetArgsVal(topic, "t", "test_01");

  im::SMqttClientInitArgs init_args;
  init_args.host = args_host.c_str();
  init_args.topic = args_topic.c_str();

  init_args.func_log = TestMqtt_Log;
  init_args.func_faild = TestMqtt_Faild;
  init_args.func_connected = TestMqtt_Connected;

  im::IMMqttClient mqt(init_args);

  if (mqt.Connect() == false) {
    std::cout << "无法连接" << std::endl;
    return;
  }

  // 等待连接完成...
  mqt.WaitConnected();

  std::string cmd;
  while (true) {
    std::cout << "m----send message" << std::endl;
    std::cout << "q----quit" << std::endl;
    std::cout << "Wait Cmd:";
    std::cin >> cmd;

    if (cmd == "q")
      break;
    else if (cmd == "m") {
      std::string tmp_topci;
      std::string tmp_data;
      std::cin >> tmp_topci >> tmp_data;

      mqt.SendMsg(tmp_topci, tmp_data);
    }

    system("cls");
  }

  if (mqt.Disconnect() == false) {
    std::cout << "断开连接错误" << std::endl;
    return;
  }
}
*/
#pragma region namespace
}
#pragma endregion

int main(int argc, char* argv[]) {

  //test::TestMqtt_Client(argc, argv);
  system("pause");
  return 0;
  return 0;
}
