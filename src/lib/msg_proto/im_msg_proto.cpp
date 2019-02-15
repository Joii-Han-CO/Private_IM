#include "pch.h"
#include "im_msg_proto.h"
#include "proto_class/msg.pb.h"
#include "base/character_conversion.hpp"

#pragma region namespace
namespace im {
namespace msg_proto {
#pragma endregion

#pragma region Header

CBaseProtoHeader::CBaseProtoHeader(EChannelType channel_type, uint8_t type) {
  type_.channel_type = channel_type;
  type_.msg_type = type;
  time_ = std::make_shared<base::time::BaseTime>();
  base::_uuid::GenerateUUID(&msg_id_);
}

CBaseProtoHeader::CBaseProtoHeader(EChannelType channel_type, uint8_t type,
                                   base::_uuid::BaseUUID *msg_id) {
  type_.channel_type = channel_type;
  type_.msg_type = type;
  time_ = std::make_shared<base::time::BaseTime>();
  memcpy_s(&msg_id_, sizeof(msg_id_), msg_id, sizeof(msg_id_));
}

CBaseProtoHeader::CBaseProtoHeader(EChannelType channel_type,
                                   cMsgBuf buf) {
  auto size = Size();
  if (buf.size() <= (size_t)size)
    return;

  int count = 0;

  type_.channel_type = channel_type;
  memcpy_s(&type_.msg_type, sizeof(type_.msg_type),
           &buf[count], sizeof(type_.msg_type));
  count += sizeof(type_.msg_type);

  int64_t time_val = 0;
  memcpy_s(&time_val, sizeof(time_val), &buf[count], sizeof(time_val));
  time_ = std::make_shared<base::time::BaseTime>(time_val);
  count += sizeof(time_val);

  memcpy_s(&msg_id_, sizeof(msg_id_), &buf[count], sizeof(msg_id_));
}

int CBaseProtoHeader::Size() {
  return sizeof(uint8_t) + sizeof(int64_t) + sizeof(base::_uuid::BaseUUID);
}

bool CBaseProtoHeader::Serializate(MsgBuf &buf) {
  auto size = Size();
  if (buf.size() <= (size_t)size) {
    PrintLogWarn("Can't serializate header, buf size too small size:%d",
                 buf.size());
    return false;
  }

  int count = 0;

  memcpy_s(&buf[count], sizeof(type_.msg_type),
           &type_.msg_type, sizeof(type_.msg_type));
  count += sizeof(type_.msg_type);

  int64_t time_val = time_->GetVal();
  memcpy_s(&buf[count], sizeof(time_val), &time_val, sizeof(time_val));
  count += sizeof(time_val);

  memcpy_s(&buf[count], sizeof(msg_id_), &msg_id_, sizeof(msg_id_));

  return true;
}

bool CBaseProtoHeader::IsBroken() {
  if (time_ == nullptr)
    return true;
  return false;
}

#pragma endregion

#pragma region pub_test_channel

bool Msg_Pub_TestChannel::Parse(cMsgBuf buf) {
  memcpy_s(&status, sizeof(status), &buf[header_->Size()], sizeof(status));
  return true;
}

MsgBuf Msg_Pub_TestChannel::Serializate() {
  int buf_size = sizeof(int);
  MP_SerializateMsg(buf_size);

  memcpy_s(&buf[CBaseProtoHeader::Size()], sizeof(int),
           &status, sizeof(int));
  return buf;
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
