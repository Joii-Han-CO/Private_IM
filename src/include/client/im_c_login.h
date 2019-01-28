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
  ~ClientLogin();
public:
  bool Init(std::wstring user_name, std::wstring user_pwd,
            Func_AsyncResult finished);

  bool Uninit(Func_AsyncResult finished);

  // 获取mqtt句柄
  im::pCMqttClient GetMqtt();

  // 注册一个mqtt状态变更的回调
  uint32_t RegMqttConnectedStatusChanged(FUNC_StatusChange func);
  void UnregMqttConnectedStatusChanged(uint32_t count);

private:
  bool InitMqtt(std::wstring user_name,
                std::wstring user_pwd);

  void InitFinished(bool suc);
  void UninitFinished(bool suc);

  // Mqtt 相关
private:
  // mqtt--日志返回
  void MqttLog(const base::log::SBaseLog &l);

  // mqtt--接到消息后处理
  void MqttConnectedStatusChanged(im::EMqttOnlineStatus status);

  void MqttConnectError();

  // 登陆--订阅公共通道消息
  void MqttSubPublicChannel();

  // 登陆--登陆消息通道
  void MqttLoginChannel(const MsgBuf &buf);

  // 登陆--发送登陆信息
  void MqttSendLoginInfo();

  // 登出--发送消息
  void MqttSendLogoutInfo();

  im::pCMqttClient mqtt_;
  std::map<uint32_t, im::FUNC_StatusChange> mqtt_status_changed_func_;
  uint32_t mqtt_status_changed_func_count_ = 0;
  std::string channel_name_login_;

  bool is_init_ = false;
  Func_AsyncResult init_finished_callback_;
  Func_AsyncResult uninit_finished_callback_;

  std::wstring user_name_;

};
StdSharedPtr_Typedef(ClientLogin);

#pragma region namespace
}
}
#pragma endregion
