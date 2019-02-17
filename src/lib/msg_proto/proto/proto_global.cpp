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
  return MsgBuf();
}

#pragma region
}
}
#pragma endregion
