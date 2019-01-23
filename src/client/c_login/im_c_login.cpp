﻿#include "pch.h"
#include "im_c_login.h"

#pragma region namespace
namespace im {
namespace c_login {
#pragma endregion

#pragma region Init

ClientLogin::ClientLogin() {}

ClientLogin::~ClientLogin() {}

// 初始化
bool ClientLogin::Init(std::wstring user_name, std::wstring user_pwd,
                       Func_AsyncResult finished) {
  if (is_init_ == true) {
    PrintLogWarn("is init ed");
    return true;
  }
  if (user_name.empty() || user_pwd.empty() || finished == nullptr) {
    PrintLogErro("user_name or user_pwd or finished is empty");
    return false;
  }
  init_finished_callback_ = finished;
  user_name_ = user_name;
  if (InitMqtt(user_name, user_pwd) == false) {
    return false;
  }
  is_init_ = true;
  return true;
}

// 释放
bool ClientLogin::Uninit(Func_AsyncResult finished) {
  uninit_finished_callback_ = finished;

  MqttSendLogoutInfo();
  return true;
}

// 初始化mqtt和登陆连接
bool ClientLogin::InitMqtt(std::wstring user_name,
                           std::wstring user_pwd) {
  if (mqtt_ != nullptr) {
    PrintLogErro("mqtt not empty");
    return false;
  }
  mqtt_ = std::make_shared<im::CMqttClient>(
    std::bind(&ClientLogin::MqttLog, this, std::placeholders::_1));

  im::SMqttConnectInfo mqtt_info;
  mqtt_info.user_name = base::Utf16ToUtf8(user_name);
  mqtt_info.user_pwd = base::Utf16ToUtf8(user_pwd);

  mqtt_info.cb_status_change =
    std::bind(&ClientLogin::MqttConnectedStatusChanged,
              this, std::placeholders::_1);
  mqtt_->Connect(mqtt_info);

  return true;
}

// 初始化结束封装
void ClientLogin::InitFinished(bool suc) {
  if (init_finished_callback_) {
    init_finished_callback_(suc);
    init_finished_callback_ = nullptr;
  }
}

void ClientLogin::UninitFinished(bool suc) {
  if (uninit_finished_callback_) {
    uninit_finished_callback_(suc);
    uninit_finished_callback_ = nullptr;
  }
}

#pragma endregion

#pragma region mqtt

// 获取mqtt句柄
im::pCMqttClient ClientLogin::GetMqtt() {
  return mqtt_;
}

// mqtt--注册回调函数
uint32_t ClientLogin::RegMqttConnectedStatusChanged(
  FUNC_StatusChange func) {
  auto count = mqtt_status_changed_func_count_++;
  mqtt_status_changed_func_.insert(
    std::pair<uint32_t, im::FUNC_StatusChange>(count, func));
  return count;
}

// mqtt--卸载回调函数
void ClientLogin::UnregMqttConnectedStatusChanged(uint32_t count) {
  auto it = mqtt_status_changed_func_.find(count);
  if (it != mqtt_status_changed_func_.end())
    mqtt_status_changed_func_.erase(it);
}

// mqtt--日志返回
void ClientLogin::MqttLog(const base::log::SBaseLog &l) {
  im::log::CLog::Get()->Print(l.type, "mqtt", l.file, l.func, l.line, l.log);
}

// mqtt--接到消息后处理
void ClientLogin::MqttConnectedStatusChanged(
  im::EMqttOnlineStatus status) {
  // 处理...
  switch (status) {
  case im::EMqttOnlineStatus::none:
    break;
  case im::EMqttOnlineStatus::connecting:
    PrintLogInfo("Mqtt connecting");
    break;
  case im::EMqttOnlineStatus::connected:
    MqttSubPublicChannel();
    PrintLogInfo("Mqtt connected success");
    break;
  case im::EMqttOnlineStatus::disconnecting:
    PrintLogInfo("Mqtt disconnecting");
    break;
  case im::EMqttOnlineStatus::disconnected:
    PrintLogInfo("Mqtt disconnected");
    break;
  default:
    break;
  }

  for (auto it : mqtt_status_changed_func_)
    if (it.second != nullptr)
      it.second(status);
}

// 登陆--订阅公共通道消息
void ClientLogin::MqttSubPublicChannel() {
  // 订阅公共通道

  // 生成Mqtt通道
  channel_name_login_ =
    im::gv::g_mqtt_login_sub_ + base::_uuid::GenerateUUID<char>();
  auto func = [this](bool suc) {
    // 订阅成功后 ,发送该通道及用户名
    PrintLogInfo("subscribe login channel success");
    MqttSendLoginInfo();
  };

  if (mqtt_->Subscribe(channel_name_login_, func,
                       std::bind(&ClientLogin::MqttLoginChannel,
                                 this, std::placeholders::_1)) == false) {
    PrintLogErro(
      "subscribe login channel faile, sub_name:%s, des:%s",
      channel_name_login_.c_str(), mqtt_->GetLastErr_Astd().c_str());
    InitFinished(false);
  }
}

// 登陆--登陆消息通道
void ClientLogin::MqttLoginChannel(const MsgBuf &buf) {

}

// 登陆--发送登陆信息
void ClientLogin::MqttSendLoginInfo() {
  // 拼接消息
  im::msg_proto::Msg_UserLogin proto;
  proto.user_name = user_name_;
  proto.login_channel = base::Utf8ToUtf16(channel_name_login_);
  proto.client_type = g_client_type_;
  proto.client_id = 0;

  auto buf = proto.Serializate();

  auto func = [this](bool suc) {
    PrintLogInfo("send user login info success");
    InitFinished(true);
  };

  if (mqtt_->Publish(im::gv::g_mqtt_pub_sub_, buf, func) == false) {
    PrintLogErro("send user login info failed, topic:%s, des:%s",
                 im::gv::g_mqtt_pub_sub_.c_str(),
                 mqtt_->GetLastErr_Astd().c_str());
    InitFinished(false);
    return;
  }
}

// 登出--发送消息
void ClientLogin::MqttSendLogoutInfo() {
  im::msg_proto::Msg_UserLogout proto;
  proto.status = 0;
  auto buf = proto.Serializate();
  auto func = [this](bool suc) {
    if (suc == false)
      UninitFinished(false);

    // 貌似不能在这里调用断开连接...
    mqtt_->Disconnect();
  };
  if (mqtt_->Publish(im::gv::g_mqtt_pub_sub_, buf, func) == false) {
    PrintLogErro("send user logout info failed, topic:%s, des:%s",
                 im::gv::g_mqtt_pub_sub_.c_str(),
                 mqtt_->GetLastErr_Astd().c_str());
    return;
  }
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
