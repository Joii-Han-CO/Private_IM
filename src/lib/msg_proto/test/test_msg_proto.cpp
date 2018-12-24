#include "pch.h"
#include "test_msg_proto.h"
#include "im_msg_proto.h"
#include "base/time.hpp"
#include <iostream>

#pragma region namespace
namespace test {
#pragma endregion

void TestTime() {
  base::time::BaseTime t;

  // 获取当前时间
  long tt_ms = 0;
  auto tt = t.GetTimeT(&tt_ms);

  base::time::BaseTime t2(tt, 0);



  //
}

#pragma region namespace
}
#pragma endregion

int main() {

  test::TestTime();

  return 0;
}
