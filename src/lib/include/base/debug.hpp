#pragma once
#include <string>
#include <mutex>
#include "log.hpp"

#pragma region namespace
namespace base {
namespace debug {
#pragma endregion

#pragma region OutputInfo

std::mutex g_output_sync_;

template<typename ... T1>
inline void OutPut(const char *sz, T1 ... args) {
  std::unique_lock<std::mutex> lock(g_output_sync_);
  auto d = base::FormatStr(sz, args...);
  std::cout << d.c_str() << std::endl;
}

template<typename ... T1>
inline void OutPut(const wchar_t *sz, T1 ... args) {
  std::unique_lock<std::mutex> lock(g_output_sync_);
  auto d = base::FormatStr(sz, args...);
  std::cout << d.c_str() << std::endl;
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
