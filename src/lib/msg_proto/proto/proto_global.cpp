#include "pch.h"
#include "im_msg_proto.h"
#include "base/cross_system.hpp"

#pragma region
namespace im {
namespace msg_proto {
#pragma endregion

// 公共通道的消息体

bool PP_CreatePrivateChannel::Parse(cMsgBuf buf) {
  Proto_CreatePriChannel proto;
  if (proto.ParseFromArray(&buf[header_->Size()],
                           buf.size() - header_->Size()) == false) {
    PrintLogWarn("Parse buf failed in protobuf");
    return false;
  }

  user_name_ = base::Utf8ToUtf16(proto.user_name().Get(0));
  auto ud = proto.channel_name().Get(0);
  memcpy_s(&channel_name_, sizeof(channel_name_), &ud[0], ud.size());

  ud = proto.client_type().Get(0);
  client_type_ = (EClientType)ud[0];

  ud = proto.client_id().Get(0);
  memcpy_s(&client_id_, sizeof(client_id_), &ud[0], ud.size());

  return true;
}

MsgBuf PP_CreatePrivateChannel::Serializate() {
  Proto_CreatePriChannel proto;
  proto.add_user_name(base::Utf16ToUtf8(user_name_));

  auto ref_channel_name = proto.add_channel_name();
  ref_channel_name->resize(sizeof(channel_name_));
  memcpy_s(&(*ref_channel_name)[0], ref_channel_name->size(),
           &channel_name_, sizeof(channel_name_));
  auto ref_client_type = proto.add_client_type();
  ref_client_type->resize(1);
  (*ref_client_type)[0] = (char)client_type_;
  auto ref_client_id_ = proto.add_client_id();
  ref_client_id_->resize(sizeof(client_id_));
  memcpy_s(&(*ref_client_id_)[0], ref_client_id_->size(),
           &client_id_, sizeof(client_id_));
  int size = proto.ByteSize();
  MP_SerializateMsg(size);
  if (proto.SerializeToArray(&buf[header_->Size()],
                             buf.size() - header_->Size()) == false) {
    PrintLogWarn("Serializate msg failed in proto buf");
    return MsgBuf();
  }
  return buf;
}

#pragma region
}
}
#pragma endregion
