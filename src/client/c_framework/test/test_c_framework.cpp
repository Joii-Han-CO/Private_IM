#include "pch.h"
#include "test_c_framework.h"
#include "im_c_framework.h"
#include "base/debug.hpp"

namespace test {

void test_C() {
  if (ClientInitGlobal == false) {
    base::debug::OutPut("Init framework failed");
    return;
  }
}

}

int main() {

  test::test_C();

  system("pause");
  return 0;
}
