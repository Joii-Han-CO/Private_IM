#include "pch.h"
#include "im_msg.h"

#pragma region namespace
namespace im {
namespace msg {
#pragma endregion

CPriChannel::CPriChannel() {}

CPriChannel::~CPriChannel() {}

bool CPriChannel::Init(CPriChannel::SInitAgrs *args,
                       Func_AsyncResult func) {
  assert(args != nullptr);
  assert(args->mqtt != nullptr);
  assert(args->channel_id != nullptr);
  assert(args->manager != nullptr);
  assert(func != nullptr);

  auto name = im::gv::g_mqtt_login_sub_ +
    base::_uuid::GetStr<char>(args->channel_id);
  std::string r_name, s_name;
  if (args->caller) {
    r_name = name + "_A";
    s_name = name + "_B";
  }
  else {
    r_name = name + "_B";
    s_name = name + "_A";
  }

  if (CBaseMsg::Init(args->mqtt, r_name, s_name,
                     im::msg_proto::EChannelType::private_channel,
                     args->manager,
                     func) == false) {
    PrintLogErro("Init private channel failed");
    return false;
  }

  return true;
}

#pragma region namespace
}
}
#pragma endregion
