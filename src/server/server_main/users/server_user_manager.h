#pragma once
#include "server_user.h"

#pragma region
namespace server {
#pragma endregion

class CSUserManager {
private:
  CSUserManager();
public:
  static CSUserManager* Get();
  ~CSUserManager();

  bool AddUser(im::msg_proto::pPP_CreatePrivateChannel msg);

  // �����Ա�б�
  void CleanUser();
private:

  std::list<pCSUser> users_;
};

#pragma region
}
#pragma endregion

