#include "pch.h"
#include "im_msg.h"

#pragma region namespace
namespace im {
namespace msg {
#pragma endregion

#pragma region Interface

CClientMsg::CClientMsg() {
  is_client_ = true;
}

bool CClientMsg::_Init() {
  return false;
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
