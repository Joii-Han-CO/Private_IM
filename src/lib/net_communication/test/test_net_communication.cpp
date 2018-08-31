#include "stdafx.h"
#include "test_net_communication.h"
#include "im_net_communication.h"
#include "im_net_communication.hpp"

#pragma region namespace
namespace test {
#pragma endregion

  void test_C(){
    std::cout << "test_net_communication_C Begin:" << std::endl;
    im::net_communication::Test_net_communication_C(8);
    std::cout << "test_net_communication_C Begin:" << std::endl;
  }

  void test_CPP(){
    std::cout << "test_net_communication_CPP Begin:" << std::endl;
    im::net_communication::Test_net_communication_CPP(8);
    std::cout << "test_net_communication_CPP Begin:" << std::endl;
  }

#pragma region namespace
}
#pragma endregion

int main() {
  
  test::test_C();
  test::test_CPP();

  return 0;
}
