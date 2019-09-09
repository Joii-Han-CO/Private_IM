#include "stdafx.h"
#include "test_ssl_base.h"
#include "im_ssl_base.h"
#include "im_ssl_base.hpp"

#pragma region namespace
namespace test {
#pragma endregion

  void test_C(){
    std::cout << "test_ssl_base_C Begin:" << std::endl;
    im::ssl_base::Test_ssl_base_C(8);
    std::cout << "test_ssl_base_C Begin:" << std::endl;
  }

  void test_CPP(){
    std::cout << "test_ssl_base_CPP Begin:" << std::endl;
    im::ssl_base::Test_ssl_base_CPP(8);
    std::cout << "test_ssl_base_CPP Begin:" << std::endl;
  }

#pragma region namespace
}
#pragma endregion

int main() {
  
  test::test_C();
  test::test_CPP();

  return 0;
}
