#pragma once
#include <memory>
#include <mosquitto.h>
#include "base/log.hpp"
#include "base/error.hpp"
#include "base/task.hpp"

#pragma region
namespace im {
#pragma endregion

struct SMqttConnectInfo {
  std::string host;
  int port = 1883;
  int keepalive = 30;
  int max_inflight_msg = -1;  // 如果离线，最大消息数。默认-1（mosquitto 默认为20）

};

class CMqttClientBase:
  public base::error::LastError,
  public base::log::Log,
  public base::task::Task {

public:
  CMqttClientBase();
  ~CMqttClientBase();

  // 描述：连接Mqtt服务器
  bool Connect(const SMqttConnectInfo &info);

  // 描述：断开连接
  bool Disconnect();

  // 描述：添加一个订阅
  bool Subscribe();

  // 描述：推送消息
  bool Publish();

protected:
  virtual void Connected(int result, int flags) = 0;
  virtual void Subscribed() = 0;
  virtual void Published() = 0;
  virtual void Messaged(const mosquitto_message *msg) = 0;
  virtual void OutLog(const base::log::SBaseLog &func) = 0;
  VirtualPrintLogFunc(OutLog);

private:
  // 设置Mqtt一些初始化信息
  bool MqttInitOpts(const SMqttConnectInfo &info);

private:
  static void SConnect_Cb(struct mosquitto *mosq, void *obj,
                          int result, int flags);
  static void SMsg_Cb(struct mosquitto *mosq, void *obj,
                      const struct mosquitto_message *message);
private:
  mosquitto * mqtt_;

  bool is_connected = false;

  // 各种默认常量
};
typedef std::shared_ptr<CMqttClientBase> pCMqttClientBase;

#pragma region
}
#pragma endregion
