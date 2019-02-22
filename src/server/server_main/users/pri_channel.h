#pragma once

#pragma region
namespace server {
#pragma endregion

// 每个用户的私有通道，可以双向连接
class UserPriChannel: public im::msg::CPriChannel {
public:
  bool Init(base::_uuid::BUID *channel_id,
            Func_AsyncResult func);

private:
  im::msg_proto::pCProtoManager msg_manager_;
};
StdSharedPtr_Typedef(UserPriChannel);

#pragma region
}
#pragma endregion
