#include "stdafx.h"
#include "im_msg_proto.h"

#pragma region namespace
namespace im {
namespace msg_proto {
#pragma endregion

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

bool ParseMsg_Text(IN IMMsg_ProtoBuf &buf, OUT IMMsg_Text &msg) {
  return true;
}

bool GenerateMsg_Text(OUT IMMsg_ProtoBuf &buf, IN IMMsg_Text &msg) {
  return true;
}

#pragma region namespace
}
}
#pragma endregion
