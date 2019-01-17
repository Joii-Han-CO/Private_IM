#include "pch.h"
#include "server_login.h"
#include "management.h"


#pragma region namespace
namespace server {
#pragma endregion

ServerLogin::ServerLogin() {}

ServerLogin::~ServerLogin() {}

#pragma region mqtt_management

bool ServerLogin::Init(Func_AsyncResult func) {
  CManagement *m = CManagement::Get();
  auto cfg = m->GetGlobalConfig();

  std::cout << "Begin connect mqtt server" << std::endl;
  PrintLogInfo("begin connect mqtt server");

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

  // 直接订阅消息
  auto sub_login_channel_res = [this](bool suc) {
    if (init_async_res_)
      init_async_res_(true);
    std::cout << "Subscribe public channel success" << std::endl;
    PrintLogInfo("Subscribe public channel success");
  };

  std::cout << "Begin subscribe public channel" << std::endl;
  PrintLogInfo("begin subscribe public channel");

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

void ServerLogin::MqttMsg_Login(const MsgBuf& buf) {
  auto msg = im::msg_proto::Parse_LoginChannel(buf);
  if (msg == nullptr) {
    return;
  }

  switch (msg->type) {
  case im::msg_proto::ELoginMsgType::UserLogin:
    return Msg_UserLogin(
      std::static_pointer_cast<im::msg_proto::Msg_UserLogin>(msg));
  default:
    break;
  }

}

void ServerLogin::MqttLog(const base::log::SBaseLog & l) {
  im::log::CLog::Get()->Print(l.type, "mqtt", l.file, l.func, l.line, l.log);
}

#pragma endregion

#pragma region UserLogin

void ServerLogin::Msg_UserLogin(im::msg_proto::pMsg_UserLogin msg) {
  auto user_ptr = std::make_shared<ServerUserLogin>(
    msg->user_name, msg->login_channel, mqtt_);

  if (user_ptr->Init() == false) {
    PrintLogWarn("receive user login notify, but can't init");
    return;
  }
  users_list_.push_back(user_ptr);
}

#pragma endregion

#pragma region namespace
}
#pragma endregion
