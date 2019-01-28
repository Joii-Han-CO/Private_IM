#pragma once
#include "type_def.h"
#include "boost/format.hpp"

#pragma region namespace
namespace base {
namespace format {
#pragma endregion

namespace {
#define cus_format boost::basic_format<cus_char>
#define pcus_format std::shared_ptr<cus_format>

template <typename cus_char>
void IterationArgs(cus_format &fmt) {}

template <typename cus_char, typename T1, typename ...T2>
void IterationArgs(cus_format &fmt, T1 t1, T2 ...t2) {
  fmt = fmt % t1;
  IterationArgs(fmt, t2...);
}

}
template <typename cus_char, typename ... T1>
cus_string FormatStr(const cus_char *sz, T1 ...t) {
  cus_format fmt = cus_format(sz);
  IterationArgs(fmt, t...);
  return fmt.str();
};

inline std::wstring GetTemplateStr(std::string s, const wchar_t *sz) {
  return base::Utf8ToUtf16(s);
}
inline std::string GetTemplateStr(std::string s, const char *sz) {
  return s;
}

inline std::string GetStr_Utf8(const std::string &s) {
  return s;
}
inline std::string GetStr_Utf8(const std::wstring &s) {
  return base::Utf16ToUtf8(s);
}

inline std::wstring GetStr_Utf16(const std::string &s) {
  return base::Utf8ToUtf16(s);
}

inline std::wstring GetStr_Utf16(const std::wstring &s) {
  return s;
}

inline std::string GetStr_Gbk(const std::string &s) {
  return s;
}
inline std::string GetStr_Gbk(const std::wstring &s) {
  return base::Utf16ToGB2312(s);
}

template <typename cus_char>
inline void DelWrap(cus_string &str) {
  auto fun_wrap = [&str](cus_char c) {
    int f1;
    while (true) {
      f1 = str.find(c);
      if (f1 == str.npos)
        return;
      str = str.substr(0, f1) + str.substr(f1 + 1, str.size() - f1 - 1);
    }
  };

  fun_wrap('\r');
  fun_wrap('\n');
}

#pragma region namespace
}
}
#pragma endregion
