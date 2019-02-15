#include "pch.h"
#include "im_msg_proto.h"
#include "proto_class/msg.pb.h"

#pragma region namespace
namespace im {
namespace msg_proto {
#pragma endregion

bool Proto_G_PrivateInfo::Parse(cMsgBuf buf) {
  return false;
}

MsgBuf Proto_G_PrivateInfo::Serializate() {
  return MsgBuf();
}

#pragma region namespace
}
}
#pragma endregion
