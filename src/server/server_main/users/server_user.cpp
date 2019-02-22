#include "pch.h"
#include "server_user.h"

#pragma region
namespace server {
#pragma endregion

CSUser::CSUser() {}

CSUser::~CSUser() {}

bool CSUser::Init(im::msg_proto::pPP_CreatePrivateChannel msg,
                  Func_AsyncResult func) {
  assert(func != nullptr);

  user_name_ = msg->user_name_;
  client_type_ = msg->client_type_;
  client_id_ = msg->client_id_;

  pri_channel_ = std::make_shared<UserPriChannel>();

  auto res_func = [this, func](bool suc) {
    if (suc == false) {
      PrintLogErro("ServerUser init private channel faield 2");
      func(false);
      return;
    }
    if (ResLoginStatus(func) == false) {
      PrintLogErro("ServerUser send resloginStatus faield");
      func(false);
      return;
    }
  };

  if (pri_channel_->Init(&msg->channel_name_, res_func) == false) {
    PrintLogErro("ServerUser init private channel faield");
    return false;
  }
  return true;
}

bool CSUser::ResLoginStatus(Func_AsyncResult func) {
  im::msg_proto::PR_ResLoginStatus proto;
  proto.status = 1;

  auto buf = proto.Serializate();

  auto ref_func = [func](bool suc) {
    if (suc == false) {
      PrintLogWarn("Send login res status failed");
      func(false);
      return;
    }
    func(true);
  };
  if (pri_channel_->Send(buf, ref_func) == false) {
    PrintLogWarn("Send login res status failed");
    func(false);
    return false;
  }

  return true;
}

#pragma region
}
#pragma endregion
