#include "pch.h"
#include "im_log.h"
#include "base/time.hpp"
#include "base/character_conversion.hpp"
#include "base/system.hpp"
#include "base/path.hpp"
#include "base/debug.hpp"


#pragma region namespace
namespace im {
namespace log {
#pragma endregion

CLog::CLog() {}

CLog::~CLog() {
#if LogASyncWrite
  StopTask();
  WaitTask();
#endif
  if (file_ && *file_)
    file_->flush();
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
      base::time::PrintTime());

    // 获取PID
    args->log_path = base::format::FormatStr(
      L"%s/im_%s--%06d.log",
      base::_path::GetExeDir<wchar_t>().c_str(),
      t.c_str(), base::sys::GetPID());
  }

  print_dbg_ = args->print_dbg;
  print_info_ = args->print_info;
  print_warn_ = args->print_warn;
  print_erro_ = args->print_erro;

#if LogASyncWrite
  StartTask();
#endif

  // 打开文件
  file_ = std::make_shared<std::ofstream>(args->log_path);

  // 写入第一条日志
  PrintHeader();

  return true;
}

bool CLog::FilterType(base::log::EBaseLogType t) {
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

void CLog::OutPutBase(const std::string &d) {
  if (print_ctrl_)
    base::debug::OutPut(d.c_str());

  // 写入日志...
  if (!file_)
    return;
  if (d.empty())
    return;

#if !LogASyncWrite
  std::unique_lock<std::mutex> lock(write_log_sync_);
#endif
  // 开始写入日志
  file_->write(d.c_str(), d.size());
}

std::string CLog::MakeHeader(base::log::EBaseLogType t,
                             const char *prj_name, const char * source_file,
                             const char * func_name, int line_num) {
  // 虽然不优雅，但是性能提升还是很明显的...
  return "[" + std::string(base::log::GetBaseLogTypeStr(t)) + "],[" +
    std::string(prj_name) + "],[" +
    std::string(func_name) + ":" + std::to_string(line_num);
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

void CLog::PrintHeader() {
  std::string p =
    "------------Start---------------\n"
    "--ExePath:%s\n"
    "--BeginTime:%s\n"
    "--------------------------------\n\n";
  std::string path = base::_path::GetExePath<char>();
  std::string tim = base::time::PrintTime();
  p = base::format::FormatStr(p.c_str(), path.c_str(), tim.c_str());

  OutPutBase(p);
}

#pragma region namespace
}
}
#pragma endregion
