#pragma once
#include "lib/im_mqtt.h"
#include "lib/base/type_def.h"

#include "user_login.h"


#pragma region
namespace server {
#pragma endregion

class ServerLogin {
public:
  ServerLogin();
  ~ServerLogin();

  bool Init(Func_AsyncResult func);

  void Uninit();

  void RegMqttStatusChange(im::FUNC_StatusChange func);

  void Run();

private:
  void MqttStatusChange(im::EMqttOnlineStatus status);

  // mqtt连接成功，准备订阅公共通道
  void MqttConnected();

  // mqtt消息， 登陆
  void MqttMsg_Login(const MsgBuf& buf);

  // 返回并打印mqtt日志
  void MqttLog(const base::log::SBaseLog &l);

  // 各种连接过程中的错误
  void MqttConnectError();

  void InitFinished(bool suc);

  void AddRunTask(std::function<void()> func);

  // 登陆消息处理
  void M_UserLogin(im::msg_proto::pMsg_UserLogin msg);

  // 用户消息登出处理...
  void M_UserLogout(uint32_t user_id);

private:
  Func_AsyncResult init_async_res_ = nullptr;

  bool run_break_ = false;
  std::condition_variable run_task_con_;
  std::mutex run_task_sync_;
  std::list<std::function<void(void)>> run_task_list_;
  std::mutex run_task_lock_break_;

  im::pCMqttClient mqtt_;
  std::vector<im::FUNC_StatusChange> mqtt_status_func_;

  std::map<uint32_t, pServerUserLogin> users_;
  uint32_t user_id_count_ = 0;
};
StdSharedPtr_Typedef(ServerLogin);

#pragma region
}
#pragma endregion
