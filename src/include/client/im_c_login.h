#pragma once
#include "base/type_def.h"
#include "lib/im_mqtt.h"
#include "lib/im_log.h"
#include "lib/im_msg.h"
#include "lib/im_msg_proto.h"


#pragma region namespace
namespace im {
namespace c_login {
#pragma endregion

class ClientLogin {
public:
  ClientLogin();
  ~ClientLogin();
public:
  bool Init(std::wstring user_name, std::wstring user_pwd,
            Func_AsyncResult finished);

  bool Uninit(Func_AsyncResult finished);

  // 获取mqtt句柄
  im::pCMqttClient GetMqtt();

private:
  void InitFinished(bool suc);
  Func_AsyncResult func_init_finished_;

  void UninitFinished(bool suc);
  Func_AsyncResult func_uninit_finished_;

private:
  void MqttConnectStatus(EMqttOnlineStatus status);

  void MqttConnected();

  void MqttConnectError();

private:
  im::pCMqttClient mqtt_;
  std::wstring user_name_;
  std::wstring channel_name_;

};
StdSharedPtr_Typedef(ClientLogin);

#pragma region namespace
}
}
#pragma endregion
