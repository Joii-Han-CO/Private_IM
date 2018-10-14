#pragma once
#include <string>
#include <mutex>
#include <iostream>
#include "log.hpp"
#include "character_conversion.hpp"

#pragma region namespace
namespace base {
namespace debug {
#pragma endregion

#pragma region OutputInfo

std::mutex g_output_sync_;

template<typename ... T1>
inline void OutPut(const char *sz, T1 ... args) {
  std::unique_lock<std::mutex> lock(g_output_sync_);
  auto d = base::log::FormatStr(sz, args...);
  std::cout << d.c_str() << std::endl;
}

template<typename ... T1>
inline void OutPut(const wchar_t *sz, T1 ... args) {
  std::unique_lock<std::mutex> lock(g_output_sync_);
  auto d = base::log::FormatStr(sz, args...);
  std::wcout << d.c_str() << std::endl;
}

inline void OutputLogInfo(const base::log::SBaseLog &l) {
  std::string str_type = base::log::Log::FormatTypeA(l.type);
  std::string str_func;
  if (l.func) str_func = l.func;
  std::string str_log;
  if (l.log) str_log = base::Utf16ToGB2312(l.log);

  OutPut("[Log]-%s--%s:%d---%s",
         str_type.c_str(), str_func.c_str(), l.line, str_log.c_str());
}

inline void WaitEnterGoon(const char *d) {
  OutPut(d);
  getchar();
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
