#pragma once
#include "im_type_def.h"
#include "im_msg_proto.h"
#include <memory>
#include <vector>


#pragma region namespace
namespace im {
namespace msg_proto {
#pragma endregion

struct CMsg_Base {
  EIMMsg_Type msg_type;   //消息类型

  INLINE void CCopy(IMMsg_Base &base_msg) {
    msg_type = base_msg.msg_type;
  }
  INLINE void CCopyOut(IMMsg_Base &base_msg) {
    base_msg.msg_type = msg_type;
  }
};

struct CMsg_Text: public CMsg_Base {
  std::wstring text;
};
typedef std::shared_ptr<CMsg_Text> pCMsg_Text;

INLINE pCMsg_Text CParseMsg_Text(std::vector<char> protobuf) {
  IMMsg_ProtoBuf tmp_buf;
  tmp_buf.buf = protobuf.data();
  tmp_buf.size = protobuf.size();

  IMMsg_Text out_msg;

  if (ParseMsg_Text(tmp_buf, out_msg) == false) {
    return nullptr;
  }

  auto ref = std::make_shared<CMsg_Text>();
  ref->CCopy(out_msg.base);
  if (out_msg.text)
    ref->text = out_msg.text;
  return ref;
};

INLINE std::vector<char> CGenerateMsg_Text(pCMsg_Text msg) {
  if (msg == nullptr)
    return std::vector<char>();

  IMMsg_ProtoBuf protobuf;
  IMMsg_Text tmp_msg;
  msg->CCopyOut(tmp_msg.base);

  tmp_msg.text = msg->text.c_str();

  if (GenerateMsg_Text(protobuf, tmp_msg) == false)
    return std::vector<char>();
  if (protobuf.buf == nullptr || protobuf.size == 0)
    return std::vector<char>();

  std::vector<char> ref;
  ref.resize(protobuf.size);
  memcpy_s(ref.data(), ref.size(), protobuf.buf, protobuf.size);
  return ref;
}

#pragma region namespace
}
}
#pragma endregion
