#include "pch.h"
#include "test_msg_proto.h"
#include "im_msg_proto.h"
#include "base/time.hpp"
#include <iostream>

#pragma region namespace
namespace test {
#pragma endregion

void Test_Login() {
  im::msg_proto::Msg_UserLogin msg;
  msg.user_name = L"sod8uf90w8s34f";
  msg.login_channel = L"osjsidjflsjefl";
  msg.client_type = im::EClientType::android;
  msg.client_id = 89123;

  auto buf = msg.Serializate();

  auto base_msg = (im::msg_proto::Parse_LoginChannel(buf));
  if (base_msg == nullptr) {
    return;
  }

  if (base_msg->type != im::msg_proto::ELoginMsgType::UserLogin) {
    return;
  }

  auto msg2 = std::dynamic_pointer_cast<im::msg_proto::Msg_UserLogin>(base_msg);
}

#pragma region namespace
}
#pragma endregion

int main() {

  test::Test_Login();

  return 0;
}
