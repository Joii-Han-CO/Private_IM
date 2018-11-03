#include "stdafx.h"
#include "test_$(--prj_name--).h"
#include "im_$(--prj_name--).h"

#pragma region namespace
namespace test {
#pragma endregion

  void test_C(){
    std::cout << "test_$(--prj_name--)_C Begin:" << std::endl;
    im::$(--prj_name--)::Test_$(--prj_name--)_C(8);
    std::cout << "test_$(--prj_name--)_C Begin:" << std::endl;
  }

#pragma region namespace
}
#pragma endregion

int main() {
  
  test::test_C();

  return 0;
}
