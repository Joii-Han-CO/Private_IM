#pragma once
#include "base/type_def.h"
#include "im_mqtt.h"
#include "im_msg_proto.h"


// 封装mqtt和proto，用于消息传输的基础通道

namespace im {
namespace msg {

//////////////////////////////////////////////////////////////////////////
// client与server建立的时序
//  server 监听 pub
//  c-->  login info  -->s
//  server 监听 c_pub_uuid
//  client 监听 s_pub_uuid
//////////////////////////////////////////////////////////////////////////

// 基础接收类
class CBaseMsgRecv {
public:
  CBaseMsgRecv(im::pCMqttClient mqtt, std::string recv_name):
    recv_name_(recv_name),
    recv_mqtt_(mqtt) {
    assert(mqtt != nullptr);
    assert(!recv_name.empty());
  };
  virtual ~CBaseMsgRecv() {};

  bool Sub(Func_AsyncResult func,
           im::msg_proto::EChannelType type,
           im::msg_proto::pCProtoManager manager) {
    assert(manager != nullptr);

    auto recv_func = [type, manager](cMsgBuf buf) {
      manager->ParseMsg(type, buf);
    };
    return recv_mqtt_->Subscribe(recv_name_, func, recv_func);
  };

private:
  std::string recv_name_;
  im::pCMqttClient recv_mqtt_;
};
StdSharedPtr_Typedef(CBaseMsgRecv);

// 基础发送类
class CBaseMsgSend {
public:
  CBaseMsgSend(im::pCMqttClient mqtt, std::string send_name):
    send_name_(send_name),
    send_mqtt_(mqtt) {
    assert(mqtt != nullptr);
    assert(!send_name.empty());
  };
  virtual ~CBaseMsgSend() {};

  bool Send(cMsgBuf buf, Func_AsyncResult func) {
    assert(!send_name_.empty());
    assert(send_mqtt_ != nullptr);

    return send_mqtt_->Publish(send_name_, buf, func);
  }

private:
  std::string send_name_;
  im::pCMqttClient send_mqtt_;
};
StdSharedPtr_Typedef(CBaseMsgSend);

// 基础消息双向通道
class CBaseMsg {
public:
  CBaseMsg() {};
  virtual ~CBaseMsg() {};

  bool Init(im::pCMqttClient mqtt,
            std::string recv_name, std::string send_name,
            im::msg_proto::EChannelType type,
            im::msg_proto::pCProtoManager manager,
            Func_AsyncResult func) {
    assert(func != nullptr);
    init_finished_ = func;

    recv_msg_ = std::make_shared<CBaseMsgRecv>(mqtt, recv_name);
    send_msg_ = std::make_shared<CBaseMsgSend>(mqtt, send_name);

    return recv_msg_->Sub(func, type, manager);
  }

  bool Send(cMsgBuf buf, Func_AsyncResult func) {
    assert(send_msg_ != nullptr);
    return send_msg_->Send(buf, func);
  }

private:
  pCBaseMsgRecv recv_msg_;
  pCBaseMsgSend send_msg_;

  Func_AsyncResult init_finished_;
};
StdSharedPtr_Typedef(CBaseMsg);


class CPriChannel: public CBaseMsg {
public:
  CPriChannel();
  virtual ~CPriChannel();

  struct SInitAgrs {
    base::_uuid::BUID *channel_id;
    bool caller;
    im::pCMqttClient mqtt;
    im::msg_proto::pCProtoManager manager;
  };

  bool Init(SInitAgrs *args, Func_AsyncResult func);
};
StdSharedPtr_Typedef(CPriChannel);


}
}
