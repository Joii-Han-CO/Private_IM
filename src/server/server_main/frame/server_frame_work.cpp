#include "pch.h"
#include "server_frame_work.h"

#pragma region
namespace server {
#pragma endregion

CSFarmeWork::CSFarmeWork() {}

CSFarmeWork * CSFarmeWork::Get() {
  static CSFarmeWork m;
  return &m;
}

CSFarmeWork::~CSFarmeWork() {
  if (pool_) {
    pool_ = nullptr;
  }
}

bool CSFarmeWork::Init() {
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

  if (InitSql() == false) {
    return false;
  }

  PrintLogInfo("Server management init success!");
  return true;
}

im::config::pCConfig CSFarmeWork::GetGlobalConfig() {
  return global_config_;
}

base::pCThreadPool CSFarmeWork::GetPool() {
  return pool_;
}

bool CSFarmeWork::InitSql() {
  auto sql_host = global_config_->GetVal(L"mqtt_server", L"sql_host");
  auto sql_name = global_config_->GetVal(L"mqtt_server", L"sql_name");
  auto sql_pwd = global_config_->GetVal(L"mqtt_server", L"sql_pwd");
  auto sql_db = global_config_->GetVal(L"mqtt_server", L"sql_db");

  if (im::s_sql::CSqlManager::Get()->Init(
    sql_host, sql_name, sql_pwd, sql_db) == false) {
    return false;
  }
  return true;
}

#pragma region
}
#pragma endregion
