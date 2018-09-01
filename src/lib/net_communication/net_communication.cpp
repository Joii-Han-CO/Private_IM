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


#pragma region Init

// 初始化
bool CNetCom::Init(const SNetCom_InitArgs &args,
                   const std::function<void(int)> &cb) {
  if (InitCallback(args) == false)
    return false;

  if (InitNet(args) == false)
    return false;

  return true;
}

bool CNetCom::InitCallback(const SNetCom_InitArgs &args) {
  if (args.cb_message == nullptr) {
    SetLastErrAndLog("args.cb_message is nullptr");
    return false;
  }
  if (args.cb_responose == nullptr) {
    SetLastErrAndLog("args.cb_responose is nullptr");
    return false;
  }
  cb_message_ = args.cb_message;
  cb_responose_ = args.cb_responose;
  cb_log_ = args.cb_log;
  return true;
}

bool CNetCom::InitNet(const SNetCom_InitArgs &args) {
  if (args.listener == true) {
    // 打开端口进行监听
  }
  else {
    // 打开对方端口
  }

  return true;
}

#pragma endregion

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
