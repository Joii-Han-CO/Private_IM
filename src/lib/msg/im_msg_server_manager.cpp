#include "pch.h"
#include "im_msg.h"

#pragma region namespace
namespace im {
namespace msg {
#pragma endregion

#pragma region Interface

CServerMsgManager::CServerMsgManager() {}

CServerMsgManager * CServerMsgManager::Get() {
  static CServerMsgManager s;
  return &s;
}

CServerMsgManager::~CServerMsgManager() {}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
