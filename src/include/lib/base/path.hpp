#pragma once
#include "type_def.h"
#include "character_conversion.hpp"
#ifdef WIN32
#include <Windows.h>
#else
#include <mach-o/dyld.h>
#endif


#pragma region namespace
namespace base {
namespace _path {
#pragma endregion

namespace {

#ifdef WIN32
bool GetCurExePath(std::string &ref_str) {
  char path[MAX_PATH + 1] = {0};
  if (GetModuleFileNameA(NULL, path, MAX_PATH) != 0) {
    ref_str = path;
    return true;
  }
  return false;
}
#else
bool GetCurExePath(std::string &ref_str) {
  char name[512];
  uint32_t size = 512;
  _NSGetExecutablePath(name, &size);
  
  ref_str = name;
  return true;
}

#endif

inline bool GetCurExePath(std::wstring &ref_str) {
  std::string u;
  auto ref = GetCurExePath(u);
  if (ref == false)
    return false;
  ref_str = base::GB2312ToUtf16(u);
  return true;
}
}

template <typename cus_char> cus_string GetExePath() {
  cus_string path;
  if (GetCurExePath(path) == false)
    return cus_string();

  return path;
}

template <typename cus_char> cus_string GetExeDir() {
  auto path = GetExePath<cus_char>();

  auto i1 = path.rfind('\\');
  if (i1 == std::string::npos)
    i1 = path.rfind('/');
  if (i1 == std::string::npos)
    return cus_string();
  return path.substr(0, i1++);
}

#pragma region namespace
}
}
#pragma endregion
