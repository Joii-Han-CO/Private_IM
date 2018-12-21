#include "stdafx.h"
#include <mosquitto.h>
#include "mqtt_client_base.h"
#include "im_log.h"

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
  base::log::Log(std::bind(&CMqttClientBase::OutputLog,
                           this, std::placeholders::_1)) {}

CMqttClientBase::CMqttClientBase(base::log::LogCallback func) :
  base::log::Log(func) {}

CMqttClientBase::~CMqttClientBase() {
  WaitTask();
}

// 连接
bool CMqttClientBase::Connect(const SMqttConnectInfo &info) {
  cb_status_change_ = info.cb_status_change;

  StartTask();
  AddTask(
    std::bind(&CMqttClientBase::Mqtt_Connect, this, info));
  return true;
}

// 断开
void CMqttClientBase::Disconnect() {
  Mqtt_StatusChange(EMqttOnlineStatus::disconnecting);
  sync_disconnect_flag_.Set(true);
}

// 添加一个订阅
bool CMqttClientBase::Subscribe(
  const std::string &topic,
  const std::function<void()> &func_sub,
  const std::function<void(std::vector<char>)> &func_msg) {

  // 该函数挂上同步锁
  std::unique_lock<std::mutex> sub_lock(sync_subscribe_);
  if (map_msg_.find(topic) != map_msg_.end()) {
    SetLastErrAndLog("[Mqtt] subscribe %s failed, duplicate", topic.c_str());
    return false;
  }
  map_msg_.insert(
    std::pair<std::string, std::function<void(const std::vector<char>&)>>(
      topic, func_msg));
  int count_id = 0;

  auto ref = mosquitto_subscribe(mqtt_, &count_id, topic.c_str(), 1);
  map_sub_.insert(
    std::pair<int, std::function<void()>>(
      count_id, func_sub));
  sync_sub_callback_.Notify();

  if (ref != MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("[Mqtt] subscribe %s failed, code: %d",
                     topic.c_str(), ref);
    return false;
  }
  return true;
}

bool CMqttClientBase::Unsubscribe(const std::string & topic) {
  auto ref = mosquitto_unsubscribe(mqtt_, nullptr, topic.c_str());
  if (ref != MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("[Mqtt] unsubscribe %s failed, code: %d",
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

  Mqtt_StatusChange(EMqttOnlineStatus::connecting);

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
  Mqtt_StatusChange(EMqttOnlineStatus::connected);

  loop_timeout_ = info.loop_timeout;

  // 下个消息循环，进入循环
  AddTask(
    std::bind(&CMqttClientBase::Mqtt_MsgLoop, this));
}

// mqtt断开连接
void CMqttClientBase::Mqtt_Disconnect() {
  if (mqtt_ == nullptr) {
    MPrintWarn("[Mqtt] not connected, mqtt_ == nullptr");
    return;
  }

  mosquitto_destroy(mqtt_);
  mqtt_ = nullptr;
  Mqtt_StatusChange(EMqttOnlineStatus::disconnected);
  is_connected = false;

  StopTask();
}

// mqtt连接前，设置一些相关参数
bool CMqttClientBase::Mqtt_InitOpts(const SMqttConnectInfo &info) {
  if (info.max_inflight_msg != -1 &&
      mosquitto_max_inflight_messages_set(mqtt_, info.max_inflight_msg) !=
      MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("[Mqtt] set max in flight message failed");
    return false;
  }
  if (info.user_name.empty() == false && info.user_pwd.empty() == false) {
    if (mosquitto_username_pw_set(mqtt_,
                                  info.user_name.c_str(),
                                  info.user_pwd.c_str()) != MOSQ_ERR_SUCCESS) {
      SetLastErrAndLog("[Mqtt] set user and possword failed");
      return false;
    }
  }

  mosquitto_subscribe_callback_set(mqtt_, SSub_Cb);
  mosquitto_message_callback_set(mqtt_, SMsg_Cb);
  mosquitto_publish_callback_set(mqtt_, SPub_Cb);

  return true;
}

// 消息循环
void CMqttClientBase::Mqtt_MsgLoop() {
  int i_ref = 0;
  while (true) {
    if (sync_disconnect_flag_.Get()) {
      Mqtt_Disconnect();
      MPrintInfo("[mqtt]--exit loop");
      break;
    }
    i_ref = mosquitto_loop(mqtt_, loop_timeout_, 1);
    if (i_ref != MOSQ_ERR_SUCCESS) {
      MPrintErro("mosquitto_loop is failed");
      break;
    }
  }
}

void CMqttClientBase::Mqtt_StatusChange(EMqttOnlineStatus status) {
  if (cb_status_change_)
    cb_status_change_(status);
}

#pragma region Callback

// 订阅成功的回调
void CMqttClientBase::SSub_Cb(mosquitto *mosq, void * obj, int mid,
                              int qos_count, const int * granted_qos) {
  if (obj == nullptr)
    return;
  CMqttClientBase *this_ = (CMqttClientBase*)obj;
  if (mosq == nullptr || mosq != this_->mqtt_)
    return;
  this_->Sub_Cb(mid, qos_count, granted_qos);
}
void CMqttClientBase::Sub_Cb(int mid,
                             int qos_count, const int * granted_qos) {
  sync_sub_callback_.Wait();
  auto it = map_sub_.find(mid);
  if (it == map_sub_.end())
    return;
  if (it->second)
    it->second();
  map_sub_.erase(it);
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
    MPrintWarn("[Mqtt] message failed, message is null");
    return;
  }
  if (message->topic == nullptr || message->topic[0] == '\0') {
    MPrintWarn("[Mqtt] message failed, topic is null");
    return;
  }
  if (message->payloadlen <= 0) {
    MPrintWarn("[Mqtt] message failed, data size is 0");
    return;
  }
  if (message->payload == nullptr) {
    MPrintWarn("[Mqtt] message failed, payload is null");
    return;
  }

  std::string topic = message->topic;
  std::vector<char> data;
  data.resize(message->payloadlen);
  memcpy(data.data(), message->payload, data.size());

  auto it = map_msg_.find(topic);
  if (it == map_msg_.end()) {
    MPrintWarn(
      "[Mqtt] message failed, No corresponding callback found");
    return;
  }
  if (it->second)
    it->second(data);
}

// 消息发送的回调
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
    MPrintWarn("[Mqtt] publish failed, No corresponding callback found");
    return;
  }
  if (it->second)
    it->second();
  map_pub_.erase(data);
}

void CMqttClientBase::OutputLog(const base::log::SBaseLog & l) {
  im::log::CLog::Get()->Print(l.type, PRJ_NAME, l.file, l.func, l.line, l.log);
}

#pragma endregion

#pragma region
}
#pragma endregion
