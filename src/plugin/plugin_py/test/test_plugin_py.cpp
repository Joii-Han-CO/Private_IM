#include "stdafx.h"
#include "test_plugin_py.h"
#include "im_plugin_py.h"
#pragma region namespace
namespace test {
#pragma endregion

  void test_C(){
    std::cout << "test_plugin_py_C Begin:" << std::endl;
    im::plugin_py::Test_plugin_py_C(8);
    std::cout << "test_plugin_py_C Begin:" << std::endl;
  }

#pragma region namespace
}
#pragma endregion

int main() {
  
  test::test_C();

  return 0;
}
