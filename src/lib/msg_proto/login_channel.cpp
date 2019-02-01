#include "pch.h"
#include "im_msg_proto.h"
#include "proto_class/msg.pb.h"

#pragma region namespace
namespace im {
namespace msg_proto {
#pragma endregion

#pragma region Base

MsgBase_Login::MsgBase_Login() {

}

pMsgBase_Login MsgBase_Login::Parse(const MsgBuf &buf) {
  auto msg = MsgBase::Parse(buf, EChannelType::login);
  return std::dynamic_pointer_cast<MsgBase_Login>(msg);
}

pMsgBase MsgBase_Login::Factory(uint8_t type) {
  auto t = (ELoginMsgType)type;
  switch (t) {
  case im::msg_proto::ELoginMsgType::Error:
    return nullptr;
  case im::msg_proto::ELoginMsgType::UserLogin:
    return std::make_shared<Msg_UserLogin>();
  case im::msg_proto::ELoginMsgType::UserLoginSRes:
    return std::make_shared<Msg_UserLoginSRes>();
  case im::msg_proto::ELoginMsgType::UserLoginCRes:
    return std::make_shared<Msg_UserLoginClientRes>();
  case im::msg_proto::ELoginMsgType::UserLogout:
    return std::make_shared<Msg_UserLogout>();
  default:
    return nullptr;
  }
}

bool MsgBase_Login::_ParseEnd() {
  type_ = (ELoginMsgType)header.type;

  return true;
}

#pragma endregion

#pragma region Msg_UserLogin

bool Msg_UserLogin::_Parse(const MsgBuf &buf) {
  Proto_UserLogin proto;
  if (proto.ParseFromArray(&buf[MsgBase_Header::GetSize()],
                           buf.size() - MsgBase_Header::GetSize()) == false) {
    PrintLogErro("Child parse msg failed, protobuf return false");
    return false;
  }

  user_name = base::Utf8ToUtf16(proto.user_name());
  login_channel = base::Utf8ToUtf16(proto.user_pwd());
  client_type = (EClientType)proto.client_type()[0];
  client_id = proto.client_id();

  return true;
}

MsgBuf Msg_UserLogin::_Serializate() {
  Proto_UserLogin proto;
  proto.set_user_name(base::Utf16ToUtf8(user_name));
  proto.set_user_pwd(base::Utf16ToUtf8(login_channel));
  proto.set_client_type((char*)&client_type);
  proto.set_client_id(client_id);

  auto size = proto.ByteSizeLong();
  if (size <= 0) {
    PrintLogErro("Chaild serializate msg failed, protobuf refsize :%d", size);
    return MsgBuf();
  }
  MsgBuf buf;
  buf.resize(size + MsgBase_Header::GetSize());
  proto.SerializeToArray((void*)&buf[MsgBase_Header::GetSize()], size);
  return buf;
}

#pragma endregion

#pragma region UserLoginSRes

bool Msg_UserLoginSRes::_Parse(const MsgBuf &buf) {
  Proto_UserLoginRes proto;
  if (proto.ParseFromArray(&buf[MsgBase_Header::GetSize()],
                           buf.size() - MsgBase_Header::GetSize()) == false) {
    PrintLogErro("Child parse msg failed, protobuf return false");
    return false;
  }

  status = proto.status();
  return true;
}

MsgBuf Msg_UserLoginSRes::_Serializate() {
  Proto_UserLoginRes proto;
  proto.set_status(status);
  MsgBuf buf;

  auto size = proto.ByteSizeLong();
  if (size <= 0) {
    PrintLogErro("Chaild serializate msg failed, protobuf refsize :%d", size);
    return MsgBuf();
  }

  buf.resize(size + MsgBase_Header::GetSize());
  proto.SerializePartialToArray((void*)&buf[MsgBase_Header::GetSize()], size);

  return buf;
}

#pragma endregion

#pragma region UserLoginCRes

bool Msg_UserLoginClientRes::_Parse(const MsgBuf &buf) {
  Proto_UserLoginRes proto;
  if (proto.ParseFromArray(&buf[MsgBase_Header::GetSize()],
                           buf.size() - MsgBase_Header::GetSize()) == false) {
    PrintLogErro("Child parse msg failed, protobuf return false");
    return false;
  }

  status = proto.status();
  return true;
}

MsgBuf Msg_UserLoginClientRes::_Serializate() {
  Proto_UserLoginRes proto;
  proto.set_status(status);
  MsgBuf buf;

  auto size = proto.ByteSizeLong();
  if (size <= 0) {
    PrintLogErro("Chaild serializate msg failed, protobuf refsize :%d", size);
    return MsgBuf();
  }

  buf.resize(size + MsgBase_Header::GetSize());
  proto.SerializePartialToArray((void*)&buf[MsgBase_Header::GetSize()], size);

  return buf;
}

#pragma endregion

#pragma region UserLogout

bool Msg_UserLogout::_Parse(const MsgBuf &buf) {
  Proto_UserLoginRes proto;
  if (proto.ParseFromArray(&buf[MsgBase_Header::GetSize()],
                           buf.size() - MsgBase_Header::GetSize()) == false) {
    PrintLogErro("Child parse msg failed, protobuf return false");
    return false;
  }

  status = proto.status();

  return true;
}

MsgBuf Msg_UserLogout::_Serializate() {
  Proto_UserLoginRes proto;
  proto.set_status(status);
  MsgBuf buf;

  auto size = proto.ByteSizeLong();
  if (size <= 0) {
    PrintLogErro("Chaild serializate msg failed, protobuf refsize :%d", size);
    return MsgBuf();
  }

  buf.resize(size + MsgBase_Header::GetSize());
  proto.SerializePartialToArray((void*)&buf[MsgBase_Header::GetSize()], size);

  return buf;
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
