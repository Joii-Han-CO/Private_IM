#include "pch.h"
#include "im_c_framework.h"
#include "im_log.h"
#include "base/path.hpp"
#include "base/time.hpp"
#include "im_global_val.h"

#pragma region namespace
namespace im {
namespace c_framework {
#pragma endregion

ClientFramework::ClientFramework() {}

ClientFramework::~ClientFramework() {}

ClientFramework *ClientFramework::Get() {
  static ClientFramework cf;
  return &cf;
}

bool ClientFramework::Init() {

  im::log::SLog_InitArgs log_args;
  log_args.output_ctrl = true;
  log_args.save_file = false;
  log_args.print_dbg = true;
  log_args.print_info = true;
  log_args.print_warn = true;
  log_args.print_erro = true;
#ifdef _WIN32  
  auto log_file_name =
    base::GB2312ToUtf16(base::time::PrintTime()) + L"_" +
    std::to_wstring(GetCurrentProcessId()) + L".log";
#else
  auto log_file_name =
    base::GB2312ToUtf16(base::time::PrintTime()) + L"_" +
    std::to_wstring(getpid()) + L".log";
#endif
  log_args.log_path = base::_path::GetExeDir<wchar_t>() + L"/" + log_file_name;
  if (im::log::CLog::Get()->Init(&log_args) == false) {
    // 初始化日志失败，后面一切都没有意义了...
    return false;
  }

  // 初始化全局配置文件
  ptr_global_config_ = std::make_shared<im::config::CConfig>();
  std::wstring global_config_path = base::_path::GetExeDir<wchar_t>() +
    L"/" + im::gv::g_cli_cfg_path;
  if (ptr_global_config_->Init(global_config_path) == false) {
    PrintLogErro(L"Init global config file fialed, des:%s",
                 ptr_global_config_->GetLastErr_Wc());
    return false;
  }

  return true;
}

im::config::pCConfig ClientFramework::GetGlobalConfigPtr() {
  if (ptr_global_config_)
    return ptr_global_config_;
  return nullptr;
}

im::c_login::pClientLogin ClientFramework::GetLoginPtr() {
  if (!ptr_login_)
    ptr_login_ = std::make_shared<im::c_login::ClientLogin>();
  return ptr_login_;
}

#pragma region namespace
}
}
#pragma endregion
