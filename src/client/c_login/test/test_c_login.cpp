#include "pch.h"
#include "test_c_login.h"
#include "im_c_login.h"

#pragma region namespace
namespace test {
#pragma endregion

  void test_C(){
    std::cout << "test_c_login_C Begin:" << std::endl;
    im::c_login::Test_c_login_C(8);
    std::cout << "test_c_login_C Begin:" << std::endl;
  }

#pragma region namespace
}
#pragma endregion

int main() {
  
  test::test_C();

  return 0;
}
