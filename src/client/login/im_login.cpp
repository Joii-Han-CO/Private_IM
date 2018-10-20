#include "stdafx.h"
#include "im_login.h"
#include "im_global_val.h"
#include "lib/msg_mqtt_client/mqtt_client_base.h"


#pragma region namespace
namespace im {
namespace login {
#pragma endregion

Login::Login() {}

Login::~Login() {}

bool Login::Init(LoginFuncResult func_done) {
  global_config_ = std::make_shared<im::config::CConfig>();
  if (global_config_->Init(im::gv::g_global_config_file_path) == false) {
    SetLastErr(L"Init config file failed, des:%s",
               global_config_->GetLastErr_Wc());
    return false;
  }

  if (InitMqtt() == false)
    return false;

  return true;
}

bool Login::InitMqtt() {
  std::wstring host = global_config_->GetVal(L"mqtt", L"host");

  //mqtt_client_ = std::make_shared<im::CMqttClientBase>();


  return true;
}

bool Login::SetUserName(std::wstring name, std::wstring pwd,
                        LoginFuncResult func_done) {
  return true;
}

#pragma region namespace
}
}
#pragma endregion
