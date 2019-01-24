#include "pch.h"
#include "im_s_user_manager.h"
#include "s_user_manager.h"

#pragma region namespace
namespace im {
namespace s_user_manager {
#pragma endregion

bool InitServerUserManager() {
  return CServerUserManager::Get()->Init();
}

bool WaitAndExit() {
  auto c = CServerUserManager::Get();
  if (c->Exit() == false)
    return false;
  if (c->WaitExit() == false)
    return false;
  return true;
}

#pragma region namespace
}
}
#pragma endregion
