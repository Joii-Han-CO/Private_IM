#include "pch.h"
#include "server_user_manager.h"

#pragma region
namespace server {
#pragma endregion

CSUserManager::CSUserManager() {}

CSUserManager * CSUserManager::Get() {
  static CSUserManager s;
  return &s;
}

CSUserManager::~CSUserManager() {}

bool CSUserManager::AddUser(im::msg_proto::pPP_CreatePrivateChannel msg) {
  auto user = std::make_shared<CSUser>();

  auto func = [this, user](bool suc) {
    if (suc == false) {
      PrintLogWarn("Add user failed");
      return;
    }
    users_.push_back(user);
  };
  if (user->Init(msg, func) == false) {
    PrintLogWarn("Init user failed");
    return false;
  }

  return true;
}

void CSUserManager::CleanUser() {
  users_.clear();
}

#pragma region
}
#pragma endregion
