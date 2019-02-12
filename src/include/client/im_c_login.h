﻿#pragma once
#include "base/type_def.h"
#include "lib/im_mqtt.h"
#include "lib/im_log.h"
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

  // 发送登陆通道的消息
  bool SendMsg(const MsgBuf &buf, Func_AsyncResult func);

  // 登陆业务
private:
  // 登陆--订阅公共通道消息
  void SubChannel();

  // 登陆--登陆消息通道
  void MsgChannel(const MsgBuf &buf);

  void Msg_LoginRes(const im::msg_proto::pMsg_UserLoginSRes msg);

  // 登陆--发送登陆信息
  void SendLoginInfo();

  // 登出--发送消息
  void SendLogoutInfo();

  im::pCMqttClient mqtt_;
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
