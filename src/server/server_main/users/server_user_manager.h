#pragma once
#include "server_user.h"

namespace server {

class CSUserManager {
private:
  CSUserManager();
public:
  static CSUserManager* Get();
  ~CSUserManager();

  bool AddUser(im::msg_proto::pPP_CreatePrivateChannel msg);

  // 清空人员列表
  void CleanUser();
private:

  std::list<pCSUser> users_;
};

}
