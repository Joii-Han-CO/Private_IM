#include "stdafx.h"
#include "test_plugin_mosquitto.h"
#include "im_plugin_mosquitto.h"
#include "im_plugin_mosquitto.hpp"

#pragma region namespace
namespace test {
#pragma endregion

  void test_C(){
    std::cout << "test_plugin_mosquitto_C Begin:" << std::endl;
    im::plugin_mosquitto::Test_plugin_mosquitto_C(8);
    std::cout << "test_plugin_mosquitto_C Begin:" << std::endl;
  }

  void test_CPP(){
    std::cout << "test_plugin_mosquitto_CPP Begin:" << std::endl;
    im::plugin_mosquitto::Test_plugin_mosquitto_CPP(8);
    std::cout << "test_plugin_mosquitto_CPP Begin:" << std::endl;
  }

#pragma region namespace
}
#pragma endregion

int main() {
  
  test::test_C();
  test::test_CPP();

  return 0;
}
