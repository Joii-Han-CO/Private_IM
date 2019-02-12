#include "pch.h"
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

  SendLogoutInfo();
  return true;
}

// 初始化mqtt和登陆连接
bool ClientLogin::InitMqtt(std::wstring user_name,
                           std::wstring user_pwd) {
  if (mqtt_ != nullptr) {
    PrintLogErro("Mqtt not empty");
    return false;
  }
  mqtt_ = std::make_shared<im::CMqttClient>(
    std::bind(&ClientLogin::MqttLog, this, std::placeholders::_1));

  im::SMqttConnectInfo mqtt_info;
  mqtt_info.user_name = base::Utf16ToUtf8(user_name);
  mqtt_info.user_pwd = base::Utf16ToUtf8(user_pwd);

  auto gc = im::c_framework::ClientFramework::Get()->GetGlobalConfigPtr();

  mqtt_info.host = gc->GetValA(L"mqtt", L"host");
  mqtt_info.port = gc->GetVal_Int(L"mqtt", L"port");
  mqtt_info.client_id = "2D8596A6-F267-4521-BF6D-880E3E2C4D30";

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
    SubChannel();
    PrintLogInfo("Mqtt connected success");
    break;
  case im::EMqttOnlineStatus::disconnecting:
    PrintLogInfo("Mqtt disconnecting");
    break;
  case im::EMqttOnlineStatus::disconnected:
    PrintLogInfo("Mqtt disconnected");
    break;
  case im::EMqttOnlineStatus::error:
    MqttConnectError();
    break;
  default:
    break;
  }
}

// mqtt--连接错误
void ClientLogin::MqttConnectError() {
  InitFinished(false);
}

bool ClientLogin::SendMsg(const MsgBuf &buf, Func_AsyncResult func) {
  if (mqtt_) {
    return mqtt_->Publish(channel_name_login_, buf, func);
  }
  return false;
}

#pragma endregion

#pragma region Business related

// 登陆--订阅公共通道消息
void ClientLogin::SubChannel() {
  // 订阅公共通道

  // 生成Mqtt通道
  channel_name_login_ =
    im::gv::g_mqtt_login_sub_ + base::_uuid::GenerateUUID<char>();
  auto func = [this](bool suc) {
    // 订阅成功后 ,发送该通道及用户名
    PrintLogInfo("Subscribe login channel success");
    SendLoginInfo();
  };

  if (mqtt_->Subscribe(channel_name_login_, func,
                       std::bind(&ClientLogin::MsgChannel,
                                 this, std::placeholders::_1)) == false) {
    PrintLogErro(
      "Subscribe login channel faile, sub_name:%s, des:%s",
      channel_name_login_.c_str(), mqtt_->GetLastErr_Astd().c_str());
    InitFinished(false);
  }
}

// 登陆--登陆消息通道
void ClientLogin::MsgChannel(const MsgBuf &buf) {
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
  case im::msg_proto::ELoginMsgType::UserLoginSRes:
    Msg_LoginRes(
      std::dynamic_pointer_cast<im::msg_proto::Msg_UserLoginSRes>(msg));
    break;
  default:
    break;
  }
}

// 登陆--收到登陆成功状态
void ClientLogin::Msg_LoginRes(
  const im::msg_proto::pMsg_UserLoginSRes msg
) {
  PrintLogDbg("Get LoginSRes msg, status :%d", msg->status);
  if (msg->status != 1) {
    // 登陆失败，需要触发错误回调
    PrintLogWarn("LoginRes retuan not 1");
    return;
  }

  im::msg_proto::Msg_UserLoginClientRes proto;
  proto.status = 1;
  auto buf = proto.Serializate();

  auto func = [this](bool suc) {
    InitFinished(suc);
  };

  if (SendMsg(buf, func) == false) {
    PrintLogErro("Send client login res msg faield");
    InitFinished(false);
    return;
  }
}

// 登陆--发送登陆信息
void ClientLogin::SendLoginInfo() {
  // 拼接消息
  im::msg_proto::Msg_UserLogin proto;
  proto.user_name = user_name_;
  proto.login_channel = base::Utf8ToUtf16(channel_name_login_);
  proto.client_type = g_client_type_;
  proto.client_id = 0;

  auto buf = proto.Serializate();

  auto func = [this](bool suc) {
    PrintLogInfo("Send user login info success");

    // 需要等待平台返回...
    //InitFinished(true);
  };

  if (mqtt_->Publish(im::gv::g_mqtt_pub_sub_, buf, func) == false) {
    PrintLogErro("Send user login info failed, topic:%s, des:%s",
                 im::gv::g_mqtt_pub_sub_.c_str(),
                 mqtt_->GetLastErr_Astd().c_str());
    InitFinished(false);
    return;
  }
}

// 登出--发送消息
void ClientLogin::SendLogoutInfo() {
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
    PrintLogErro("Send user logout info failed, topic:%s, des:%s",
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
