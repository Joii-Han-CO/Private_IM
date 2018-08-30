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

// 连接
bool CMqttClientBase::Connect(const SMqttConnectInfo &info) {
  StartTask();
  AddTask(
    std::bind(&CMqttClientBase::Mqtt_Connect, this, info));
  return true;
}

// 断开
void CMqttClientBase::Disconnect() {
  OnlineStatusChange(EMqttOnlineStatus::disconnecting);
  if (mqtt_ == nullptr) {
    PrintWarn("[Mqtt] not connected, mqtt_ == nullptr");
    return;
  }

  mosquitto_destroy(mqtt_);
  mqtt_ = nullptr;
  OnlineStatusChange(EMqttOnlineStatus::disconnected);
  is_connected = false;

  StopTask();
}

// 添加一个订阅
bool CMqttClientBase::Subscribe(
  const std::string &topic,
  const std::function<void(std::vector<char>)> &func) {
  if (map_sub_.find(topic) != map_sub_.end()) {
    SetLastErrAndLog("[Mqtt] subscribe %s failed, duplicate", topic.c_str());
    return false;
  }
  map_sub_.insert(
    std::pair<std::string, std::function<void(const std::vector<char>&)>>(
      topic, func));

  auto ref = mosquitto_subscribe(mqtt_, nullptr, topic.c_str(), 1);
  if (ref != MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("[Mqtt] subscribe %s failed, code: %d",
                     topic.c_str(), ref);
    return false;
  }
  return true;
}

// 推送消息
bool CMqttClientBase::Publish(const std::string &topic,
                              const std::vector<char> &data,
                              const std::function<void()> &func) {
  int pub_id = publish_count_id_++;
  map_pub_.insert(std::pair<int, std::function<void()>>(pub_id, func));
  int ref = mosquitto_publish(mqtt_, nullptr, topic.c_str(),
                              data.size(), data.data(),
                              pub_id, true);
  if (ref != MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("[Mqtt] publish %s failed, data_size:%d, code: %d",
                     topic.c_str(), data.size(), ref);
    map_pub_.erase(pub_id);
    return false;
  }
  return true;
}

// 格式化在线状态
std::string CMqttClientBase::FormatOnlineStatusA(EMqttOnlineStatus s) {
  switch (s) {
  case im::EMqttOnlineStatus::none:
    return "none";
  case im::EMqttOnlineStatus::connecting:
    return "connecting";
  case im::EMqttOnlineStatus::connected:
    return "connected";
  case im::EMqttOnlineStatus::disconnecting:
    return "disconnecting";
  case im::EMqttOnlineStatus::disconnected:
    return "disconnected";
  default:
    break;
  }
  return "unknow";
}

// mqtt同步连接
void CMqttClientBase::Mqtt_Connect(const SMqttConnectInfo &info) {
  if (is_connected == true) {
    SetLastErrAndLog("[Mqtt] is connected");
    return;
  }

  OnlineStatusChange(EMqttOnlineStatus::connecting);

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
  OnlineStatusChange(EMqttOnlineStatus::connected);

  loop_timeout_ = info.loop_timeout;

  // 下个消息循环，进入循环
  AddTask(
    std::bind(&CMqttClientBase::Mqtt_MsgLoop, this));
}

// mqtt连接前，设置一些相关参数
bool CMqttClientBase::Mqtt_InitOpts(const SMqttConnectInfo &info) {
  if (info.max_inflight_msg != -1 &&
      mosquitto_max_inflight_messages_set(mqtt_, info.max_inflight_msg) !=
      MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("[Mqtt] set max in flight message failed");
    return false;
  }

  mosquitto_message_callback_set(mqtt_, SMsg_Cb);
  mosquitto_publish_callback_set(mqtt_, SPub_Cb);

  return true;
}

// 消息循环
void CMqttClientBase::Mqtt_MsgLoop() {
  while (true) {
    int ref = mosquitto_loop(mqtt_, loop_timeout_, 1);
    if (ref == 0) {

    }
  }
}

// 消息循环的静态回调
void CMqttClientBase::SMsg_Cb(mosquitto *mosq, void *obj,
                              const mosquitto_message *message) {
  if (obj == nullptr)
    return;
  CMqttClientBase *this_ = (CMqttClientBase*)obj;
  if (mosq == nullptr || mosq != this_->mqtt_)
    return;
  this_->Msg_Cb(message);
}
void CMqttClientBase::Msg_Cb(const mosquitto_message *message) {
  if (message == nullptr) {
    PrintWarn("[Mqtt] message failed, message is null");
    return;
  }
  if (message->topic == nullptr || message->topic[0] == '\0') {
    PrintWarn("[Mqtt] message failed, topic is null");
    return;
  }
  if (message->payloadlen <= 0) {
    PrintWarn("[Mqtt] message failed, data size is 0");
    return;
  }
  if (message->payload == nullptr) {
    PrintWarn("[Mqtt] message failed, payload is null");
    return;
  }

  std::string topic = message->topic;
  std::vector<char> data;
  data.resize(message->payloadlen);
  memcpy(data.data(), message->payload, data.size());

  auto it = map_sub_.find(topic);
  if (it == map_sub_.end()) {
    PrintWarn(
      "[Mqtt] message failed, No corresponding callback found");
    return;
  }
  if (it->second)
    it->second(data);
}

void CMqttClientBase::SPub_Cb(mosquitto *mosq, void *obj, int data) {
  if (obj == nullptr)
    return;
  CMqttClientBase *this_ = (CMqttClientBase*)obj;
  if (mosq == nullptr || mosq != this_->mqtt_)
    return;
  this_->Pub_Cb(data);
}
void CMqttClientBase::Pub_Cb(int data) {
  auto it = map_pub_.find(data);
  if (it == map_pub_.end()) {
    PrintWarn("[Mqtt] publish failed, No corresponding callback found");
    return;
  }
  if (it->second)
    it->second();
  map_pub_.erase(data);
}

#pragma region
}
#pragma endregion
