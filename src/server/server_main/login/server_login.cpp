#include "pch.h"
#include "server_login.h"
#include "management.h"


#pragma region
namespace server {
#pragma endregion

ServerLogin::ServerLogin() {}

ServerLogin::~ServerLogin() {}

bool ServerLogin::Init(Func_AsyncResult func) {
  CManagement *m = CManagement::Get();
  auto cfg = m->GetGlobalConfig();

  im::SMqttConnectInfo mqtt_connect_info;

  mqtt_connect_info.host = cfg->GetValA(L"mqtt_server", L"host");
  mqtt_connect_info.port = cfg->GetVal_Int(L"mqtt_server", L"port");

  mqtt_connect_info.user_name = cfg->GetValA(L"mqtt_server", L"admin_name");
  mqtt_connect_info.user_name = cfg->GetValA(L"mqtt_server", L"admin_pwd");

  mqtt_connect_info.cb_status_change = std::bind(
    &ServerLogin::MqttStatusChange, this, std::placeholders::_1);

  init_async_res_ = func;
  mqtt_ = std::make_shared<im::CMqttClientBase>(
    std::bind(&ServerLogin::MqttLog, this, std::placeholders::_1));
  if (mqtt_->Connect(mqtt_connect_info) == false) {
    std::cout << "connetc mqtt failed" << std::endl;
    PrintLogErro("connect mqtt failed, des:%s",
                 mqtt_->GetLastErr_Astd().c_str());
    return false;
  }

  return true;
}

void ServerLogin::RegMqttStatusChange(im::FUNC_StatusChange func) {
  if (func)
    mqtt_status_func_.push_back(func);
}

void ServerLogin::MqttStatusChange(im::EMqttOnlineStatus status) {
  // 指定订阅操作：
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
  default:
    break;
  }

  for (auto it : mqtt_status_func_) {
    if (it)
      it(status);
  }
}

void ServerLogin::MqttConnected() {

  auto sub_login_channel_res = [this]() {
    if (init_async_res_)
      init_async_res_(true);
  };

  if (mqtt_->Subscribe(im::gv::g_mqtt_pub_sub_, sub_login_channel_res,
                       std::bind(&ServerLogin::MqttMsg_Login,
                                 this, std::placeholders::_1)) == false) {
    std::cout << "mqtt subscribe " <<
      im::gv::g_mqtt_pub_sub_.c_str() << " failed" << std::endl;
    if (init_async_res_)
      init_async_res_(false);
    return;
  }
}

void ServerLogin::MqttMsg_Login(std::vector<char>) {}

void ServerLogin::MqttLog(const base::log::SBaseLog & l) {
  im::log::CLog::Get()->Print(l.type, "mqtt", l.file, l.func, l.line, l.log);
}

#pragma region
}
#pragma endregion
