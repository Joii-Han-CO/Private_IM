#include "pch.h"
#include "test_c_framework.h"
#include "im_c_framework.h"
#include "base/debug.hpp"

#pragma region namespace
namespace test {
#pragma endregion

void test_C() {
  if (ClientInitGlobal == false) {
    base::debug::OutPut("Init framework failed");
    return;
  }
}

#pragma region namespace
}
#pragma endregion

int main() {

  test::test_C();

  system("pause");
  return 0;
}
