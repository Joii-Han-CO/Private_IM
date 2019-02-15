#include "pch.h"
#include "im_msg.h"

#pragma region namespace
namespace im {
namespace msg {
#pragma endregion

#pragma region CPubChannel_Server

CPubChannel_Server::CPubChannel_Server(im::pCMqttClient mqtt,
                                       std::string pub_name):
  CBaseMsgRecv(mqtt, pub_name) {}

CPubChannel_Server::~CPubChannel_Server() {}

bool CPubChannel_Server::Init(Func_AsyncResult func,
                              std::function<void(cMsgBuf)> recv_func) {
  return CBaseMsgRecv::Sub(func, recv_func);
}

#pragma endregion

#pragma region CPubChannel_Client

CPubChannel_Client::CPubChannel_Client(im::pCMqttClient mqtt,
                                       std::string pub_name):
  CBaseMsgSend(mqtt, pub_name) {}

CPubChannel_Client::~CPubChannel_Client() {}

void CPubChannel_Client::Send_PrivateInfo(
  CPubChannel_Client::PriInfo *args) {
  assert(args != nullptr);
}

#pragma endregion

CPriChannel::CPriChannel() {}

CPriChannel::~CPriChannel() {}

bool CPriChannel::Init(SInitArgs *args, Func_AsyncResult func) {
  // 监听自己的通道
  init_finished_ = func;
  if (args->pub_name.empty()) {
    PrintLogWarn("Mqtt msg channel name is empty");
    return false;
  }

  client_ = args->client;
  std::wstring send_name, recv_name;
  if (client_) {
    send_name = args->pub_name + L"_A";
    recv_name = args->pub_name + L"_B";
  }
  else {
    send_name = args->pub_name + L"_B";
    recv_name = args->pub_name + L"_A";
  }

  //CBaseMsg::InitBaseMsg(args->mqtt, send_name, recv_name, args->func_recv);

  // 监听通道
  auto sub_cb = [this](bool suc) {};
  /*if (Sub(sub_cb) == false)
    return false;*/

  // client will send test channel
  if (client_) {
    if (SendTestMsg() == false)
      return false;
  }

  return true;
}

bool CPriChannel::SendTestMsg() {
  im::msg_proto::Msg_Pub_TestChannel proto;
  proto.status = 1;
  auto buf = proto.Serializate();

  auto func = [this](bool suc) {};
  //return SendMsg(buf, func);
  return false;
}

#pragma region namespace
}
}
#pragma endregion
