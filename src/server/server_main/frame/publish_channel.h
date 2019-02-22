#pragma once
#include "lib/im_msg.h"

#pragma region
namespace server {
#pragma endregion

class CSPubChannel: private base::async::AsyncInit {
private:
  CSPubChannel();
public:
  ~CSPubChannel();
  static CSPubChannel* Get();

  bool Init(Func_AsyncResult func);

  void Uninit();

  im::pCMqttClient GetMqtt();
private:
  void RegMsgCallback();

  void OnStatusChange(im::EMqttOnlineStatus status);

  // 创建server与client的私有通道
  void OnMsg_CreatePriChannel(im::msg_proto::pPP_CreatePrivateChannel msg);

private:
  im::msg::pCBaseMsgRecv msg_;
  im::msg_proto::pCProtoManager proto_manager_;

  im::pCMqttClient mqtt_;
};

#pragma region
}
#pragma endregion
