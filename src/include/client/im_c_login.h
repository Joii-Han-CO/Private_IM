#pragma once
#include "base/type_def.h"
#include "lib/im_mqtt.h"
#include "lib/im_log.h"


#pragma region namespace
namespace im {
namespace c_login {
#pragma endregion

class ClientLogin {
public:
  ClientLogin();

public:
  bool Init();

  // 获取mqtt句柄
  im::pCMqttClientBase GetMqtt();

  // 注册一个mqtt状态变更的回调
  uint32_t RegMqttConnectedStatusChanged(FUNC_StatusChange func);
  void UnregMqttConnectedStatusChanged(uint32_t count);

private:
  bool InitMqtt();

  // Mqtt 相关
private:
  void MqttLog(const base::log::SBaseLog &l);

  void MqttConnectedStatusChanged(im::EMqttOnlineStatus status);

  im::pCMqttClientBase mqtt_;
  std::map<uint32_t, im::FUNC_StatusChange> mqtt_status_changed_func_;
  uint32_t mqtt_status_changed_func_count_ = 0;
};

#pragma region namespace
}
}
#pragma endregion
