#include "pch.h"
#include "im_msg.h"

#pragma region namespace
namespace im {
namespace msg {
#pragma endregion

CMsg::CMsg() {}

CMsg::~CMsg() {}

bool CMsg::Init(SMsg_InitArgs *info) {
  if (info == nullptr || info->channel_name.empty() || info->mqtt == nullptr) {
    PrintLogErro("CMsg init failed, info is nullptr");
    return false;
  }
  init_finished_ = info->init_finished;
  connect_finished_ = info->connect_finished;

  caller_ = info->caller;
  auto channel_name = base::Utf16ToUtf8(info->channel_name);
  mqtt_ = info->mqtt;

  if (caller_) {
    recv_channel_name_ = channel_name + "_C";
    send_channel_name_ = channel_name + "_E";
  }
  else {
    recv_channel_name_ = channel_name + "_E";
    send_channel_name_ = channel_name + "_C";
  }

  // 订阅完成
  auto subref_func = [this](bool suc) {
    InitFinished(suc);
  };

  // 订阅消息
  if (mqtt_->Subscribe(recv_channel_name_, subref_func,
                       std::bind(&CMsg::RecvMsg, this, std::placeholders::_1))
      == false) {
    PrintLogErro("Mqtt subscribe failed, channel name: %s",
                 recv_channel_name_.c_str());
    return false;
  }

  return true;
}

bool CMsg::Uninit() {
  return false;
}

bool CMsg::SendTestMsg() {
  if (caller_ == false) {
    ConnectFinished(true);
    return true;
  }

  return SendTestMsg_(rand());
}

bool CMsg::SendMsg(const MsgBuf &buf, Func_AsyncResult func) {
  return mqtt_->Publish(send_channel_name_, buf, func);
}

void CMsg::InitFinished(bool suc) {
  if (init_finished_)
    init_finished_(suc);
}

void CMsg::ConnectFinished(bool suc) {
  if (connect_finished_)
    connect_finished_(suc);
}

void CMsg::RecvMsg(const MsgBuf &buf) {
  auto msg = im::msg_proto::MsgBase::Parse(buf, msg_proto::EChannelType::none);
  if (msg == nullptr)
    return;

  if (msg->header.type == (int)im::msg_proto::EPubMsgType::test_channel) {
    RecvTestMsg(
      std::dynamic_pointer_cast<im::msg_proto::Msg_TestChannel>(msg));
  }
}

bool CMsg::SendTestMsg_(int rand_num) {
  im::msg_proto::Msg_TestChannel proto;
  proto.status_ = rand_num;
  cur_init_count = rand_num;

  auto func = [this](bool suc) {
    if (suc == false) {
      ConnectFinished(suc);
    }
    if (!caller_ && suc == true)
      ConnectFinished(true);
  };

  return SendMsg(proto.Serializate(), func);
}

void CMsg::RecvTestMsg(im::msg_proto::pMsg_TestChannel msg) {
  if (caller_) {
    ConnectFinished(cur_init_count == msg->status_ - 1);
  }
  else {
    if (SendTestMsg_(msg->status_ + 1) == false)
      ConnectFinished(false);
  }
}

#pragma region namespace
}
}
#pragma endregion
