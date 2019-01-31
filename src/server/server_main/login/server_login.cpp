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

  PrintLogInfo("Begin connect mqtt server");

  im::SMqttConnectInfo mqtt_connect_info;

  mqtt_connect_info.host = cfg->GetValA(L"mqtt_server", L"host");
  mqtt_connect_info.port = cfg->GetVal_Int(L"mqtt_server", L"port");

  mqtt_connect_info.user_name = cfg->GetValA(L"mqtt_server", L"admin_name");
  mqtt_connect_info.user_name = cfg->GetValA(L"mqtt_server", L"admin_pwd");

  mqtt_connect_info.cb_status_change = std::bind(
    &ServerLogin::MqttStatusChange, this, std::placeholders::_1);

  mqtt_connect_info.client_id = im::gv::g_ser_mqtt_id;

  init_async_res_ = func;
  mqtt_ = std::make_shared<im::CMqttClient>(
    std::bind(&ServerLogin::MqttLog, this, std::placeholders::_1));
  if (mqtt_->Connect(mqtt_connect_info) == false) {
    PrintLogErro("Connect mqtt failed, des:%s",
                 mqtt_->GetLastErr_Astd().c_str());
    return false;
  }

  return true;
}

void ServerLogin::Uninit() {
  if (mqtt_) {
    mqtt_->Disconnect();
  }
}

void ServerLogin::RegMqttStatusChange(im::FUNC_StatusChange func) {
  if (func)
    mqtt_status_func_.push_back(func);
}

void ServerLogin::Run() {
  // 此处制作一个循环，全部的消息都在此处处理...
  while (!run_break_) {
    std::function<void(void)> func;
    {
      std::unique_lock<std::mutex> lock(run_task_sync_);
      while (run_task_list_.empty()) {
        if (run_break_)
          return;
        run_task_con_.wait(lock);
      }
      func = run_task_list_.front();
      run_task_list_.pop_front();
    }
    func();
  }
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
  case im::EMqttOnlineStatus::error:
    MqttConnectError();
    break;
  default:
    break;
  }

  for (auto it : mqtt_status_func_) {
    if (it)
      it(status);
  }
}

// Mqtt连接成功之后
void ServerLogin::MqttConnected() {

  // 直接订阅消息
  auto sub_login_channel_res = [this](bool suc) {
    InitFinished(true);
    PrintLogInfo("Subscribe public channel success");
  };

  PrintLogInfo("Begin subscribe public channel");

  if (mqtt_->Subscribe(im::gv::g_mqtt_pub_sub_, sub_login_channel_res,
                       std::bind(&ServerLogin::MqttMsg_Login,
                                 this, std::placeholders::_1)) == false) {
    PrintLogErro("Mqtt subscribe %s fialed", im::gv::g_mqtt_pub_sub_.c_str());
    InitFinished(false);
    return;
  }
}

void ServerLogin::MqttMsg_Login(const MsgBuf& buf) {
  auto msg = im::msg_proto::MsgBase_Login::Parse(buf);
  if (msg == nullptr) {
    return;
  }

  switch (msg->type_) {
  case im::msg_proto::ELoginMsgType::UserLogin:
    return M_UserLogin(
      std::dynamic_pointer_cast<im::msg_proto::Msg_UserLogin>(msg));
  default:
    break;
  }

}

void ServerLogin::MqttLog(const base::log::SBaseLog & l) {
  im::log::CLog::Get()->Print(l.type, "mqtt", l.file, l.func, l.line, l.log);
}

void ServerLogin::MqttConnectError() {
  InitFinished(false);
}

void ServerLogin::InitFinished(bool suc) {
  if (init_async_res_) {
    init_async_res_(suc);
    init_async_res_ = nullptr;
  }
}

void ServerLogin::AddRunTask(std::function<void()> func) {
  std::unique_lock<std::mutex> lock_break(run_task_lock_break_);
  if (run_break_)
    return;
  std::lock_guard<std::mutex> lock(run_task_sync_);
  run_task_list_.emplace_back(std::move(func));
  run_task_con_.notify_one();
}

#pragma endregion

#pragma region UserLogin

void ServerLogin::M_UserLogin(im::msg_proto::pMsg_UserLogin msg) {
  // 线程池中操作
  CManagement::Get()->GetPool()->Commit([msg, this]() {

    auto user_id = user_id_count_++;

    auto user_ptr = std::make_shared<ServerUserLogin>(
      user_id, msg->user_name, msg->login_channel, mqtt_,
      std::bind(&ServerLogin::M_UserLogout, this, std::placeholders::_1));

    if (user_ptr->Init() == false) {
      PrintLogWarn("Receive user login notify, but can't init");
      return;
    }
    users_.insert(std::pair<uint32_t, pServerUserLogin>(user_id, user_ptr));
  });
}

void ServerLogin::M_UserLogout(uint32_t user_id) {
  AddRunTask([this]() {
    // 移除该用户

  });
}

#pragma endregion

#pragma region namespace
}
#pragma endregion
