#pragma once
#include <map>
#include <locale>
#include <string>
#include <array>
#include <vector>
#include <codecvt>

#include "type_def.h"

namespace base {

// TODO 执行错误，这里需要判断...
inline void Exception_Throw() {
  // 抛出异常
  //  TODO 默认还是不开启了，方便调试
#ifdef _DEBUG
  //throw;
#endif
}

// win32
#define GBK_LOCALE_NAME ".936"

// des：  utf8-->utf16
inline bool Utf8ToUtf16(const std::string& utf8Text,
                        std::wstring& unicodeText) {
  bool ref = false;
  std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
  try {
    unicodeText = cvt.from_bytes(utf8Text);
    ref = true;
  }
  catch (std::range_error) {
    Exception_Throw();
  }
  catch (...) {
    Exception_Throw();
  }
  return ref;
};
inline std::wstring Utf8ToUtf16(const std::string& utf8Text) {
  if (utf8Text.empty())
    return std::wstring();
  std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
  try {
    return cvt.from_bytes(utf8Text);
  }
  catch (std::range_error) {
    Exception_Throw();
    // 输出错误原因
  }
  catch (...) {
    Exception_Throw();
  }
  return std::wstring();
}

// des：  utf16-->utf8
inline bool Utf16ToUtf8(const std::wstring& unicodeText,
                        std::string& utf8Text) {
  bool ref = false;
  std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
  try {
    utf8Text = cvt.to_bytes(unicodeText);
    ref = true;
  }
  catch (std::range_error) {
    Exception_Throw();
  }
  catch (...) {
    Exception_Throw();
  }
  return ref;
};
inline std::string Utf16ToUtf8(const std::wstring& unicodeText) {
  if (unicodeText.empty())
    return std::string();
  std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
  try {
    return cvt.to_bytes(unicodeText);
  }
  catch (std::range_error) {
    Exception_Throw();
  }
  catch (...) {
    Exception_Throw();
  }
  return std::string();
}

class codecvt_gbk: public std::codecvt_byname<wchar_t, char, mbstate_t> {
public:
  codecvt_gbk():
    std::codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME) {};
  ~codecvt_gbk() {};
};

// des：  gb2312-->utf16
inline bool GB2312ToUtf16(const std::string& multiText,
                          std::wstring& unicodeText) {
#ifndef WIN32
  return Utf8ToUtf16(multiText, unicodeText);
#endif

  bool ref = false;
  codecvt_gbk c_gbk;
  std::wstring_convert<codecvt_gbk>
    cvt(&c_gbk);
  try {
    unicodeText = cvt.from_bytes(multiText);
    ref = true;
  }
  catch (std::range_error) {
    Exception_Throw();
  }
  catch (...) {
    Exception_Throw();
  }
  return ref;
};
inline std::wstring GB2312ToUtf16(const std::string& multiText) {
#ifndef WIN32
  return Utf8ToUtf16(multiText);
#endif

  if (multiText.empty())
    return std::wstring();
  std::wstring_convert<codecvt_gbk> cvt(new codecvt_gbk());
  try {
    auto ref_str = cvt.from_bytes(multiText);
    return ref_str;
  }
  catch (std::range_error) {
    Exception_Throw();
  }
  catch (...) {
    Exception_Throw();
  }
  return std::wstring();
}

// des：  utf16-->gb2312
inline bool Utf16ToGB2312(const std::wstring& unicodeText,
                          std::string& multiText) {
#ifndef WIN32
  return Utf16ToUtf8(unicodeText, multiText);
#endif

  bool ref = false;
  std::wstring_convert<codecvt_gbk> cvt(new codecvt_gbk());
  try {
    multiText = cvt.to_bytes(unicodeText);
    ref = true;
  }
  catch (std::range_error) {
    Exception_Throw();
  }
  catch (...) {
    Exception_Throw();
  }
  return ref;
};
inline std::string Utf16ToGB2312(const std::wstring& unicodeText) {
#ifndef WIN32
  return Utf16ToUtf8(unicodeText);
#endif

  if (unicodeText.empty())
    return std::string();
  std::wstring_convert<codecvt_gbk> cvt(new codecvt_gbk());
  try {
    return cvt.to_bytes(unicodeText);
  }
  catch (std::range_error) {
    Exception_Throw();
  }
  catch (...) {
    Exception_Throw();
  }
  return std::string();
};


}
