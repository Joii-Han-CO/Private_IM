#include "pch.h"
#include "im_msg_proto.h"

#pragma region
namespace im {
namespace msg_proto {
#pragma endregion

// 公共通道的消息体

bool PP_CreatePrivateChannel::Parse(cMsgBuf buf) {


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

  proto.ByteSize();
  return MsgBuf();
}

#pragma region
}
}
#pragma endregion
