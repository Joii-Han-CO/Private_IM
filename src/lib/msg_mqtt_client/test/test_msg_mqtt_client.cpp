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

std::string g_topic_ = "test_1";

class TestMsg: public im::CMqttClientBase {
protected:
  void OnlineStatusChange(im::EMqttOnlineStatus status) override {
    base::debug::OutPut("[StatusChange]--%s",
                        FormatOnlineStatusA(status).c_str());

    // 连接成功后，添加一个订阅
    if (status == im::EMqttOnlineStatus::connected) {
      if (Subscribe(g_topic_,
                    std::bind(&TestMsg::Msg_1, this,
                              g_topic_, std::placeholders::_1))
          == false) {
        base::debug::OutPut(L"Subscribe failed, des:%S",
                            GetLastErr_Astd().c_str());
        return;
      }
      base::debug::OutPut("Subscribe finished");
    }
  };

  void Subscribed(const std::string &topic) override {
    base::debug::OutPut("[Subscribed]--%s", topic.c_str());
  };

  void OutLog(const base::log::SBaseLog &l) override {
    base::debug::OutputLogInfo(l);
  };

  void Msg_1(const std::string &topic, const std::vector<char> &data) {
    base::debug::OutPut("%s message", topic.c_str());
  }
};

void Loop(std::shared_ptr<TestMsg> p_mqt) {
  base::debug::OutPut("Loop");
  while (true) {
    std::string cmd, data;
    std::cin >> cmd >> data;
    if (cmd == "q")
      break;
    if (p_mqt->Publish(cmd, std::vector<char>(data.begin(), data.end()),
                       [] () {
      base::debug::OutPut(L"Publish success");
    })
        == false) {
      base::debug::OutPut(L"Publish failed, des:%S",
                          p_mqt->GetLastErr_Astd().c_str());
    }
  }
}

void TestMqtt_Client(int argc, char* argv[]) {
  auto args = base::ArgsToMap(argc, argv);
  auto arg_topic = args["t"];
  if (!arg_topic.empty())
    g_topic_ = arg_topic;

  base::debug::OutPut("Begin...");
  auto p_mqt = std::make_shared<TestMsg>();

  im::SMqttConnectInfo connect_info;
  connect_info.host = "127.0.0.1";
  connect_info.port = 1883;

  if (p_mqt->Connect(connect_info) == false) {
    base::debug::OutPut(L"Connect failed, des:%S",
                        p_mqt->GetLastErr_Astd().c_str());
    return;
  }
  base::debug::OutPut("Connect finished");

  // loop
  Loop(p_mqt);

  p_mqt->Disconnect();
  base::debug::OutPut("Disconnect finished");

  base::debug::OutPut("End...");
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
