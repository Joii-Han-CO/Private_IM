#include "stdafx.h"
#include "test_msg_mqtt_client.h"
#include "base/character_conversion.hpp"
#include "im_msg_mqtt_client.h"
#include "im_msg_mqtt_client.hpp"

#include "base/debug.hpp"

#include "mqtt_client_base.h"

#ifdef WIN32
#include <Windows.h>
#endif

#pragma region namespace
namespace test {
#pragma endregion

class TestMsg: public im::CMqttClientBase {
protected:
  void Connected(int result, int flags) override {};
  void Subscribed()override {};
  void Published() override {};
  void Messaged(const mosquitto_message *msg) override {};
  void OutLog(const base::SBaseLog &func) override {};
};

void TestMqtt_Client(int argc, char* argv[]) {
  auto p_mqt = std::make_shared<TestMsg>();

  im::SMqttConnectInfo connect_info;
  connect_info.host = "127.0.0.1";
  connect_info.port = 1883;

  if (p_mqt->Connect(connect_info) == false) {
    base::debug::OutPut(L"Connect failed, des:%s",
                        p_mqt->GetLastErr_Astd().c_str());
    return;
  }


  system("pause");
}

#pragma region namespace
}
#pragma endregion

int main(int argc, char* argv[]) {

  test::TestMqtt_Client(argc, argv);
  system("pause");
  return 0;
}
