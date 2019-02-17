#pragma once
#include "lib/im_msg.h"

#pragma region
namespace server {
#pragma endregion

class CSPubChannel {
private:
  CSPubChannel();
public:
  ~CSPubChannel();
  static CSPubChannel* Get();

  bool Init(im::pCMqttClient mqtt, Func_AsyncResult func);

private:
  void RegMsgCallback();

  // 创建server与client的私有通道
  void OnMsg_CreatePriChannel(im::msg_proto::pPP_CreatePrivateChannel msg);

private:
  im::msg::pCBaseMsgRecv msg_;
  im::msg_proto::pCProtoManager proto_manager_;

};

#pragma region
}
#pragma endregion
