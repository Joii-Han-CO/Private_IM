#include "pch.h"
#include "im_msg_proto.h"
#include "proto_class/msg.pb.h"
#include "base/character_conversion.hpp"

#pragma region namespace
namespace im {
namespace msg_proto {
#pragma endregion

#pragma region Header

int MsgBase_Header::GetSize() {
  return sizeof(type) + sizeof(int64_t);
}

bool MsgBase_Header::Parse(const MsgBuf &buf) {
  if (buf.size() < (size_t)GetSize())
    return false;

  type = (uint8_t)buf[0];

  int64_t time_val = 0;
  memcpy_s(&time_val, sizeof(time_val), &buf[1], sizeof(time_val));
  time = base::time::BaseTime(time_val);

  return true;
}

bool MsgBase_Header::Serialization(MsgBuf &buf) {
  if (buf.size() < (size_t)GetSize()) {
    return false;
  }

  buf[0] = (char)type;

  auto time_val = time.GetVal();
  memcpy_s(&buf[1], sizeof(time_val), &time_val, sizeof(time_val));

  return true;
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
