#include "publish_channel.h"
#include "frame/server_frame_work.h"
#include "users/server_user_manager.h"

namespace server {

CSPubChannel::CSPubChannel() {}

CSPubChannel::~CSPubChannel() {}

CSPubChannel* CSPubChannel::Get() {
  static CSPubChannel s;
  return &s;
}

bool CSPubChannel::Init(Func_AsyncResult func) {
  assert(msg_ == nullptr);

  init_finished_ = func;

  // 在调用监控mqtt之前注册proto回调信息
  RegMsgCallback();

  mqtt_ = std::make_shared<im::CMqttClient>();

  auto gc = CSFarmeWork::Get()->GetGlobalConfig();
  im::SMqttConnectInfo info;
  info.host = gc->GetValA(L"mqtt_server", L"host");
  info.port = gc->GetVal_Int(L"mqtt_server", L"port");
  info.user_name = gc->GetValA(L"mqtt_server", L"user_name");
  info.user_pwd = gc->GetValA(L"mqtt_server", L"admin_pwd");
  info.cb_status_change = std::bind(&CSPubChannel::OnStatusChange,
                                    this, std::placeholders::_1);
  return mqtt_->Connect(info);
}

void CSPubChannel::Uninit() {
  if (mqtt_) {
    mqtt_->Disconnect();
    mqtt_ = nullptr;
  }

  if (proto_manager_)
    proto_manager_ = nullptr;

  CSUserManager::Get()->CleanUser();
}

im::pCMqttClient CSPubChannel::GetMqtt() {
  return mqtt_;
}

void CSPubChannel::RegMsgCallback() {
  proto_manager_ = std::make_shared<im::msg_proto::CProtoManager>();

  im::msg_proto::PP_CreatePrivateChannel::RegCallback(
    proto_manager_.get(),
    std::bind(&CSPubChannel::OnMsg_CreatePriChannel,
              this, std::placeholders::_1));
}

void CSPubChannel::OnStatusChange(im::EMqttOnlineStatus status) {
  if (status == im::EMqttOnlineStatus::connected) {
    msg_ = std::make_shared<im::msg::CBaseMsgRecv>(mqtt_,
                                                   im::gv::g_mqtt_pub_sub_);
    auto func = [this](bool suc) {
      InitFinished(suc);
    };
    msg_->Sub(func,
              im::msg_proto::EChannelType::golbal_channel,
              proto_manager_);
  }
  else if (status == im::EMqttOnlineStatus::error) {
    PrintLogErro("mqtt connect failed will exit");
    InitFinished(false);
  }
}

// 创建server与client的私有通道
void CSPubChannel::OnMsg_CreatePriChannel(
  im::msg_proto::pPP_CreatePrivateChannel msg
) {
  assert(msg != nullptr);

  if (CSUserManager::Get()->AddUser(msg) == false) {
    //
  }
}

}
