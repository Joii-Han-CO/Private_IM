#pragma once
#include "pri_channel.h"

namespace server {

class CSUser {
public:
  CSUser();
  ~CSUser();

  bool Init(im::msg_proto::pPP_CreatePrivateChannel msg,
            Func_AsyncResult func);

private:
  bool ResLoginStatus(Func_AsyncResult func);

  pUserPriChannel pri_channel_;

  std::wstring user_name_;
  im::EClientType client_type_;
  base::_uuid::BUID client_id_;
};
StdSharedPtr_Typedef(CSUser);

}
