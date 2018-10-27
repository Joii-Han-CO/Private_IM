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

CLog::~CLog() {}

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
      base::time::PrintTime("%Y_%m_%d-%H_%M_%S--"));

    // 获取PID
    std::wstring pid = std::to_wstring(base::sys::GetPID());

    args->log_path = L"im_" + t + pid + L".log";
  }

  // 打开文件
  file_ = std::make_shared<std::ifstream>(args->log_path);

  // 写入第一条日志

  is_init_ = true;
  return true;
}

void CLog::OutPut(const wchar_t * d) {
  if (is_init_ == false)
    return;
}

void CLog::OutPut(const char * d) {
  if (is_init_ == false)
    return;
}

#pragma region namespace
}
}
#pragma endregion
