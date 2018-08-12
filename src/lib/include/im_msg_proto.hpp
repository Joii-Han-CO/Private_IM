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

  INLINE void ToCPP(IN IMMsg_Base &msg) {
    msg_type = msg.msg_type;
  }
  INLINE void ToC(OUT IMMsg_Base &msg) {
    msg.msg_type = msg_type;
  }
};

struct CMsg_Text: public CMsg_Base {
  std::wstring text;

  INLINE void ToCPP(IN IMMsg_Text &msg) {
    if (msg.text)
      text = msg.text;
    CMsg_Base::ToCPP(msg.base);
  }
  INLINE void ToC(OUT IMMsg_Text &msg) {
    if (text.empty() == false)
      msg.text = text.c_str();
    CMsg_Base::ToC(msg.base);
  }
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
  ref->ToCPP(out_msg);
  FreeMsgText(&out_msg);

  return ref;
};

INLINE std::vector<char> CGenerateMsg_Text(pCMsg_Text msg) {
  if (msg == nullptr)
    return std::vector<char>();

  IMMsg_ProtoBuf protobuf;
  IMMsg_Text tmp_msg;
  msg->ToC(tmp_msg);

  if (GenerateMsg_Text(protobuf, tmp_msg) == false)
    return std::vector<char>();
  if (protobuf.buf == nullptr || protobuf.size == 0)
    return std::vector<char>();

  std::vector<char> ref;
  ref.resize(protobuf.size);
  memcpy_s(ref.data(), ref.size(), protobuf.buf, protobuf.size);

  FreeProtoBuf(&protobuf);

  return ref;
}

#pragma region namespace
}
}
#pragma endregion
