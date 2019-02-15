#include "pch.h"
#include "im_msg.h"

#pragma region namespace
namespace im {
namespace msg {
#pragma endregion

#pragma region Interface

CServerMsg::CServerMsg() {
  is_client_ = false;
}

bool CServerMsg::_Init() {


  return true;
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
