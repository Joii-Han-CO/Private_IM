#include "stdafx.h"
#include "test_msg_proto.h"
#include "im_msg_proto.h"
#include "im_msg_proto.hpp"
#include <iostream>

#pragma region namespace
namespace test {
#pragma endregion

void TestText() {
  auto msg_text = std::make_shared<im::msg_proto::CMsg_Text>();
  msg_text->msg_type = im::msg_proto::EIMMsg_Type::Sending;
  msg_text->text = L"Hello World";

  auto protobuf = im::msg_proto::CGenerateMsg_Text(msg_text);
  if (protobuf.empty()) {
    std::cout << "CGenerateMsg_Text Faild" << std::endl;
    return;
  }

  auto ref_msg = im::msg_proto::CParseMsg_Text(protobuf);
  if (ref_msg == nullptr) {
    std::cout << "CGenerateMsg_Text Faild" << std::endl;
    return;
  }



}

#pragma region namespace
}
#pragma endregion

int main() {

  return 0;
}
