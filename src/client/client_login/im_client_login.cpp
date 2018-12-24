#include "pch.h"
#include "im_client_login.h"
#include "im_global_val.h"
#include "im_log.h"
#include "base/uuid.hpp"


#pragma region namespace
namespace im {
namespace login {
#pragma endregion

Login::Login() {}

Login::~Login() {}

bool Login::Connect(std::wstring user_name, std::string user_pwd,
                    Func_LoginResult func_done) {
  PrintLogInfo("Begin Init");
  global_config_ = std::make_shared<im::config::CConfig>();
  if (global_config_->Init(im::gv::g_global_config_file_path) == false) {
    SetLastErr(L"Init config file failed, des:%s",
               global_config_->GetLastErr_Wc());
    return false;
  }

  if (InitMqtt(user_name, user_pwd) == false)
    return false;

  PrintLogInfo("Init Success");
  return true;
}

bool Login::Reconnect(Func_LoginResult func_done) {
  return false;
}

bool Login::Disconnect(Func_LoginResult func_done) {
  return false;
}

uint32_t Login::RegStatusCallback(im::FUNC_StatusChange func) {
  return uint32_t();
}

bool Login::InitMqtt(std::wstring user_name, std::string user_pwd) {
  mqtt_client_ = std::make_shared<im::CMqttClientBase>();

  im::SMqttConnectInfo connect_info;

  // 配置中获取mqtt服务地址
  connect_info.host = global_config_->GetValA(L"mqtt", L"host");
  connect_info.port = global_config_->GetVal_Int(L"mqtt", L"port");

  // 用户名信息
  connect_info.user_name = base::Utf16ToUtf8(user_name);
  connect_info.user_pwd = user_pwd;

  connect_info.cb_status_change =
    [this]
  (im::EMqttOnlineStatus status) {
    MqttStatus(status);
  };

  if (mqtt_client_->Connect(connect_info) == false) {
    SetLastErr("mqtt connect failed:%s",
               mqtt_client_->GetLastErr_Astd().c_str());
    return false;
  }

  PrintLogInfo("Wait Mqtt Connected");
  return true;
}

void Login::MqttStatus(im::EMqttOnlineStatus status) {
  if (status == im::EMqttOnlineStatus::connected) {
    // 直接订阅登陆通道...
    SubLoginChannel();
  }

  // 返回订阅队列
}

void Login::SubLoginChannel() {
  // 生成UUID,用于监听...
  auto local_id = base::_uuid::GenerateUUID<char>();
  mqtt_client_->Subscribe(
    local_id,
    std::bind(&Login::SubLoginChannelFinished, this),
    std::bind(&Login::LoginMsg, this, std::placeholders::_1));
}

void Login::SubLoginChannelFinished() {
  // 返回登陆成功的回调...
}

void Login::LoginMsg(std::vector<char> data) {}

#pragma region namespace
}
}
#pragma endregion
