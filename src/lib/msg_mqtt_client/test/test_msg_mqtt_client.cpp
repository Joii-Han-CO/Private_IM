#include "stdafx.h"
#include "test_msg_mqtt_client.h"
#include "im_msg_mqtt_client.h"
#include "im_msg_mqtt_client.hpp"

#include "mqtt_client.h"

#ifdef WIN32
#include <Windows.h>
#endif

#pragma region namespace
namespace test {
#pragma endregion

// log
void test_mqtt_log(base::SBaseLog log) {
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

void test_mqtt_faild(std::wstring des) {
  std::wcout << L"MqttFaild:" << des.c_str() << std::endl;
}

void TestMQTT_Connected() {

}

void test_mqtt_client(int argc, char* argv[]) {
  auto a = base::ArgsToMap(argc, argv);

  im::SMqttClientInitArgs init_args;
  init_args.host = "tcp://127.0.0.1:1883";
  init_args.topic = "test_01";

  init_args.func_log = test_mqtt_log;
  init_args.func_faild = test_mqtt_faild;
  init_args.func_connected = TestMQTT_Connected;

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
      std::string d;
      std::cin >> d;
      std::string ss = "123";
      mqt.SendMsg(ss, d);
    }

    system("cls");
  }

  if (mqt.Disconnect() == false) {
    std::cout << "断开连接错误" << std::endl;
    return;
  }
}

#pragma region namespace
}
#pragma endregion

int main(int argc, char* argv[]) {

  test::test_mqtt_client(argc, argv);
  system("pause");
  return 0;
  return 0;
}
