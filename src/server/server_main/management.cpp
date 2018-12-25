#include "pch.h"
#include "management.h"

#pragma region
namespace server {
#pragma endregion

CManagement::CManagement() {}

CManagement * CManagement::Get() {
  static CManagement m;
  return &m;
}

CManagement::~CManagement() {
  if (pool_) {
    pool_ = nullptr;
  }
}

bool CManagement::Init() {
  // 初始化Log
  im::log::SLog_InitArgs log_args;
  log_args.print_dbg = true;
  log_args.print_info = true;
  log_args.print_warn = true;
  log_args.print_erro = true;
  log_args.log_path = L"server_main.log";
  im::log::CLog::Get()->Init();

  pool_ = std::make_shared<base::CThreadPool>();
  global_config_ = std::make_shared<im::config::CConfig>();

  return true;
}

im::config::pCConfig CManagement::GetGlobalConfig() {
  return global_config_;
}

#pragma region
}
#pragma endregion
