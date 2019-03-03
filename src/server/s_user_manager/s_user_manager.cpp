#include "pch.h"
#include "s_user_manager.h"

namespace im {
namespace s_user_manager {

CServerUserManager* CServerUserManager::Get() {
  static CServerUserManager cs;
  return &cs;
}

CServerUserManager::CServerUserManager() {}

CServerUserManager::~CServerUserManager() {}

bool CServerUserManager::Init() {
  return true;
}

bool CServerUserManager::Exit() {
  return true;
}

bool CServerUserManager::WaitExit() {
  return true;
}

}
}
