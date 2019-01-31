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
  time = std::make_shared<base::time::BaseTime>(time_val);

  return true;
}

bool MsgBase_Header::Serializate(MsgBuf &buf) {
  if (buf.size() < (size_t)GetSize()) {
    return false;
  }

  buf[0] = (char)type;

  time = std::make_shared<base::time::BaseTime>();
  auto time_val = time->GetVal();
  memcpy_s(&buf[1], sizeof(time_val), &time_val, sizeof(time_val));

  return true;
}

#pragma endregion

#pragma region MsgBase

MsgBase::MsgBase() {
  // header = std::make_shared<MsgBase_Header>();
}

MsgBase::~MsgBase() {}

pMsgBase MsgBase::Parse(const MsgBuf &buf, EChannelType t) {
  auto h = std::make_shared<MsgBase_Header>();
  if (h->Parse(buf) == false) {
    return nullptr;
  }

  pMsgBase msg;
  if (t == EChannelType::login)
    msg = MsgBase_Login::Factory(h->type);

  if (msg == nullptr) {
    return nullptr;
  }

  msg->header.type = h->type;
  msg->header.time = h->time;

  if (msg->_Parse(buf) == false) {
    return nullptr;
  }

  if (msg->_ParseEnd() == false) {
    return nullptr;
  }

  return msg;
}

MsgBuf MsgBase::Serializate() {
  header.time = std::make_shared<base::time::BaseTime>();

  auto buf = _Serializate();
  if (header.Serializate(buf) == false) {
    return MsgBuf();
  }

  return buf;
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
