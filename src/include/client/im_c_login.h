#pragma once
#include "lib/base/async.hpp"
#include "lib/im_mqtt.h"
#include "lib/im_msg.h"

namespace im {
namespace c_login {

class ClientLogin: protected base::async::AsyncInit {
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
  void UninitFinished(bool suc);
  Func_AsyncResult func_uninit_finished_;

private:
  void MqttConnectStatus(EMqttOnlineStatus status);

  void MqttConnected();

  void MqttConnectError();

  // 创建私有通道
  void CreatePriChannel();

  // 创建私有通道成功之后
  void CreatePriChannelFinished(base::_uuid::BUID channel_id);

  // 登陆超时
  void LoginTimeOut();

  void OnMsg_ResLoginStatus(im::msg_proto::pPR_ResLoginStatus msg);

private:
  im::pCMqttClient mqtt_;
  std::wstring user_name_;

  im::msg::pCBaseMsgSend pub_channel_;
  im::msg::pCPriChannel pri_channel_;
  im::msg_proto::pCProtoManager pri_manager_;
};
StdSharedPtr_Typedef(ClientLogin);

}
}
