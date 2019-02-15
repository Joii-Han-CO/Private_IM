#include "pch.h"
#include "user_login.h"

#pragma region namespace
namespace server {
#pragma endregion

ServerUserLogin::ServerUserLogin(uint32_t id,
                                 std::wstring user_name,
                                 std::wstring channel_name,
                                 im::pCMqttClient mqtt,
                                 std::function<void(uint32_t)> exit_func):
  id_(id),
  user_name_(user_name),
  channel_name_(channel_name),
  mqtt_(mqtt),
  exit_func_(exit_func) {}

ServerUserLogin::~ServerUserLogin() {
  // 需要断开连接
}

bool ServerUserLogin::Init() {
  if (user_name_.empty()) {
    PrintLogWarn("User login user_name_ is empty");
    return false;
  }
  if (channel_name_.empty()) {
    PrintLogWarn("User login channel_name_ is empty");
    return false;
  }
  if (mqtt_ == nullptr) {
    PrintLogErro("User login mqtt is nullptr");
    return false;
  }

  auto func = [this](bool suc) {
    if (suc == false) {
      PrintLogErro("User login subscribe user channel failed");
      LogoutExit();
      return;
    }
    else {
      if (SendLoginStatus() == false) {
        return;
      }
    }
  };

  if (mqtt_->Subscribe(base::Utf16ToUtf8(channel_name_), func,
                       std::bind(&ServerUserLogin::RecvMsg,
                                 this, std::placeholders::_1)) == false) {
    PrintLogWarn(
      L"User login subscribe user channel failed, channel name:%s, des:%s",
      channel_name_.c_str(), mqtt_->GetLastErr_Wc());
    return false;
  }
  PrintLogInfo(L"A new user login: %s", user_name_.c_str());

  return true;
}

bool ServerUserLogin::Uninit() {
  return false;
}

// 发送消息
bool ServerUserLogin::SendMsg(cMsgBuf buf,
                              const Func_AsyncResult func) {
  return mqtt_->Publish(base::Utf16ToUtf8(channel_name_), buf, func);
}

// 当前用户发送的消息
void ServerUserLogin::RecvMsg(cMsgBuf buf) {
  auto msg = im::msg_proto::MsgBase_Login::Parse(buf);
  if (msg == nullptr) {
    PrintLogWarn("Receive a unknow msg, msg size:%d", buf.size());
    return;
  }

  if (msg->type_ == im::msg_proto::ELoginMsgType::Error) {
    PrintLogWarn("Receive a msg, but can't parse, msg size:%d", buf.size());
    return;
  }

  switch (msg->type_) {
  case (im::msg_proto::ELoginMsgType::UserLoginCRes):
    ClientLoginRes(
      std::dynamic_pointer_cast<im::msg_proto::Msg_UserLoginClientRes>(msg));
    break;
  default:
    break;
  }
}

#pragma region Business related

bool ServerUserLogin::SendLoginStatus() {
  im::msg_proto::Msg_UserLoginSRes proto;
  proto.status = 1;
  auto buf = proto.Serializate();
  auto func_ref = [this](bool suc) {
    if (suc == false) {
      PrintLogErro("Send login res msg failed in callback");
      LogoutExit();
      return;
    }
  };

  if (SendMsg(buf, func_ref) == false) {
    PrintLogErro("Send login res msg failed, des: %s",
                 mqtt_->GetLastErr_Astd().c_str());
    LogoutExit();
    return false;
  }
  return true;
}

// 收到客户端登陆消息返回
void ServerUserLogin::ClientLoginRes(
  const im::msg_proto::pMsg_UserLoginClientRes msg
) {
  PrintLogDbg("Get LoginCRes msg, status :%d", msg->status);
  if (msg->status == 1) {
    PrintLogInfo(L"User %s login success!", user_name_.c_str());
  }
  else {
    PrintLogErro("User login failed, will logout and exit");
    LogoutExit();
  }
}

// 用户登陆成功之后触发
void ServerUserLogin::LoginSuccess() {}

// 触发登出操作
void ServerUserLogin::LogoutExit() {
  if (exit_func_) {
    mqtt_->Unsubscribe(base::Utf16ToUtf8(channel_name_));
    exit_func_(id_);
  }
}

#pragma endregion

#pragma region namespace
}
#pragma endregion
