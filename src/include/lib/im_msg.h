#pragma once
#include "base/type_def.h"
#include "im_mqtt.h"
#include "im_msg_proto.h"


// 封装mqtt和proto，用于消息传输的基础通道

#pragma region namespace
namespace im {
namespace msg {
#pragma endregion

//////////////////////////////////////////////////////////////////////////
// client与server建立的时序
//  server 监听 pub
//  c-->  login info  -->s
//  server 监听 c_pub_uuid
//  client 监听 s_pub_uuid
//////////////////////////////////////////////////////////////////////////

#pragma region Base

// 基础接收类
class CBaseMsgRecv {
public:
  CBaseMsgRecv(im::pCMqttClient mqtt, std::string recv_name):
    recv_mqtt_(mqtt),
    recv_name_(recv_name) {
#ifdef _DEBUG
    assert(mqtt == nullptr);
    assert(!recv_name.empty());
#endif // _DEBUG
  };
  virtual ~CBaseMsgRecv() {};

  bool Sub(Func_AsyncResult func, std::function<void(cMsgBuf)> recv_func) {
#ifdef _DEBUG
    assert(!recv_name_.empty());
    assert(recv_mqtt_ != nullptr);
#endif // _DEBUG
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
    send_mqtt_(mqtt),
    send_name_(send_name) {
#ifdef _DEBUG
    assert(mqtt == nullptr);
    assert(!send_name.empty());
#endif // _DEBUG
  };
  virtual ~CBaseMsgSend() {};

  bool Send(cMsgBuf buf, Func_AsyncResult func) {
#ifdef _DEBUG
    assert(!send_name_.empty());
    assert(send_mqtt_ != nullptr);
#endif // _DEBUG
    return send_mqtt_->Publish(send_name_, buf, func);
  }

private:
  std::string send_name_;
  im::pCMqttClient send_mqtt_;
};
StdSharedPtr_Typedef(CBaseMsgSend);

#pragma endregion

#pragma region pubchannel

// 用户->服务器 单向服务
class CPubChannel_Server: private CBaseMsgRecv {
public:
  CPubChannel_Server(im::pCMqttClient mqtt, std::string pub_name);
  ~CPubChannel_Server();
  bool Init(Func_AsyncResult func,
            std::function<void(cMsgBuf)> recv_func);
};

class CPubChannel_Client: public CBaseMsgSend {
public:
  CPubChannel_Client(im::pCMqttClient mqtt, std::string pub_name);
  ~CPubChannel_Client();

  // 发送私有通道信息
  struct PriInfo {
    std::string pri_name;
  };
  void Send_PrivateInfo(PriInfo *args);

};

#pragma endregion

// 用户-服务 私有通道
class CPriChannel {
public:
  CPriChannel();
  ~CPriChannel();

  struct SInitArgs {
    bool client;
    im::pCMqttClient mqtt;
    std::wstring pub_name;

    std::function<void(cMsgBuf)> func_recv;
  };

  bool Init(SInitArgs *args, Func_AsyncResult func);

private:
  Func_AsyncResult init_finished_;
  void InitFinished(bool suc) {
    if (init_finished_)
      init_finished_(suc);
  }

  bool SendTestMsg();

  bool client_;
};
StdSharedPtr_Typedef(CPriChannel);


class CMsg {
public:
  CMsg();
  virtual ~CMsg();

  struct SInitArgs {
    std::wstring user_name;
    std::wstring channel_name;
    im::pCMqttClient mqtt;
    MsgBuf send_test_buf;   // 发送测试数据时带的buf
    Func_AsyncResult func_connected;  // 连接流程走完后的回调
    std::function<MsgBuf(MsgBuf)> func_recv_testbuf; // server接到客户端的buf
  };

  bool Init(SInitArgs *info);
  bool Uninit();

  // 通过该接口发送数据
  bool SendMsg(cMsgBuf buf, Func_AsyncResult func);

protected:
  virtual bool _Init() = 0;

protected:
  void Connected(bool suc);
private:
  Func_AsyncResult func_connected_;

protected:
  bool is_client_;

  std::string send_channel_name_, recv_channel_name_;
  im::pCMqttClient mqtt_;
};
StdSharedPtr_Typedef(CMsg);

class CClientMsg: public CMsg {
public:
  CClientMsg();

protected:
  virtual bool _Init() override;
};
StdSharedPtr_Typedef(CClientMsg);

class CServerMsg: public CMsg {
public:
  CServerMsg();

protected:
  virtual bool _Init() override;
};
StdSharedPtr_Typedef(CServerMsg);

// 消息管理类，可以根据不同的类型自动生成消息通道
class CServerMsgManager {
private:
  CServerMsgManager();
public:
  static CServerMsgManager *Get();
  ~CServerMsgManager();

  struct SInitArgs {
    im::pCMqttClient mqtt;
  };

  struct SCallbackFunc {
    std::function<bool(std::wstring channel_name)> func_init;
    std::function<bool()> func_uninit;
    std::function<void(cMsgBuf) > func_recv;
  };

private:
  im::pCMqttClient mqtt_;

};
StdSharedPtr_Typedef(CServerMsgManager);

#pragma region def
class TestCls;
StdSharedPtr_Typedef(TestCls);
class TestCls: public im::msg::CServerMsg {
public:
  bool Initialization(std::wstring channel_name) {
    msg_manager = std::make_shared<im::msg_proto::CProtoManager>();
    return true;
  };
  bool Uninitialization() {
    return true;
  };
  bool RecvMsg(cMsgBuf buf) {
    msg_manager->ParseMsg(im::msg_proto::EChannelType::login, buf);
  }
  void RegCallback() {
    im::msg_proto::Msg_Pub_TestChannel::RegCallback(
      msg_manager.get(),
      std::bind(&TestCls::OnMsg_TestChannel, this, std::placeholders::_1));
  }

  void OnMsg_TestChannel(im::msg_proto::pMsg_Pub_TestChannel msg) {

  }

  im::msg_proto::pCProtoManager msg_manager;
};

/*
#define M_BeginDefineServerMsgClass(class_name, channel_type) \
class class_name; \
StdSharedPtr_Typedef(class_name); \
class class_name: public im::msg::CServerMsg { \
public: \
  bool Initialization(std::wstring channel_name); \
  bool Uninitialization(); \
  bool RecvMsg(const im::msg_proto::pBaseMsg &); \
  void RegCallback() { \
  }

#define M_EndDefineServerMsgClass() \
};*/

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
