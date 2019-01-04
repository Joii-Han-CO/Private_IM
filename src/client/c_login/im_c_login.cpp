#include "pch.h"
#include "im_c_login.h"

#pragma region namespace
namespace im {
namespace c_login {
#pragma endregion

#pragma region Init

ClientLogin::ClientLogin() {}

bool ClientLogin::Init() {
  if (InitMqtt() == false) {
    return false;
  }
  return true;
}

bool ClientLogin::InitMqtt() {
  if (mqtt_ != nullptr) {
    PrintLogErro("mqtt not empty");
    return false;
  }
  mqtt_ = std::make_shared<im::CMqttClientBase>(
    std::bind(&ClientLogin::MqttLog, this, std::placeholders::_1));

  im::SMqttConnectInfo mqtt_info;
  mqtt_info.cb_status_change =
    std::bind(&ClientLogin::MqttConnectedStatusChanged,
              this, std::placeholders::_1);
  mqtt_->Connect(mqtt_info);

  return true;
}

#pragma endregion

#pragma region mqtt

im::pCMqttClientBase ClientLogin::GetMqtt() {
  return mqtt_;
}

// 注册回调函数
uint32_t ClientLogin::RegMqttConnectedStatusChanged(
  FUNC_StatusChange func) {
  auto count = mqtt_status_changed_func_count_++;
  mqtt_status_changed_func_.insert(
    std::pair<uint32_t, im::FUNC_StatusChange>(count, func));
  return count;
}

// 卸载
void ClientLogin::UnregMqttConnectedStatusChanged(uint32_t count) {
  auto it = mqtt_status_changed_func_.find(count);
  if (it != mqtt_status_changed_func_.end())
    mqtt_status_changed_func_.erase(it);
}

// mqtt 日志返回
void ClientLogin::MqttLog(const base::log::SBaseLog &l) {
  im::log::CLog::Get()->Print(l.type, "mqtt", l.file, l.func, l.line, l.log);
}

// 接到消息后处理...
void ClientLogin::MqttConnectedStatusChanged(im::EMqttOnlineStatus status) {
  // 处理...

  for (auto it : mqtt_status_changed_func_)
    if (it.second != nullptr)
      it.second(status);
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
