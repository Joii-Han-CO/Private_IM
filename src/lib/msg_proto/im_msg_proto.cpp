﻿#include "pch.h"
#include "im_msg_proto.h"
#include "proto_class/msg.pb.h"
#include "base/character_conversion.hpp"

#pragma region namespace
namespace im {
namespace msg_proto {
#pragma endregion

pSIM_MsgText ParseText(void *data, size_t len) {
  return nullptr;
}

MsgBuffer SerializationTextMsg(pSIM_MsgText msg) {
  return MsgBuffer();
}

#pragma region namespace
}
}
#pragma endregion
