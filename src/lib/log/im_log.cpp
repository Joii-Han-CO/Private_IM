#include "stdafx.h"
#include "im_log.h"
#include "base/time.hpp"
#include "base/character_conversion.hpp"
#include "base/system.hpp"


#pragma region namespace
namespace im {
namespace log {
#pragma endregion

CLog::CLog() {}

CLog::~CLog() {
  StopTask();
  WaitTask();
}

CLog* CLog::Get() {
  static CLog l;
  return &l;
}

bool CLog::Init(SLog_InitArgs *args) {
  SLog_InitArgs def_args;
  if (args == nullptr) {
    args = &def_args;
  }

  if (args->log_path.empty()) {
    // 获取当前时间
    std::wstring t = base::Utf8ToUtf16(
      base::time::PrintTime("%Y_%m_%d-%H_%M_%S"));

    // 获取PID
    args->log_path.resize(512);

#ifdef WIN32
    swprintf_s((wchar_t*)args->log_path.c_str(), args->log_path.size(),
               L"im_%s--%0000d.log", t.c_str(), base::sys::GetPID());
#else
    swprintf((wchar_t*)args->log_path.c_str(),
             L"im_%s--%0000d.log", t.c_str(), base::sys::GetPID());
#endif // WIN32

  }

  print_dbg_ = args->print_dbg;
  print_info_ = args->print_info;
  print_warn_ = args->print_warn;
  print_erro_ = args->print_erro;

  StartTask();

  // 打开文件
  file_ = std::make_shared<std::ofstream>(args->log_path);

  // 写入第一条日志

  OutPutBase("Start\n");

  return true;
}

void CLog::OutPutBase(const std::wstring &d) {
  // 写入日志...
  if (!file_)
    return;
  if (d.empty())
    return;

  auto r = base::Utf16ToUtf8(d);

  // 开始写入日志
  file_->write(r.c_str(), r.size());
}

void CLog::OutPutBase(const std::string &d) {
  // 写入日志...
  if (!file_)
    return;
  if (d.empty())
    return;

  // 开始写入日志
  file_->write(d.c_str(), d.size());
}

std::string CLog::MakeHeader(base::log::EBaseLogType t,
                             const char *prj_name, const char * source_file,
                             const char * func_name, int line_num) {
  const char *base_format =
    "[%s],[%s],[%s-%d]:%s\n";

  auto str_type = base::log::GetBaseLogTypeStr(t);
  return base::log::FormatStr(base_format, str_type, prj_name,
                              func_name, line_num, "%s");
}

bool CLog::WriteLog(base::log::EBaseLogType t) {
  switch (t) {
  case base::log::EBaseLogType::dbg:
    return print_dbg_;
  case base::log::EBaseLogType::info:
    return print_info_;
  case base::log::EBaseLogType::warn:
    return print_warn_;
  case base::log::EBaseLogType::erro:
    return print_erro_;
  default:
    return false;
  }
}

void CLog::PrintHeader() {}

std::wstring CLog::GetTemplateStr(std::string s, const wchar_t *sz) {
  return base::Utf8ToUtf16(s);
}
std::string CLog::GetTemplateStr(std::string s, const char *sz) {
  return s;
}

#pragma region namespace
}
}
#pragma endregion
