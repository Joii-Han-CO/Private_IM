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
  func_init_finished_ = finished;
  user_name_ = user_name;
  auto cfg = im::c_framework::ClientFramework::Get()->GetGlobalConfigPtr();
  if (cfg == nullptr) {
    PrintLogErro("get global config is nullptr");
    func_init_finished_ = nullptr;
    return false;
  }

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
    func_init_finished_ = nullptr;
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

void ClientLogin::InitFinished(bool suc) {
  if (func_init_finished_) {
    func_init_finished_(suc);
    func_init_finished_ = nullptr;
  }
};

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
  msg_ = std::make_shared<im::msg::CMsg>();

  im::msg::SMsg_InitArgs args;
  args.client = true;
  args.channel_name = channel_name_ =
    base::Utf8ToUtf16(im::gv::g_mqtt_login_sub_) +
    base::_uuid::GenerateUUID<wchar_t>();
  args.mqtt = mqtt_;
/*

  args.init_finished = [this](bool suc) {
    if (suc == false) {
      PrintLogErro("init msg failed");
      InitFinished(false);
      return;
    }

    if (msg_->SendTestMsg() == false) {
      PrintLogErro("send test msg failed");
      InitFinished(false);
      return;
    }
  };
  args.connect_finished = [this](bool suc) {
    if (suc == false) {
      PrintLogErro(L"connect %s failed", channel_name_.c_str());
      return;
    }
  };*/

  msg_->Init(&args);
}

void ClientLogin::MqttConnectError() {
  InitFinished(false);
  UninitFinished(false);
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
