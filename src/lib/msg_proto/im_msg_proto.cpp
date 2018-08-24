#include "stdafx.h"
#include "im_msg_proto.h"
#include "proto_class/msg.pb.h"
#include "base/character_conversion.hpp"

#pragma region namespace
namespace im {
namespace msg_proto {
#pragma endregion

#pragma region Free

void FreeProtoBuf(IN IMMsg_ProtoBuf *buf) {
  if (buf != nullptr &&
      buf->buf != nullptr && buf->size != 0)
    delete[] buf->buf;
  buf->buf = nullptr;
  buf->size = 0;
}

void FreeMsgText(IN IMMsg_Text * buf) {
  if (buf != nullptr && buf->text != nullptr)
    delete[] buf->text;
  buf->text = nullptr;
}

#pragma endregion

bool ParseMsg_Text(IN IMMsg_ProtoBuf &buf, OUT IMMsg_Text &msg) {
  Proto_Text tmp_msg;
  tmp_msg.ParseFromArray(buf.buf, buf.size);


  return true;
}

bool GenerateMsg_Text(OUT IMMsg_ProtoBuf &buf, IN IMMsg_Text &msg) {
  Proto_Text tmp_msg;
  tmp_msg.set_text(base::Utf16ToUtf8(msg.text));
  //tmp_msg.base_->set_msg_type();

  return true;
}

#pragma region namespace
}
}
#pragma endregion
