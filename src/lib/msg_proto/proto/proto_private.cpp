#include "pch.h"
#include "im_msg_proto.h"

namespace im {
namespace msg_proto {

bool PR_ResLoginStatus::Parse(cMsgBuf buf) {
  int size = sizeof(status);
  if (buf.size() < (size_t)header_->Size() + size) {
    PrintLogWarn("Buf too small");
    return false;
  }
  memcpy_s(&status, sizeof(status), &buf[header_->Size()], sizeof(status));
  return true;
}

MsgBuf PR_ResLoginStatus::Serializate() {
  int size = sizeof(status);
  MP_SerializateMsg(size);

  memcpy_s(&buf[header_->Size()], sizeof(status), &status, sizeof(status));
  return buf;
}

}
}
