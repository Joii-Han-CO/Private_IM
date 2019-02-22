#include "pch.h"
#include "test_msg_proto.h"
#include "im_msg_proto.h"
#include "base/time.hpp"
#include <iostream>

#pragma region namespace
namespace test {
#pragma endregion

void Test_MsgManager() {
  auto msg_manager = std::make_shared<im::msg_proto::CProtoManager>();
  auto func_base = [](im::msg_proto::pMsg_Pub_TestChannel msg) {

  };
  im::msg_proto::Msg_Pub_TestChannel::RegCallback(msg_manager.get(),
                                                  func_base);

  im::msg_proto::Msg_Pub_TestChannel msg;
  msg.status = 82197342;
  auto buf = msg.Serializate();

  msg_manager->ParseMsg(im::msg_proto::EChannelType::pub_channel, buf);
}

void Test_GolbalChannel() {
  im::msg_proto::PP_CreatePrivateChannel p;
  p.user_name_ = L"sdkjfjseo";
  base::_uuid::Generate(&p.channel_name_);
  p.client_type_ = im::EClientType::win;
  base::_uuid::Generate(&p.client_id_);
  auto buf = p.Serializate();

  im::msg_proto::PP_CreatePrivateChannel p2;
  p2.Parse(buf);
}

#pragma region namespace
}
#pragma endregion

int main() {
  im::msg_proto::CProtoCallbackManager::Get()->Init();

  //test::Test_MsgManager();
  test::Test_GolbalChannel();

  return 0;
}
