#include "stdafx.h"
#include "mqtt_client_base.h"

#pragma region
namespace im {
#pragma endregion

// base
namespace {
class GlobalInitMqtt {
private:
  GlobalInitMqtt() {};
public:
  ~GlobalInitMqtt() {
    mosquitto_lib_cleanup();
  };

  static GlobalInitMqtt* Get() {
    static GlobalInitMqtt s;
    return &s;
  };

  int Init() {
    if (is_init_)
      return MOSQ_ERR_SUCCESS;

    // 全局的只初始化一次
    auto ref = mosquitto_lib_init();
    if (ref == MOSQ_ERR_SUCCESS)
      is_init_ = true;
    return ref;
  };

private:
  bool is_init_ = false;
};
}

CMqttClientBase::CMqttClientBase():
  base::log::Log(BindVirtualLog(CMqttClientBase)) {}

CMqttClientBase::~CMqttClientBase() {}

// 连接Mqtt服务器
bool CMqttClientBase::Connect(const SMqttConnectInfo &info) {
  StartTask();

  auto func = [this, info] () {
    Mqtt_Connect(info);

  };
  AddTask(func);
  return true;
}

// 断开连接
bool CMqttClientBase::Disconnect() {
  if (mqtt_ == nullptr) {
    PrintWarn("[Mqtt] not connected, mqtt_ == nullptr");
    return true;
  }

  mosquitto_destroy(mqtt_);
  mqtt_ = nullptr;

  return true;
}

// 添加一个订阅
bool CMqttClientBase::Subscribe() {
  return false;
}

// 推送消息
bool CMqttClientBase::Publish() {
  return false;
}

// mqtt同步连接
void CMqttClientBase::Mqtt_Connect(const SMqttConnectInfo &info) {
  auto ref = GlobalInitMqtt::Get()->Init();
  if (ref != MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("[Mqtt] global init failed, code: %d", ref);
    return;
  }

  // 创建mosquitto实例
  mqtt_ = mosquitto_new(nullptr, true, this);
  if (mqtt_ == nullptr || mqtt_ == (void*)EINVAL) {
    SetLastErrAndLog("[Mqtt] new mosquitto failed");
    return;
  }

  // 设置
  if (Mqtt_InitOpts(info) == false) {
    return;
  }

  // 连接...
  ref = mosquitto_connect(mqtt_,
                          info.host.c_str(), info.port,
                          info.keepalive);
  if (ref != MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("[Mqtt] connect failed, code: %d", ref);
    return;
  }
  is_connected = true;
}

// mqtt连接前，设置一些相关参数
bool CMqttClientBase::Mqtt_InitOpts(const SMqttConnectInfo &info) {
  if (info.max_inflight_msg != -1 &&
      mosquitto_max_inflight_messages_set(mqtt_, info.max_inflight_msg) !=
      MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("[Mqtt] set max in flight message failed");
    return false;
  }

  return true;
}

#pragma region
}
#pragma endregion
