﻿#pragma once
#include <string>
#include "character_conversion.hpp"
#include "log.hpp"

#pragma region namespace
namespace base {
namespace error {
#pragma endregion

// 用于继承
class LastError {
public:
  virtual ~LastError() {};
  std::wstring GetLastErr_Wstd() { return last_error_; };
  const wchar_t *GetLastErr_Wc() { return last_error_.c_str(); };
  std::string GetLastErr_Astd() { return base::Utf16ToGB2312(last_error_); };

protected:
  template<typename ... T1>
  void SetLastErr(const wchar_t *sz, T1 ... args) {
    last_error_ = base::log::FormatStr(sz, args...);
  }
  template<typename ... T1>
  void SetLastErr(const char *sz, T1 ... args) {
    last_error_ = base::GB2312ToUtf16(base::log::FormatStr(sz, args...));
  }

#define SetLastErrAndLog(log_str, ...) \
  SetLastErr(log_str, ##__VA_ARGS__); \
  PrintErro(log_str, ##__VA_ARGS__);

protected:
  std::wstring last_error_;
};

#pragma region namespace
}
}
#pragma endregion
