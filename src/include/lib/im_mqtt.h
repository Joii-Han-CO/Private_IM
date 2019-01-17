#pragma once
#include <memory>
#include "base/log.hpp"
#include "base/error.hpp"
#include "base/task.hpp"
#include "base/async.hpp"

#pragma region define

struct mosquitto;
struct mosquitto_message;

#pragma endregion


#pragma region
namespace im {
#pragma endregion

// mqtt 各种连接状态...
enum class EMqttOnlineStatus {
  none,   // 最初状态
  connecting, // 正在连接
  connected,  // 连接成功
  disconnecting, // 正在断开
  disconnected,   // 已断开
};

typedef std::function<void(EMqttOnlineStatus status)> FUNC_StatusChange;

struct SMqttConnectInfo {
  std::string host;
  int port = 1883;

  // utf8 编码
  std::string user_name;
  std::string user_pwd;

  int keepalive = 30;
  int max_inflight_msg = -1;  // 如果离线，最大消息数。默认-1（mosquitto 默认为20）
  int loop_timeout = 0;   // 每次消息循环的超时时间

  FUNC_StatusChange cb_status_change;
};

enum class EMqttQos {
  MostOne = 0,
  LeastOne = 1,
  OnlyOne = 2,
};

class CMqttClientBase:
  public base::error::LastError,
  public base::log::Log,
  public virtual base::b_async::Task {

public:
  CMqttClientBase();
  CMqttClientBase(base::log::LogCallback func);
  virtual ~CMqttClientBase();

  // 描述：连接\断开
  //  对外接口，异步执行
  bool Connect(const SMqttConnectInfo &info);
  void Disconnect();

  // 描述：添加一个订阅
  bool Subscribe(const std::string &topic,
                 const Func_AsyncResult &func_sub,
                 const std::function<void(const MsgBuf&)> &func_msg);

  // 描述：取消一个订阅
  bool Unsubscribe(const std::string &topic);

  // 描述：推送消息
  bool Publish(const std::string &topic,
               const MsgBuf &data,
               const Func_AsyncResult &func);

  static std::string FormatOnlineStatusA(EMqttOnlineStatus s);

private:
  // mqtt同步连接
  void Mqtt_Connect(const SMqttConnectInfo &info);

  // mqtt断开连接
  void Mqtt_Disconnect();

  // mqtt连接前，设置一些相关参数
  bool Mqtt_InitOpts(const SMqttConnectInfo &info);

  void Mqtt_MsgLoop();

  // mqtt状态变更
  void Mqtt_StatusChange(EMqttOnlineStatus status);

  static void SSub_Cb(mosquitto *, void *obj, int mid,
                      int qos_count, const int *granted_qos);
  void Sub_Cb(int mid, int qos_count, const int *granted_qos);

  static void SMsg_Cb(mosquitto *mosq, void *obj,
                      const mosquitto_message *message);
  void Msg_Cb(const mosquitto_message *message);

  static void SPub_Cb(mosquitto *mosq, void *obj, int data);
  void Pub_Cb(int data);

  // 日志输出到日志里
  void OutputLog(const base::log::SBaseLog &l);

private:
  FUNC_StatusChange cb_status_change_ = nullptr;

  mosquitto * mqtt_;
  bool is_connected = false;
  int loop_timeout_ = 0;
  base::async::SyncVal<bool> sync_disconnect_flag_ = false;

  // 订阅成功的列表，用于回调
  std::map<int, Func_AsyncResult> map_sub_;
  std::mutex sync_subscribe_;
  base::async::Event sync_sub_callback_;

  // 订阅对应主题下发的消息
  std::map<std::string, std::function<void(const MsgBuf&)>> map_msg_;

  // 发送消息列表，用于回调
  std::map<int, Func_AsyncResult> map_pub_;
};
typedef std::shared_ptr<CMqttClientBase> pCMqttClientBase;

#pragma region
}
#pragma endregion
