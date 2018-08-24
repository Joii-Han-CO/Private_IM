#pragma once
#include "base/type_def.h"


#ifdef __cplusplus
extern "C" {
  namespace im {
  namespace msg_proto {
#endif

  // Protobuf
  struct IMMsg_ProtoBuf {
    const char * buf = nullptr;
    unsigned int size = 0;
  };
  // 释放
  IM_LIB_EXP void FreeProtoBuf(IN IMMsg_ProtoBuf *buf);

  // 消息状态
  enum class EIMMsg_Type {
    None = 0,
    Sending,    // 发送中
    Send_OK,    // 发送成功
    Receiving,  // 接收中
    Receive_OK, // 接收成功
  };

  // 基础类型，所有消息共有
  struct IMMsg_Base {
    EIMMsg_Type msg_type;   //消息类型

  };

  // 文本消息
  struct IMMsg_Text {
    IMMsg_Base base;
    const wchar_t *text = nullptr;
  };
  IM_LIB_EXP void FreeMsgText(IN IMMsg_Text *buf);

  // 文本消息
  IM_LIB_EXP bool ParseMsg_Text(IN IMMsg_ProtoBuf &buf, OUT IMMsg_Text &msg);
  IM_LIB_EXP bool GenerateMsg_Text(OUT IMMsg_ProtoBuf &buf, IN IMMsg_Text &msg);

#ifdef __cplusplus
  }
  }
}
#endif
