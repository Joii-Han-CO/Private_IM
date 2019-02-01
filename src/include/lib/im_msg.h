#pragma once
#include "base/type_def.h"
#include "im_mqtt.h"
#include "im_msg_proto.h"


// 封装mqtt和proto，用于消息传输的基础通道

#pragma region namespace
namespace im {
namespace msg {
#pragma endregion

enum class EMsg_InitStatus {
  def = 0,
  connecting,
  connected,
  sending,
  sended,
  ok,
  error,
};

typedef std::function<void(EMsg_InitStatus)> Func_MsgResult;

struct SMsg_InitArgs {
  std::wstring user_name;
  std::wstring channel_name;
  im::pCMqttClient mqtt;
  bool caller = false;    // true:  先发消息， false：  等对方的消息到了再发

  Func_AsyncResult init_finished;
  Func_AsyncResult connect_finished;
};

class CMsg {
public:
  CMsg();
  virtual ~CMsg();

  bool Init(SMsg_InitArgs *info);
  bool Uninit();

  bool SendTestMsg();

  bool SendMsg(const MsgBuf &buf, Func_AsyncResult func);

private:
  void InitFinished(bool suc);
  void ConnectFinished(bool suc);

  void RecvMsg(const MsgBuf &buf);

  bool SendTestMsg_(int rand_num);

  void RecvTestMsg(im::msg_proto::pMsg_TestChannel msg);

private:
  Func_AsyncResult init_finished_;
  Func_AsyncResult connect_finished_;
  bool caller_;

  int cur_init_count = 0;

  std::string send_channel_name_, recv_channel_name_;
  im::pCMqttClient mqtt_;

};

#pragma region namespace
}
}
#pragma endregion
