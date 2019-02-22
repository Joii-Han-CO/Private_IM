#include "pch.h"
#include "im_c_login.h"

#pragma region namespace
namespace im {
namespace c_login {
#pragma endregion

#pragma region Interface

ClientLogin::ClientLogin() {}

ClientLogin::~ClientLogin() {}

// 初始化
bool ClientLogin::Init(std::wstring user_name, std::wstring user_pwd,
                       Func_AsyncResult finished) {
  assert(finished != nullptr);
  init_finished_ = finished;
  user_name_ = user_name;

  auto cfg = im::c_framework::ClientFramework::Get()->GetGlobalConfigPtr();
  if (cfg == nullptr) {
    PrintLogErro("get global config is nullptr");
    return false;
  }

  // 注册消息
  pri_manager_ = std::make_shared<im::msg_proto::CProtoManager>();
  im::msg_proto::PR_ResLoginStatus::RegCallback(
    pri_manager_.get(), std::bind(&ClientLogin::OnMsg_ResLoginStatus,
                                  this, std::placeholders::_1));

  // 初始化mqtt
  im::SMqttConnectInfo mqtt_init;
  mqtt_init.host = cfg->GetValA(L"mqtt", L"host");
  mqtt_init.port = cfg->GetVal_Int(L"mqtt", L"port");
  mqtt_init.cb_status_change = std::bind(&ClientLogin::MqttConnectStatus,
                                         this, std::placeholders::_1);

  mqtt_init.user_name = base::Utf16ToUtf8(user_name);
  mqtt_init.user_pwd = base::Utf16ToUtf8(user_pwd);

  mqtt_ = std::make_shared<im::CMqttClient>();
  if (mqtt_->Connect(mqtt_init) == false) {
    PrintLogErro("connect mqtt failed");
    return false;
  }

  return true;
}

// 释放
bool ClientLogin::Uninit(Func_AsyncResult finished) {
  return true;
}

// 获取mqtt句柄
im::pCMqttClient ClientLogin::GetMqtt() {
  return mqtt_;
}

#pragma endregion

#pragma region init

void ClientLogin::UninitFinished(bool suc) {
  if (func_uninit_finished_) {
    func_uninit_finished_(suc);
    func_uninit_finished_ = nullptr;
  }
};

#pragma endregion

#pragma region mqtt

void ClientLogin::MqttConnectStatus(EMqttOnlineStatus status) {
  switch (status) {
  case im::EMqttOnlineStatus::none:
    break;
  case im::EMqttOnlineStatus::connecting:
    break;
  case im::EMqttOnlineStatus::connected:
    MqttConnected();
    break;
  case im::EMqttOnlineStatus::disconnecting:
    break;
  case im::EMqttOnlineStatus::disconnected:
    break;
  case im::EMqttOnlineStatus::error:
    MqttConnectError();
    break;
  default:
    break;
  }
}

// mqtt 连接成功之后..
void ClientLogin::MqttConnected() {
  CreatePriChannel();
}

void ClientLogin::MqttConnectError() {
  InitFinished(false);
  UninitFinished(false);
}

#pragma endregion

#pragma region

// 创建私有通道
void ClientLogin::CreatePriChannel() {
  pub_channel_ = std::make_shared<im::msg::CBaseMsgSend>(
    mqtt_, im::gv::g_mqtt_pub_sub_);

  base::_uuid::BUID channel_id;
  base::_uuid::Generate(&channel_id);

  pri_channel_ = std::make_shared<im::msg::CPriChannel>();
  im::msg::CPriChannel::SInitAgrs args;
  args.mqtt = mqtt_;
  args.caller = true;
  args.channel_id = &channel_id;
  args.manager = pri_manager_;

  auto func = [this, channel_id](bool suc) {
    if (suc == false) {
      PrintLogErro("Init private channel failed 2");
      InitFinished(false);
      return;
    }
    else {
      CreatePriChannelFinished(channel_id);
    }
  };
  if (pri_channel_->Init(&args, func) == false) {
    PrintLogErro("Init private channel failed");
    return;
  }
}

// 创建私有通道成功之后
void ClientLogin::CreatePriChannelFinished(base::_uuid::BUID channel_id) {
  // 发送公共通道消息消息
  im::msg_proto::PP_CreatePrivateChannel proto;
  proto.channel_name_ = channel_id;
  proto.user_name_ = user_name_;
  proto.client_type_ = EClientType::win;
  base::_uuid::Generate(&proto.client_id_);

  auto buf = proto.Serializate();
  if (buf.empty()) {
    PrintLogErro(
      "Serializate proto buf (create channel) failed, buf is empty");
    InitFinished(false);
    return;
  }

  auto func_send = [this](bool suc) {
    if (suc == false) {
      PrintLogErro("Publish create channel msg failed 2");
      InitFinished(false);
      return;
    }

    // 等待通道建立成功之后才会返回
    // 设置超时时间，防止卡死
    ClientFW->GetTimerPtr()->AddTimer(
      30 * 1000,
      std::bind(&ClientLogin::LoginTimeOut, this));
  };

  if (pub_channel_->Send(buf, nullptr) == false) {
    PrintLogErro("Publish create channel msg failed");
    InitFinished(false);
    return;
  }
}

// 登陆超时
void ClientLogin::LoginTimeOut() {
  PrintLogErro("Login timeout");
  InitFinished(false);
  return;
}

// 从平台返回的消息
void ClientLogin::OnMsg_ResLoginStatus(im::msg_proto::pPR_ResLoginStatus msg) {
  PrintLogInfo("Recv login resust status");
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
