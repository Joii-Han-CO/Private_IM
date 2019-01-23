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

  log_args.output_ctrl = true;
  log_args.save_file = false;

  log_args.log_path = L"server_main.log";
  if (im::log::CLog::Get()->Init(&log_args) == false) {
    std::cout << "init log module failed" << std::endl;
    return false;
  }

  // 初始化线程池
  pool_ = std::make_shared<base::CThreadPool>();

  // 初始化全局配置文件
  global_config_ = std::make_shared<im::config::CConfig>();
  auto cfg_path = base::_path::GetExeDir<wchar_t>() + L"\\" +
    im::gv::g_ser_cfg_path;
  if (global_config_->Init(cfg_path) == false) {
    std::cout << "init config module failed" << std::endl;
    return false;
  }

  return true;
}

im::config::pCConfig CManagement::GetGlobalConfig() {
  return global_config_;
}

#pragma region
}
#pragma endregion
