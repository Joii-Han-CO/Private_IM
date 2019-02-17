#include "publish_channel.h"

#pragma region
namespace server {
#pragma endregion

CSPubChannel::CSPubChannel() {}

CSPubChannel::~CSPubChannel() {}

CSPubChannel* CSPubChannel::Get() {
  static CSPubChannel s;
  return &s;
}

bool CSPubChannel::Init(im::pCMqttClient mqtt, Func_AsyncResult func) {
  assert(msg_ == nullptr);

  // 在调用监控mqtt之前注册proto回调信息
  RegMsgCallback();

  msg_ = std::make_shared<im::msg::CBaseMsgRecv>(mqtt,
                                                 im::gv::g_mqtt_pub_sub_);
  return msg_->Sub(func,
                   im::msg_proto::EChannelType::golbal_channel,
                   proto_manager_);
}

void CSPubChannel::RegMsgCallback() {
  proto_manager_ = std::make_shared<im::msg_proto::CProtoManager>();

  im::msg_proto::PP_CreatePrivateChannel::RegCallback(
    proto_manager_.get(),
    std::bind(&CSPubChannel::OnMsg_CreatePriChannel,
              this, std::placeholders::_1));
}

// 创建server与client的私有通道
void CSPubChannel::OnMsg_CreatePriChannel(
  im::msg_proto::pPP_CreatePrivateChannel msg
) {}

#pragma region
}
#pragma endregion
