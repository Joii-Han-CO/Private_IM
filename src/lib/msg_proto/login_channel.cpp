#include "pch.h"
#include "im_msg_proto.h"
#include "proto_class/msg.pb.h"

#pragma region namespace
namespace im {
namespace msg_proto {
#pragma endregion

#pragma region Ulity

ELoginMsgType GetLoginMsgType(const MsgBuf &buf) {
  return (ELoginMsgType)buf[0];
}

pMsg_LoginChannel Factory_Login(ELoginMsgType type) {
  pMsg_LoginChannel msg;
  switch (type) {
  case im::msg_proto::ELoginMsgType::Error:
    return nullptr;
  case im::msg_proto::ELoginMsgType::UserLogin:
    return std::make_shared<Msg_UserLogin>();
  default:
    break;
  }
  return nullptr;
}

#pragma endregion

pMsg_LoginChannel Parse_LoginChannel(const MsgBuf &buf) {
  auto type = GetLoginMsgType(buf);
  auto msg = Factory_Login(type);
  if (msg != nullptr && msg->Parse(buf) == true)
    return msg;
  else
    return nullptr;
}

#pragma region UserLogin

Msg_UserLogin::Msg_UserLogin() {
  type = im::msg_proto::ELoginMsgType::UserLogin;
}

bool Msg_UserLogin::Parse(const MsgBuf &buf) {
  if (buf.size() - 1 == 0)
    return false;

  Proto_UserLogin proto;
  if (proto.ParseFromArray(&buf[1], buf.size() - 1) == false)
    return false;

  user_name = base::Utf8ToUtf16(proto.user_name());
  login_channel = base::Utf8ToUtf16(proto.user_pwd());
  client_type = (EClientType)proto.client_type()[0];
  client_id = proto.client_id();

  return true;
}

MsgBuf Msg_UserLogin::Serializate() {
  Proto_UserLogin proto;
  proto.set_user_name(base::Utf16ToUtf8(user_name));
  proto.set_user_pwd(base::Utf16ToUtf8(login_channel));
  proto.set_client_type((char*)&client_type);
  proto.set_client_id(client_id);

  auto size = proto.ByteSizeLong();
  if (size <= 0)
    return MsgBuf();
  MsgBuf buf;
  buf.resize(size + 1);
  proto.SerializeToArray((void*)&buf[1], size);

  buf[0] = (uint8_t)(type);
  return buf;
}

#pragma endregion

#pragma region UserLoginRes

Msg_UserLoginRes::Msg_UserLoginRes() {
  type = im::msg_proto::ELoginMsgType::UserLoginRes;
}

bool Msg_UserLoginRes::Parse(const MsgBuf &buf) {
  if (buf.size() - 1 == 0) {
    return false;
  }
  Proto_UserLoginRes proto;
  if (proto.ParseFromArray(&buf[1], buf.size() - 1) == false) {
    return false;
  }

  status = proto.status();

  return true;
}

MsgBuf Msg_UserLoginRes::Serializate() {
  Proto_UserLoginRes proto;
  proto.set_status(status);
  MsgBuf buf;

  auto size = proto.ByteSizeLong();
  if (size <= 0) {
    return buf;
  }

  buf.resize(size + 1);
  proto.SerializePartialToArray((void*)&buf[1], size);

  buf[0] = (uint8_t)(type);
  return buf;
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
