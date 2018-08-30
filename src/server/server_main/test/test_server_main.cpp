#include "stdafx.h"
#include "test_server_main.h"
#include "im_server_main.h"
#include "im_server_main.hpp"

#pragma region namespace
namespace test {
#pragma endregion

  void test_C(){
    std::cout << "test_server_main_C Begin:" << std::endl;
    im::server_main::Test_server_main_C(8);
    std::cout << "test_server_main_C Begin:" << std::endl;
  }

  void test_CPP(){
    std::cout << "test_server_main_CPP Begin:" << std::endl;
    im::server_main::Test_server_main_CPP(8);
    std::cout << "test_server_main_CPP Begin:" << std::endl;
  }

#pragma region namespace
}
#pragma endregion

int main() {
  
  test::test_C();
  test::test_CPP();

  return 0;
}
