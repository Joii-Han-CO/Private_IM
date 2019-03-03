#pragma once
#include <string>
#include <functional>
#include "character_conversion.hpp"
#include "format_str.hpp"

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32


namespace base {
namespace log {


#define LOG_BASE_IS_SAME(T) std::is_same<T1, T>::value

inline void CountAgrsSizeC(int &size, std::string arg) {
  size += arg.size();
};

enum class EBaseLogType {
  dbg,
  info,
  warn,
  erro,
};

// Log info
struct SBaseLog {
  EBaseLogType type;
  const char *func = nullptr;
  const char *file = nullptr;
  int line = 0;
  const wchar_t *log = nullptr;
};

inline const char *GetBaseLogTypeStr(EBaseLogType t) {
  switch (t) {
  case base::log::EBaseLogType::dbg:
    return "_dbg";
  case base::log::EBaseLogType::info:
    return "info";
  case base::log::EBaseLogType::warn:
    return "warn";
  case base::log::EBaseLogType::erro:
    return "erro";
  default:
    return "";
  }
}

typedef std::function<void(const SBaseLog &)> LogCallback;

class Log {
public:
  Log(LogCallback func):
    func_(func) {};
  virtual ~Log() {};

  static const char* FormatTypeA(EBaseLogType t) {
    if (t == EBaseLogType::info)
      return "info";
    else if (t == EBaseLogType::warn)
      return "warn";
    else if (t == EBaseLogType::erro)
      return "erro";
    else
      return "";
  }
  static const wchar_t* FormatTypeW(EBaseLogType t) {
    if (t == EBaseLogType::info)
      return L"info";
    else if (t == EBaseLogType::warn)
      return L"warn";
    else if (t == EBaseLogType::erro)
      return L"erro";
    else
      return L"";
  }


#define VirtualPrintLogFunc(FuncName) \
protected: \
  void VPrintLogFunc(const base::log::SBaseLog &func) { return FuncName(func); };

#define BindVirtualLog(class_name) \
  std::bind(&class_name::VPrintLogFunc, this, std::placeholders::_1)

protected:

  void LogPrintLog(EBaseLogType type,
                   const char *func, const char *file, int line,
                   const wchar_t *log = nullptr) {
    SBaseLog l;
    l.type = type;
    l.func = func;
    l.file = file;
    l.line = line;
    l.log = log;
    func_(l);
  };
  void LogPrintLog(EBaseLogType type,
                   const char *func, const char *file, int line,
                   const char *log = nullptr) {
    SBaseLog l;
    l.type = type;
    l.func = func;
    l.file = file;
    l.line = line;
    auto str_log = base::GB2312ToUtf16(log);
    l.log = str_log.c_str();
    func_(l);
  };
#define PrintLog(type, log_str, ...) \
  if (func_) \
    LogPrintLog(type, __FUNCTION__, __FILE__, __LINE__, \
      base::format::FormatStr(log_str, ##__VA_ARGS__).c_str());
#define MPrintInfo(log_str, ...) \
  PrintLog(base::log::EBaseLogType::info, log_str, ##__VA_ARGS__)
#define MPrintWarn(log_str, ...) \
  PrintLog(base::log::EBaseLogType::warn, log_str, ##__VA_ARGS__)
#define MPrintErro(log_str, ...) \
  PrintLog(base::log::EBaseLogType::erro, log_str, ##__VA_ARGS__)

protected:
  LogCallback func_;
};

inline void PrintModleHeader() {
  printf("--------%s--------\n", PRJ_NAME);
}

#ifdef _WIN32
inline std::wstring GetMqttErrorDesW() {
  const int err_max_size = 1024;
  std::wstring err;
  err.resize(err_max_size);
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errno,
                0, (LPTSTR)err.c_str(), err_max_size, NULL);
  base::format::DelWrap(err);

  return err.c_str();
}
#else
inline std::wstring GetMqttErrorDesW() {
  return std::wstring();
}
#endif

}
}
