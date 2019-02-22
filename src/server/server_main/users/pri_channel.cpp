#include "pch.h"
#include "pri_channel.h"
#include "frame/server_frame_work.h"
#include "frame/publish_channel.h"

#pragma region
namespace server {
#pragma endregion

bool UserPriChannel::Init(base::_uuid::BUID *channel_id,
                          Func_AsyncResult func) {
  msg_manager_ = std::make_shared<im::msg_proto::CProtoManager>();
  //msg_manager_->RegFunc();

  im::msg::CPriChannel::SInitAgrs args;
  args.mqtt = CSPubChannel::Get()->GetMqtt();
  args.caller = false;
  args.channel_id = channel_id;
  args.manager = msg_manager_;
  return im::msg::CPriChannel::Init(&args, func);
}

#pragma region
}
#pragma endregion
