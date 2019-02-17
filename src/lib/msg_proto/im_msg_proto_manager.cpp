#include "pch.h"
#include "im_msg_proto.h"
#include "proto_class/msg.pb.h"
#include "base/character_conversion.hpp"

#pragma region namespace
namespace im {
namespace msg_proto {
#pragma endregion


CProtoCallbackManager::CProtoCallbackManager() {};

CProtoCallbackManager *CProtoCallbackManager::Get() {
  static CProtoCallbackManager s;
  return &s;
};

void CProtoCallbackManager::Init() {
  Msg_Pub_TestChannel::RegGlobalCallback();

  PP_CreatePrivateChannel::RegGlobalCallback();
}

void CProtoCallbackManager::RegFunc(EChannelType ct, uint8_t mt,
                                    SMsgParseCB &callback) {
  SMsgType msg;
  msg.channel_type = ct;
  msg.msg_type = mt;
  auto msg_val = msg.GetVal();
#ifdef _DEBUG
  // 断言，重复注册
  assert(msg_func_.find(msg_val) == msg_func_.end());
#endif

  msg_func_[msg_val] = callback;
}

SMsgParseCB CProtoCallbackManager::GetMsgCBInfo(uint16_t msg_val) {
  auto it = msg_func_.find(msg_val);
  if (it == msg_func_.end())
    return SMsgParseCB();
  return it->second;
}



CProtoManager::CProtoManager() {}

CProtoManager::~CProtoManager() {}

void CProtoManager::ParseMsg(EChannelType type, cMsgBuf buf) {
  auto header = std::make_shared<CBaseProtoHeader>(type, buf);
  if (header->IsBroken() == true) {
    PrintLogWarn("Can't parse header  :channel:%d, msg:%d",
      (int)header->type_.channel_type, (int)header->type_.msg_type);
    return;
  }
  auto msg_val = header->type_.GetVal();

  auto it = msg_func_.find(msg_val);
  if (it == msg_func_.end()) {
    PrintLogWarn("Can't find msg callback :channel:%d, msg:%d",
      (int)header->type_.channel_type, (int)header->type_.msg_type);
    return;
  }

  auto func_msg_cb = it->second.msg_cb;
  auto func_msg_parse = it->second.msg_parse;

  if (func_msg_cb == nullptr || func_msg_parse == nullptr) {
    PrintLogWarn("Callback is nullptr")
      return;
  }
  auto msg = func_msg_parse(buf, header);
  if (!msg) {
    PrintLogWarn("Can't parse msg :channel:%d, msg:%d",
      (int)header->type_.channel_type, (int)header->type_.msg_type);
    return;
  }
  func_msg_cb(msg);
}

void CProtoManager::RegFunc(EChannelType ct, uint8_t mt,
                            Func_MsgCb callback) {
  SMsgType msg_type;
  msg_type.channel_type = ct;
  msg_type.msg_type = mt;
  auto msg_val = msg_type.GetVal();

  auto msg_cb = CProtoCallbackManager::Get()->GetMsgCBInfo(msg_val);
#ifdef _DEBUG
  assert(msg_cb.msg_parse != nullptr);
#endif // _DEBUG
  msg_cb.msg_cb = callback;
  msg_func_[msg_val] = msg_cb;
}

#pragma region namespace
}
}
#pragma endregion
