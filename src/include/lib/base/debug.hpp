#pragma once
#include <string>
#include <mutex>
#include <iostream>
#include "log.hpp"
#include "character_conversion.hpp"

namespace base {
namespace debug {

inline std::mutex &GetGlobalOutputSync() {
  static std::mutex output_sync;
  return output_sync;
}

template<typename ... T1>
inline void OutPut(const char *sz, T1 ... args) {
  std::unique_lock<std::mutex> lock(GetGlobalOutputSync());
  auto d = base::format::FormatStr(sz, args...);
  std::cout << d.c_str() << std::endl;
}

template<typename ... T1>
inline void OutPut(const wchar_t *sz, T1 ... args) {
  std::unique_lock<std::mutex> lock(GetGlobalOutputSync());
  auto d = base::format::FormatStr(sz, args...);
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

}
}
