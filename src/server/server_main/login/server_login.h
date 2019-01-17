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

  void RegMqttStatusChange(im::FUNC_StatusChange func);

private:
  void MqttStatusChange(im::EMqttOnlineStatus status);

  // mqtt连接成功，准备订阅公共通道
  void MqttConnected();

  // mqtt消息， 登陆
  void MqttMsg_Login(const MsgBuf& buf);

  // 返回并打印mqtt日志
  void MqttLog(const base::log::SBaseLog &l);


  // 登陆消息处理
  void Msg_UserLogin(im::msg_proto::pMsg_UserLogin msg);

private:
  im::pCMqttClientBase mqtt_;
  std::vector<im::FUNC_StatusChange> mqtt_status_func_;

  Func_AsyncResult init_async_res_ = nullptr;

  std::list<pServerUserLogin> users_list_;
};

#pragma region
}
#pragma endregion
