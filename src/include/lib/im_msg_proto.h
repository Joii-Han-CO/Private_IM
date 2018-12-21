#pragma once
#include "base/type_def.h"
#include "base/time.hpp"
#include <list>
#include <vector>
#include <memory>


#pragma region
namespace im {
namespace msg_proto {
#pragma endregion

#pragma region MsgStructs

typedef std::vector<char> MsgBuffer;

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

#pragma endregion

pSIM_MsgText ParseText(void *data, size_t len);

MsgBuffer SerializationTextMsg(pSIM_MsgText msg);

#pragma region
}
}
#pragma endregion

