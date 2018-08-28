﻿#include "stdafx.h"
#include "test_msg_mqtt_client.h"
#include "base/character_conversion.hpp"
#include "im_msg_mqtt_client.h"
#include "im_msg_mqtt_client.hpp"

#include "mqtt_client_base.h"

#ifdef WIN32
#include <Windows.h>
#endif

#pragma region namespace
namespace test {
#pragma endregion

void TestMqtt_CB_Mqtt(const base::SBaseLog &func) {}

void TestMqtt_Client(int argc, char* argv[]) {
  auto p_mqt = std::make_shared<im::CMqttClientBase>(TestMqtt_CB_Mqtt);
}

#pragma region namespace
}
#pragma endregion

int main(int argc, char* argv[]) {

  test::TestMqtt_Client(argc, argv);
  system("pause");
  return 0;
  return 0;
}
