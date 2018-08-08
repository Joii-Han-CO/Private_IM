#include "stdafx.h"
#include "test_msg_proto.h"
#include "im_msg_proto.h"
#include "im_msg_proto.hpp"

#pragma region namespace
namespace test {
#pragma endregion

  void test_C(){
    std::cout << "test_msg_proto_C Begin:" << std::endl;
    im::msg_proto::Test_msg_proto_C(8);
    std::cout << "test_msg_proto_C Begin:" << std::endl;
  }

  void test_CPP(){
    std::cout << "test_msg_proto_CPP Begin:" << std::endl;
    im::msg_proto::Test_msg_proto_CPP(8);
    std::cout << "test_msg_proto_CPP Begin:" << std::endl;
  }

#pragma region namespace
}
#pragma endregion

int main() {
  
  test::test_C();
  test::test_CPP();

  return 0;
}
