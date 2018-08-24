#include "stdafx.h"
#include "test_msg_mqtt_client.h"
#include "im_msg_mqtt_client.h"
#include "im_msg_mqtt_client.hpp"

#pragma region namespace
namespace test {
#pragma endregion

  void test_C(){
    std::cout << "test_msg_mqtt_client_C Begin:" << std::endl;
    im::msg_mqtt_client::Test_msg_mqtt_client_C(8);
    std::cout << "test_msg_mqtt_client_C Begin:" << std::endl;
  }

  void test_CPP(){
    std::cout << "test_msg_mqtt_client_CPP Begin:" << std::endl;
    im::msg_mqtt_client::Test_msg_mqtt_client_CPP(8);
    std::cout << "test_msg_mqtt_client_CPP Begin:" << std::endl;
  }

#pragma region namespace
}
#pragma endregion

int main() {
  
  test::test_C();
  test::test_CPP();

  return 0;
}
