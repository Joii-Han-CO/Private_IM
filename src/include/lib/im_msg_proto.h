#pragma once
#include "base/type_def.h"
#include "im_type_def.h"
#include "base/time.hpp"
#include <list>
#include <vector>
#include <memory>


#pragma region
namespace im {
namespace msg_proto {
#pragma endregion

#pragma region Def

#define ProroMsgBaseClassVirtual(ClassName) \
public: \
  virtual ~ClassName() {}; \
  virtual bool Parse(const MsgBuf &buf) = 0; \
  virtual MsgBuf Serializate() = 0;

#define ProtoMsgChildClassVirtual(ClassName) \
public: \
  virtual bool Parse(const MsgBuf &buf) override; \
  virtual MsgBuf Serializate() override;

#pragma endregion

#pragma region LoginChannel

// 枚举   uint8_t
// 登陆通道消息体类型
enum class ELoginMsgType {
  Error = 0,   // 报错...
  UserLogin,    // 用户登陆
  UserLoginRes, // server登陆成功后平台返回标志
};

// 基类
// 登陆通道基类
struct Msg_LoginChannel {
  ProroMsgBaseClassVirtual(Msg_LoginChannel);

public:
  ELoginMsgType type;
};
StdSharedPtr_Typedef(Msg_LoginChannel);

// 用户登陆消息
struct Msg_UserLogin: public Msg_LoginChannel {
  ProtoMsgChildClassVirtual(Msg_UserLogin);
  Msg_UserLogin();

public:
  std::wstring user_name;
  std::wstring login_channel;
  im::EClientType client_type;
  uint32_t client_id;
};
StdSharedPtr_Typedef(Msg_UserLogin);

struct Msg_UserLoginRes: public Msg_LoginChannel {
  ProtoMsgChildClassVirtual(Msg_UserLogin);
  Msg_UserLoginRes();

public:
  uint32_t status;

};

// 解析登陆相关消息
pMsg_LoginChannel Parse_LoginChannel(const MsgBuf &buf);

#pragma endregion

#pragma region Msg

enum class EIMMsg_Type {
  text,
  image,
};

// 基础的消息结构体
struct SIMMsg_Base {
  EIMMsg_Type type = EIMMsg_Type::text;
  // 消息ID
  base::time::BaseTime send_time;   // 消息的发送时间
  base::time::BaseTime receive_time;    // 消息的接收时间
};
typedef std::shared_ptr<SIMMsg_Base> pSIM_MsgBase;

// 基础文本消息
struct SIMMsg_Text: public SIMMsg_Base {
  std::wstring text;
};
typedef std::shared_ptr<SIMMsg_Text> pSIM_MsgText;

// 图片类型
enum class EIM_MsgImage_Type {
  none,
  jpg,
  png,
  bmp,
  gif,
};

// 图片消息
struct IMMsg_Image: public SIMMsg_Base {
  EIM_MsgImage_Type img_type = EIM_MsgImage_Type::none;
  int high = 0;
  int width = 0;
};
typedef std::shared_ptr<IMMsg_Image> pSIM_MsgImage;

pSIM_MsgText ParseText(void *data, size_t len);

MsgBuf SerializationTextMsg(pSIM_MsgText msg);

#pragma endregion

#pragma region
}
}
#pragma endregion

