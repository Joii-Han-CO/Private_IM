#include "pch.h"
#include "im_msg.h"

#pragma region namespace
namespace im {
namespace msg {
#pragma endregion

#pragma region Interface

CMsg::CMsg() {}

CMsg::~CMsg() {}

bool CMsg::Init(SInitArgs *info) {
  if (info == nullptr) {
    PrintLogErro("cmsg init failed, info is nullptr");
    return false;
  }
  if (info->user_name.empty() || info->channel_name.empty() ||
      info->mqtt == nullptr) {
    PrintLogErro("CMsg init failed, info is nullptr");
    return false;
  }
  func_connected_ = info->func_connected;
  mqtt_ = info->mqtt;

  auto channel_name = base::Utf16ToUtf8(info->channel_name);
  if (is_client_) {
    recv_channel_name_ = channel_name + "_C";
    send_channel_name_ = channel_name + "_S";
  }
  else {
    recv_channel_name_ = channel_name + "_S";
    send_channel_name_ = channel_name + "_C";
  }

  if (_Init() == false)
    return false;

  return true;
}

bool CMsg::Uninit() {
  return false;
}

bool CMsg::SendMsg(cMsgBuf buf, Func_AsyncResult func) {
  return mqtt_->Publish(send_channel_name_, buf, func);
}

void CMsg::Connected(bool suc) {
  if (func_connected_)
    func_connected_(suc);
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
