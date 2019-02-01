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
  type = (uint8_t)buf[0];

  int64_t time_val = 0;
  memcpy_s(&time_val, sizeof(time_val), &buf[1], sizeof(time_val));
  time = std::make_shared<base::time::BaseTime>(time_val);

  return true;
}

bool MsgBase_Header::Serializate(MsgBuf &buf) {
  if (buf.size() < (size_t)GetSize()) {
    PrintLogErro("Serializate msg header failed, buf too small %d",
                 buf.size());
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
  if (buf.size() <= (size_t)MsgBase_Header::GetSize()) {
    PrintLogErro("Parse msg failed, bufsize too small %d", buf.size());
    return nullptr;
  }
  if (h->Parse(buf) == false) {
    return nullptr;
  }

  pMsgBase msg = nullptr;
  if (h->type < (int)EPubMsgType::end) {
    // 基础消息
    if (h->type == (int)EPubMsgType::test_channel)
      msg = std::make_shared<Msg_TestChannel>();
  }
  else {
    if (t == EChannelType::login)
      msg = MsgBase_Login::Factory(h->type);
  }

  if (msg == nullptr) {
    PrintLogErro("Parse msg failed, unable to find type: %d", (int)h->type);
    return nullptr;
  }

  msg->header.type = h->type;
  msg->header.time = h->time;

  if (msg->_Parse(buf) == false) {
    PrintLogErro("Parse msg failed, chaild class parse faield");
    return nullptr;
  }

  if (msg->_ParseEnd() == false) {
    PrintLogErro("Parse msg failed, chaild class parse end faield");
    return nullptr;
  }

  return msg;
}

MsgBuf MsgBase::Serializate() {
  header.time = std::make_shared<base::time::BaseTime>();

  auto buf = _Serializate();
  if (header.Serializate(buf) == false) {
    PrintLogErro("Serializate msg failed, chaild class serializate failed");
    return MsgBuf();
  }

  return buf;
}

#pragma endregion

#pragma region TestChannel

// TODO 暂时不用proto，因为太麻烦了

bool Msg_TestChannel::_Parse(const MsgBuf &buf) {
  if (buf.size() < MsgBase_Header::GetSize() + sizeof(int)) {
    PrintLogErro("Parse msg faield, buf too small %d", buf.size());
    return false;
  }
  status_ = 0;
  memcpy_s(&status_, sizeof(status_),
           &buf[MsgBase_Header::GetSize()], sizeof(status_));
  return true;
}

bool Msg_TestChannel::_ParseEnd() {
  return true;
}

MsgBuf Msg_TestChannel::_Serializate() {
  int size = MsgBase_Header::GetSize() + sizeof(status_);

  MsgBuf buf;
  buf.resize(size);
  memcpy_s(&buf[MsgBase_Header::GetSize()], sizeof(status_),
           &status_, sizeof(status_));

  return buf;
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
