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

#pragma region Header

class MsgBase_Header {
public:
  static int GetSize();
  bool Parse(const MsgBuf &buf);
  bool Serialization(MsgBuf &buf);

public:
  uint8_t type = 0;
  base::time::BaseTime time;
};
StdSharedPtr_Typedef(MsgBase_Header);

#pragma endregion

#pragma region Base

class MsgBase {
  MsgBase();

public:
  pMsgBase_Header header;
};

#pragma endregion

#pragma region LoginChannel

// 枚举   uint8_t
// 登陆通道消息体类型
enum class ELoginMsgType {
  Error = 0,      // 报错...
  UserLogin,      // C-->S 用户登陆
  UserLoginSRes,  // S-->C server登陆成功后平台返回标志
  UserLoginCRes,  // C-->S client收到了来自平台的登陆标示
  UserLogout,     // C-->S 用户登出
};

// 基类, 登陆通道基类
struct Msg_LoginChannel {
  ProroMsgBaseClassVirtual(Msg_LoginChannel);

public:
  ELoginMsgType type;
};
StdSharedPtr_Typedef(Msg_LoginChannel);

// 用户登陆消息
struct Msg_UserLogin: public Msg_LoginChannel {
  ProtoMsgChildClassVirtual(Msg_UserLogin);
  Msg_UserLogin() {
    type = im::msg_proto::ELoginMsgType::UserLogin;
  }

public:
  std::wstring user_name;
  std::wstring login_channel;
  im::EClientType client_type;
  uint32_t client_id;
};
StdSharedPtr_Typedef(Msg_UserLogin);

// 服务端返回登陆成功
struct Msg_UserLoginSRes: public Msg_LoginChannel {
  ProtoMsgChildClassVirtual(Msg_UserLogin);
  Msg_UserLoginSRes() {
    type = im::msg_proto::ELoginMsgType::UserLoginSRes;
  }

public:
  uint32_t status;

};
StdSharedPtr_Typedef(Msg_UserLoginSRes);

struct Msg_UserLoginClientRes: public Msg_LoginChannel {
  ProtoMsgChildClassVirtual(Msg_UserLoginClientRes);
  Msg_UserLoginClientRes() {
    type = ELoginMsgType::UserLoginCRes;
  };

public:
  uint32_t status;
};
StdSharedPtr_Typedef(Msg_UserLoginClientRes);

// 登出
struct Msg_UserLogout: public Msg_LoginChannel {
  ProtoMsgChildClassVirtual(Msg_UserLogin);
  Msg_UserLogout() {
    type = im::msg_proto::ELoginMsgType::UserLogout;
  }

public:
  uint32_t status;
};
StdSharedPtr_Typedef(Msg_UserLogout);

// 解析登陆相关消息
pMsg_LoginChannel Parse_LoginChannel(const MsgBuf &buf);

#pragma endregion

#pragma region
}
}
#pragma endregion

