#include "pch.h"
#include "im_mqtt.h"
#include "im_log.h"

namespace im {

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

CMqttClient::CMqttClient():
  base::log::Log(std::bind(&CMqttClient::OutputLog,
                           this, std::placeholders::_1)) {}

CMqttClient::CMqttClient(base::log::LogCallback func) :
  base::log::Log(func) {}

CMqttClient::~CMqttClient() {
  WaitTask();
}

// 连接
bool CMqttClient::Connect(const SMqttConnectInfo &info) {
  AddConnectStatusFunc(info.cb_status_change);

  StartTask();
  AddTask(
    std::bind(&CMqttClient::Mqtt_Connect, this, info));
  return true;
}

// 断开
void CMqttClient::Disconnect() {
  if (cur_status_ == EMqttOnlineStatus::error ||
      cur_status_ == EMqttOnlineStatus::connecting ||
      cur_status_ == EMqttOnlineStatus::none) {
    StopTask();
  }
  Mqtt_StatusChange(EMqttOnlineStatus::disconnecting);
  sync_disconnect_flag_.Set(true);
}

// 添加一个订阅
bool CMqttClient::Subscribe(
  const std::string &topic,
  const Func_AsyncResult &func_sub,
  const std::function<void(const MsgBuf&)> &func_msg,
  EMqttQos qos) {

  // 该函数挂上同步锁
  std::unique_lock<std::mutex> sub_lock(sync_subscribe_);
  if (map_msg_.find(topic) != map_msg_.end()) {
    SetLastErrAndLog("subscribe %s failed, duplicate", topic.c_str());
    return false;
  }
  map_msg_.insert(
    std::pair<std::string, std::function<void(const MsgBuf&)>>(
      topic, func_msg));
  int count_id = 0;

  auto ref = mosquitto_subscribe(mqtt_, &count_id, topic.c_str(),
                                 GetQosVal(qos));
  map_sub_.insert(
    std::pair<int, Func_AsyncResult>(
      count_id, func_sub));
  sync_sub_callback_.Notify();

  if (ref != MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("subscribe %s failed, code: %d",
                     topic.c_str(), ref);
    return false;
  }
  return true;
}

bool CMqttClient::Unsubscribe(const std::string & topic) {
  auto ref = mosquitto_unsubscribe(mqtt_, nullptr, topic.c_str());
  if (ref != MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("unsubscribe %s failed, code: %d",
                     topic.c_str(), ref);
    return false;
  }
  return true;
}

// 推送消息
bool CMqttClient::Publish(const std::string &topic,
                          cMsgBuf data,
                          const Func_AsyncResult &func,
                          EMqttQos qos) {
  int pub_id = -1;
  int ref = mosquitto_publish(mqtt_, &pub_id, topic.c_str(),
                              data.size(), data.data(),
                              (int)qos, false);
  // 需要同步锁处理异步问题
  if (ref != MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("publish %s failed, data_size:%d, code: %d",
                     topic.c_str(), data.size(), ref);
    map_pub_.erase(pub_id);
    return false;
  }
  if (map_pub_.find(pub_id) != map_pub_.end()) {
    PrintLogWarn("publish warning, ref mid is repeat");
    if (func)
      func(false);
    return true;
  }
  else {
    map_pub_.insert(std::pair<int, Func_AsyncResult>(pub_id, func));
    return true;
  }
}

// 格式化在线状态
std::string CMqttClient::FormatOnlineStatusA(EMqttOnlineStatus s) {
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

int CMqttClient::GetQosVal(EMqttQos q) {
  return (int)q;
}

uint32_t CMqttClient::AddConnectStatusFunc(FUNC_StatusChange func) {
  if (func == nullptr)
    return 0;
  std::unique_lock<std::mutex> lock(cb_status_change_sync_);

  uint32_t c = ++cb_status_change_count_;

  cb_status_changs_.push_back(
    std::pair<uint32_t, im::FUNC_StatusChange>(c, func));

  return c;
}

void CMqttClient::DelConnectStatusFunc(uint32_t cb_id) {
  std::unique_lock<std::mutex> lock(cb_status_change_sync_);

  for (auto it = cb_status_changs_.begin();
       it != cb_status_changs_.end(); it++) {
    if (it->first == cb_id) {
      cb_status_changs_.erase(it);
      return;
    }
  }
}

// mqtt同步连接
void CMqttClient::Mqtt_Connect(const SMqttConnectInfo &info) {
  if (is_connected == true) {
    SetLastErrAndLog("Is connected");
    Mqtt_StatusChange(EMqttOnlineStatus::error);
    return;
  }

  Mqtt_StatusChange(EMqttOnlineStatus::connecting);

  auto ref = GlobalInitMqtt::Get()->Init();
  if (ref != MOSQ_ERR_SUCCESS) {
    SetLastErrAndLog("Global init failed, code: %d", ref);
    Mqtt_StatusChange(EMqttOnlineStatus::error);
    return;
  }

  const char *id = nullptr;
  bool client_session = true;
  if (!info.client_id.empty()) {
    id = info.client_id.c_str();
    //client_session = false;
  }

  // 创建mosquitto实例
  mqtt_ = mosquitto_new(id, client_session, this);
  if (mqtt_ == nullptr || mqtt_ == (void*)EINVAL) {
    SetLastErrAndLog("New mosquitto failed");
    Mqtt_StatusChange(EMqttOnlineStatus::error);
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
    SetLastErrAndLog(L"Connect failed, code: %d",
                     base::log::GetMqttErrorDesW().c_str());
    StopTask();
    Mqtt_StatusChange(EMqttOnlineStatus::error);
    return;
  }
  is_connected = true;
  Mqtt_StatusChange(EMqttOnlineStatus::connected);

  loop_timeout_ = info.loop_timeout;

  // 下个消息循环，进入循环
  AddTask([this]() {
    Mqtt_MsgLoop();
    StopTask();
  });
}

// mqtt断开连接
void CMqttClient::Mqtt_Disconnect() {
  if (mqtt_ == nullptr) {
    MPrintWarn("Not connected, mqtt_ == nullptr");
    Mqtt_StatusChange(EMqttOnlineStatus::error);
    return;
  }

  mosquitto_destroy(mqtt_);
  mqtt_ = nullptr;
  Mqtt_StatusChange(EMqttOnlineStatus::disconnected);
  is_connected = false;

  StopTask();
}

// mqtt连接前，设置一些相关参数
bool CMqttClient::Mqtt_InitOpts(const SMqttConnectInfo &info) {
  if (info.max_inflight_msg != -1) {
    if (
      mosquitto_max_inflight_messages_set(mqtt_, info.max_inflight_msg) !=
      MOSQ_ERR_SUCCESS) {
      SetLastErrAndLog("Set max in flight message failed");
      return false;
    }
  }
  if (info.user_name.empty() == false && info.user_pwd.empty() == false) {
    if (mosquitto_username_pw_set(mqtt_,
                                  info.user_name.c_str(),
                                  info.user_pwd.c_str()) != MOSQ_ERR_SUCCESS) {
      SetLastErrAndLog("Set user and possword failed");
      return false;
    }
  }

  mosquitto_subscribe_callback_set(mqtt_, SSub_Cb);
  mosquitto_message_callback_set(mqtt_, SMsg_Cb);
  mosquitto_publish_callback_set(mqtt_, SPub_Cb);

  return true;
}

// 消息循环
void CMqttClient::Mqtt_MsgLoop() {
  int i_ref = 0;
  while (true) {
    if (sync_disconnect_flag_.Get()) {
      Mqtt_Disconnect();
      MPrintInfo("Exit loop");
      break;
    }
    i_ref = mosquitto_loop(mqtt_, loop_timeout_, 1);
    if (i_ref != MOSQ_ERR_SUCCESS) {
      MPrintErro(L"Mosquitto_loop is failed, des: %s",
                 base::log::GetMqttErrorDesW().c_str());
      break;
    }
  }
}

void CMqttClient::Mqtt_StatusChange(EMqttOnlineStatus status) {
  cur_status_ = status;

  std::unique_lock<std::mutex> lock(cb_status_change_sync_);
  size_t s = cb_status_changs_.size();
  for (size_t i = 0; i < s; i++) {
    if (cb_status_changs_[i].second) {
      cb_status_changs_[i].second(status);
    }
  }
}


// 订阅成功的回调
void CMqttClient::SSub_Cb(mosquitto *mosq, void * obj, int mid,
                          int qos_count, const int * granted_qos) {
  if (obj == nullptr)
    return;
  CMqttClient *this_ = (CMqttClient*)obj;
  if (mosq == nullptr || mosq != this_->mqtt_)
    return;
  this_->Sub_Cb(mid, qos_count, granted_qos);
}
void CMqttClient::Sub_Cb(int mid,
                         int qos_count, const int * granted_qos) {
  sync_sub_callback_.Wait();
  auto it = map_sub_.find(mid);
  if (it == map_sub_.end())
    return;
  if (it->second)
    it->second(true);
  map_sub_.erase(it);
}

// 消息循环的静态回调
void CMqttClient::SMsg_Cb(mosquitto *mosq, void *obj,
                          const mosquitto_message *message) {
  if (obj == nullptr)
    return;
  CMqttClient *this_ = (CMqttClient*)obj;
  if (mosq == nullptr || mosq != this_->mqtt_)
    return;
  this_->Msg_Cb(message);
}
void CMqttClient::Msg_Cb(const mosquitto_message *message) {
  if (message == nullptr) {
    MPrintWarn("Message failed, message is null");
    return;
  }
  if (message->topic == nullptr || message->topic[0] == '\0') {
    MPrintWarn("Message failed, topic is null");
    return;
  }
  if (message->payloadlen <= 0) {
    MPrintWarn("Message failed, data size is 0");
    return;
  }
  if (message->payload == nullptr) {
    MPrintWarn("Message failed, payload is null");
    return;
  }

  std::string topic = message->topic;
  MsgBuf data;
  data.resize(message->payloadlen);
  memcpy(data.data(), message->payload, data.size());

  auto it = map_msg_.find(topic);
  if (it == map_msg_.end()) {
    MPrintWarn(
      "Message failed, No corresponding callback found");
    return;
  }
  if (it->second)
    it->second(data);
}

// 消息发送的回调
void CMqttClient::SPub_Cb(mosquitto *mosq, void *obj, int mid) {
  if (obj == nullptr)
    return;
  CMqttClient *this_ = (CMqttClient*)obj;
  if (mosq == nullptr || mosq != this_->mqtt_)
    return;
  this_->Pub_Cb(mid);
}
void CMqttClient::Pub_Cb(int mid) {
  auto it = map_pub_.find(mid);
  if (it == map_pub_.end()) {
    MPrintWarn("Publish failed, No corresponding callback found");
    return;
  }
  if (it->second)
    it->second(true);
  map_pub_.erase(mid);
}

void CMqttClient::OutputLog(const base::log::SBaseLog & l) {
  im::log::CLog::Get()->Print(l.type, PRJ_NAME, l.file, l.func, l.line, l.log);
}

}
