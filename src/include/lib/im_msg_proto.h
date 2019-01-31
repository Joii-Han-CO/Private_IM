#pragma once
#include "base/type_def.h"
#include "im_type_def.h"
#include "base/time.hpp"
#include <list>
#include <map>
#include <vector>
#include <memory>


#pragma region
namespace im {
namespace msg_proto {
#pragma endregion

#pragma region Header

class MsgBase_Header {
public:
  static int GetSize();
  bool Parse(const MsgBuf &buf);
  bool Serializate(MsgBuf &buf);

public:
  uint8_t type = 0;
  base::time::pBaseTime time;
};
StdSharedPtr_Typedef(MsgBase_Header);

#pragma endregion

#pragma region Base

enum class EChannelType {
  none = 0,
  login,    // 登陆通道
};

class MsgBase;
StdSharedPtr_Typedef(MsgBase);

class MsgBase {
public:
  MsgBase();
  virtual ~MsgBase();

  static pMsgBase Parse(const MsgBuf &buf, EChannelType t);
  MsgBuf Serializate();

protected:
  virtual bool _Parse(const MsgBuf &buf) = 0;
  virtual bool _ParseEnd() = 0;
  virtual MsgBuf _Serializate() = 0;

public:
  MsgBase_Header header;

};

#pragma endregion

// 登陆通道
#pragma region LoginChannel

// 登陆通道消息体类型 uint8_t
enum class ELoginMsgType {
  Error = 0,      // 报错...
  UserLogin,      // C-->S 用户登陆
  UserLoginSRes,  // S-->C server登陆成功后平台返回标志
  UserLoginCRes,  // C-->S client收到了来自平台的登陆标示
  UserLogout,     // C-->S 用户登出
};

class MsgBase_Login;
StdSharedPtr_Typedef(MsgBase_Login);

class MsgBase_Login: public MsgBase {
public:
  MsgBase_Login();

  static pMsgBase_Login Parse(const MsgBuf &buf);

  static pMsgBase Factory(uint8_t type);

  ELoginMsgType type_;

protected:
  bool _ParseEnd() override;

};

// 用户登陆消息
struct Msg_UserLogin: public MsgBase_Login {
public:
  Msg_UserLogin() {
    header.type = (uint8_t)im::msg_proto::ELoginMsgType::UserLogin;
  }

protected:
  virtual bool _Parse(const MsgBuf &buf) override;
  virtual MsgBuf _Serializate() override;

public:
  std::wstring user_name;
  std::wstring login_channel;
  im::EClientType client_type;
  uint32_t client_id;
};
StdSharedPtr_Typedef(Msg_UserLogin);

// 服务端返回登陆成功
struct Msg_UserLoginSRes: public MsgBase_Login {
public:
  Msg_UserLoginSRes() {
    header.type = (uint8_t)im::msg_proto::ELoginMsgType::UserLoginSRes;
  }

protected:
  virtual bool _Parse(const MsgBuf &buf) override;
  virtual MsgBuf _Serializate() override;

public:
  uint32_t status;

};
StdSharedPtr_Typedef(Msg_UserLoginSRes);

// 客户端确认登陆成功
struct Msg_UserLoginClientRes: public MsgBase_Login {
public:
  Msg_UserLoginClientRes() {
    header.type = (uint8_t)im::msg_proto::ELoginMsgType::UserLoginCRes;
  }

protected:
  virtual bool _Parse(const MsgBuf &buf) override;
  virtual MsgBuf _Serializate() override;

public:
  uint32_t status;
};
StdSharedPtr_Typedef(Msg_UserLoginClientRes);

// 登出
struct Msg_UserLogout: public MsgBase_Login {
public:
  Msg_UserLogout() {
    header.type = (uint8_t)im::msg_proto::ELoginMsgType::UserLogout;
  }

protected:
  virtual bool _Parse(const MsgBuf &buf) override;
  virtual MsgBuf _Serializate() override;

public:
  uint32_t status;
};
StdSharedPtr_Typedef(Msg_UserLogout);

#pragma endregion

#pragma region
}
}
#pragma endregion
