#pragma once
#include "base/type_def.h"
#include "im_type_def.h"
#include "base/time.hpp"
#include "base/uuid.hpp"
#include "im_log.h"
#include <list>
#include <map>
#include <vector>
#include <memory>


#pragma region
namespace im {
namespace msg_proto {
#pragma endregion

// 预定义
#pragma region def

class CProtoCallbackManager;
StdSharedPtr_Typedef(CProtoCallbackManager);

class CProtoManager;
StdSharedPtr_Typedef(CProtoManager);

class CBaseProtoHeader;
StdSharedPtr_Typedef(CBaseProtoHeader);

class CBaseProto;
StdSharedPtr_Typedef(CBaseProto);

#define MP_BeginDefMsgClass(msg_class, channel_type, msg_type) \
class msg_class; \
StdSharedPtr_Typedef(msg_class); \
class msg_class: public CBaseProto { \
  public: \
  msg_class() { \
    header_ = std::make_shared<CBaseProtoHeader>(\
      channel_type, (uint8_t)msg_type); \
  }; \
  msg_class(pCBaseProtoHeader header): CBaseProto(header) {}; \
  bool Parse(cMsgBuf buf); \
  MsgBuf Serializate(); \
  static void RegCallback( \
    CProtoManager *msg_manager, \
    std::function<void(std::shared_ptr<msg_class>)> func) { \
    if (func == nullptr) \
      return; \
    auto f = [func](pCBaseProto m) { \
      func(std::static_pointer_cast<msg_class>(m)); \
    }; \
    msg_manager->RegFunc( \
      channel_type, (uint8_t)msg_type, f); \
  } \
  static void RegGlobalCallback() { \
    auto func = [](cMsgBuf buf, pCBaseProtoHeader header) { \
      auto msg = std::make_shared<msg_class>(header); \
      msg->Parse(buf); \
      return msg; \
    }; \
    SMsgParseCB scb; \
    scb.msg_cb = nullptr; \
    scb.msg_parse = func;\
    CProtoCallbackManager::Get()-> \
      RegFunc(channel_type, (uint8_t)msg_type, scb); \
    } \
public:

#define MP_EndDefMsgClass() \
};

// 消息体部分的消息长度
#define MP_SerializateMsg(buf_size) \
  MsgBuf buf; \
  { \
    auto ___buf_size = CBaseProtoHeader::Size() + buf_size; \
    buf.resize(___buf_size); \
    if (header_->Serializate(buf) == false) \
      return MsgBuf(); \
  }

#pragma endregion

#pragma region type_def

// 枚举消息通道
enum class EChannelType {
  pub_channel = 0,  // 公共通道（Server监听）
  golbal_channel,   // server的全局通道
  login,            // 登陆通道
};

// 公共消息
enum class EPubMsgType {
  none = 0,
  test_channel,
  end,
};

enum class EGlobalType {
  begin = (int)EPubMsgType::end,
  private_info,
};

// 消息类型
struct SMsgType {
  EChannelType channel_type;
  uint8_t msg_type;

  SMsgType operator=(const SMsgType &t1) {
    SMsgType t2;
    t2.channel_type = t1.channel_type;
    t2.msg_type = t1.msg_type;
    return t2;
  }

  uint16_t GetVal() {
    if (msg_type < (uint8_t)EPubMsgType::end)
      return msg_type;

    return ((int8_t)channel_type << 8) + msg_type;
  }
};

#pragma endregion

// 管理消息函数的注册及获取，同一个消息只允许注册一个函数
typedef std::function<void(pCBaseProto)> Func_MsgCb;
typedef std::function<pCBaseProto(cMsgBuf, pCBaseProtoHeader)> Func_MsgParse;
struct SMsgParseCB {
  Func_MsgCb msg_cb;
  Func_MsgParse msg_parse;

  SMsgParseCB& operator=(const SMsgParseCB &t1) {
    SMsgParseCB t2;
    this->msg_cb = t1.msg_cb;
    this->msg_parse = t1.msg_parse;
    return *this;
  }
};

class CProtoCallbackManager {
private:
  CProtoCallbackManager();
public:
  static CProtoCallbackManager *Get();
  void Init();
  void RegFunc(EChannelType ct, uint8_t mt, SMsgParseCB &callback);
  SMsgParseCB GetMsgCBInfo(uint16_t);

private:
  std::map<uint16_t, SMsgParseCB> msg_func_;
};

// 消息管理类
class CProtoManager {
public:
  CProtoManager();
  ~CProtoManager();

  // 如果添加新消息，需要在这里注册
  void ParseMsg(EChannelType type, cMsgBuf buf);

  // 注册回调函数触发
  void RegFunc(EChannelType ct, uint8_t mt, Func_MsgCb callback);

private:
  std::map<uint16_t, SMsgParseCB> msg_func_;
};

// 消息头
class CBaseProtoHeader {

  //////////////////////////////////////////////////////////////////////////
  // Header Type
  // uint8_t  uint64_t
  //////////////////////////////////////////////////////////////////////////

public:
  CBaseProtoHeader(EChannelType channel_type, uint8_t type);
  CBaseProtoHeader(EChannelType channel_type, uint8_t type,
                   base::_uuid::BaseUUID *msg_id);
  CBaseProtoHeader(EChannelType channel_type, cMsgBuf buf);

  // header消息中占用的尺寸
  static int Size();

  // 拼接消息
  bool Serializate(MsgBuf &buf);

  //判断消息是坏掉的
  bool IsBroken();

  SMsgType type_;
  base::time::pBaseTime time_;
  base::_uuid::BaseUUID msg_id_;
};

// 消息基类
class CBaseProto {
public:
  CBaseProto() {};
  CBaseProto(pCBaseProtoHeader header): header_(header) {};
  bool Parse(cMsgBuf buf) { return false; };
  MsgBuf Serializate() { return MsgBuf(); };
  int Size() { return 0; };

protected:
  pCBaseProtoHeader header_;
};

//////////////////////////////////////////////////////////////////////////
// 介绍
//  1. 通道。在 EChannelType 中定义了mqtt的各种通道
//  2. 消息命令。分为公共消息和专有消息。
//    在 EPubMsgType 中定义了公共通道，公共通道定义的消息在其他通道也可能会出现；
//    其他通道消息全部的通道都是从  EPubMsgType::end 开始的，自定义时注意，
//      现在之分配到了 uint8_t 大小用于消息的存储
// 添加消息说明：
//  1. 在枚举中定义通道和消息
//  2. 使用宏定义消息类： MP_BeginDefMsgClass 和 MP_EndDefMsgClass
//    其中 MP_BeginDefMsgClass 需要 通道枚举 消息枚举
//  3. 实现 
//        bool Parse(cMsgBuf buf)
//        MsgBuf Serializate()
//    其中 Parse 在最初使用 MP_SerializateMsg 定义了 buf，同时序列化 header
//  4. 在 MsgManager::Init 中添加 msg_class_name::RegGlobalCallback() 函数
//    用于注册，在解析消息的时候可以直接路由到注册的函数中
//  5. 在程序的最初调用 im::msg_proto::MsgManager::Get()->Init(); 初始化消息管理类
//    该类是单例，用于管理全局的消息。
//  6. 在消息被接收前调用
//        im::msg_proto::Msg_Pub_TestChannel::RegCallback(func_base);
//    用于注册收到消息后的业务相关函数，该函数会在调用 
//        im::msg_proto::MsgManager::Get()->ParseMsg
//    的线程中执行，如果业务处理执行时间太长会卡死该函数。
//    同时注意 ParseMsg 没有做线程安全相关处理
//////////////////////////////////////////////////////////////////////////

// 全局通道的消息
MP_BeginDefMsgClass(Proto_G_PrivateInfo,
                    EChannelType::golbal_channel, EGlobalType::private_info);
MP_EndDefMsgClass();

MP_BeginDefMsgClass(Msg_Pub_TestChannel,
                    EChannelType::pub_channel, EPubMsgType::test_channel);
int status = 0;
MP_EndDefMsgClass();

#pragma region
}
}
#pragma endregion
