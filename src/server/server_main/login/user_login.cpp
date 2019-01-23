#include "pch.h"
#include "user_login.h"

#pragma region namespace
namespace server {
#pragma endregion

ServerUserLogin::ServerUserLogin(std::wstring user_name,
                                 std::wstring channel_name,
                                 im::pCMqttClient mqtt):
  user_name_(user_name),
  channel_name_(channel_name),
  mqtt_(mqtt) {}

ServerUserLogin::~ServerUserLogin() {
  // 需要断开连接
}

bool ServerUserLogin::Init() {
  if (user_name_.empty()) {
    PrintLogWarn("user login user_name_ is empty");
    return false;
  }
  if (channel_name_.empty()) {
    PrintLogWarn("user login channel_name_ is empty");
    return false;
  }
  if (mqtt_ == nullptr) {
    PrintLogErro("user login mqtt is nullptr");
    return false;
  }

  auto func = [this](bool suc) {
    // 订阅成功后，通知客户端
    im::msg_proto::Msg_UserLoginRes msg;
    msg.status = 1;
    auto buf = msg.Serializate();
    auto func = [this](bool suc) {};
    if (suc == false) {
      PrintLogWarn("subscribe mqtt first msg failed");
      return;
    }
    if (mqtt_->Publish(base::Utf16ToUtf8(channel_name_), buf, func) == false) {
      PrintLogWarn("");
      return;
    }
  };

  if (mqtt_->Subscribe(base::Utf16ToUtf8(channel_name_), func,
                       std::bind(&ServerUserLogin::RecvMsg,
                                 this, std::placeholders::_1)) == false) {
    PrintLogWarn(
      L"user login subscribe user channel failed, channel name:%s, des:%s",
      channel_name_.c_str(), mqtt_->GetLastErr_Wc());
    return false;
  }

  PrintLogInfo(L"a new user login: %s", user_name_.c_str());
  return true;
}

void ServerUserLogin::RecvMsg(const MsgBuf &buf) {}

#pragma region namespace
}
#pragma endregion
