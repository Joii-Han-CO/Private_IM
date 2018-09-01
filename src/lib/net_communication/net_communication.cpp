#include "stdafx.h"
#include "net_communication.h"

#pragma region namespace
namespace im {
namespace nc {
#pragma endregion

CNetCom::CNetCom():
  base::log::Log(std::bind(&CNetCom::LogCallabck,
                           this, std::placeholders::_1)) {}

CNetCom::~CNetCom() {}

// 初始化
bool CNetCom::Init(const SNetCom_InitArgs &args,
                   const std::function<void(int)> &cb) {
  return true;
}

// 释放
bool CNetCom::Release(const std::function<void()> &cb) {
  return true;
}

bool CNetCom::Send(const std::vector<char> &data,
                   const FUNC_SendCallback &cb) {
  return true;
}

bool CNetCom::Request(const std::vector<char> &data,
                      const FUNC_RequestCallback &cb) {
  return true;
}

void CNetCom::LogCallabck(const base::log::SBaseLog &func) {
  if (cb_log_)
    cb_log_(func);
}

#pragma region namespace
}
}
#pragma endregion
