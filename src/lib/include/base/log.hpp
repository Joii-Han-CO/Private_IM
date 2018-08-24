#pragma once
#include <string>
#include <functional>
#include "character_conversion.hpp"

#pragma region namespace
namespace base {
#pragma endregion

#pragma region Format

template<typename ... T1>
inline std::string FormatStr(const char *sz, T1 ... args) {
  if (sz == nullptr || sz[0] == '\0')
    return "";
  std::string rf;
  auto bufsize = snprintf(nullptr, 0, sz, args...);
  if (bufsize == 0)
    return "";
  rf.resize(bufsize + 1);
  snprintf((char*)rf.c_str(), rf.size(), sz, args...);
  return rf;
};

#define LOG_BASE_IS_SAME(T) std::is_same<T1, T>::value

inline void CountAgrsSizeC(int &size, std::string arg) {
  size += arg.size();
};

template<typename ... T1>
inline std::wstring FormatStr(const wchar_t *sz, T1 ... args) {
  if (sz == nullptr || sz[0] == '\0')
    return L"";
  std::wstring rf;
  const int black_size = 256;
  for (int i = 1; i <= 16; i++) {
    rf.resize(i * black_size);
    if (swprintf((wchar_t*)rf.c_str(), rf.size(), sz, args...) != -1)
      return rf;
  }
  return L"";
};

#pragma endregion

enum class EBaseLogType {
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

typedef std::function<void(const SBaseLog &func)> LogCallback;

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

protected:
#pragma region PrintLog

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
#define PrintLog(type, log, ...) \
  if (func_) \
    LogPrintLog(type, __FUNCTION__, __FILE__, __LINE__, \
      base::FormatStr(log, ##__VA_ARGS__).c_str());
#define PrintInfo(log, ...) \
  PrintLog(base::EBaseLogType::info, log, ##__VA_ARGS__)
#define PrintWarn(log, ...) \
  PrintLog(base::EBaseLogType::warn, log, ##__VA_ARGS__)
#define PrintErro(log, ...) \
  PrintLog(base::EBaseLogType::erro, log, ##__VA_ARGS__)

#pragma endregion

protected:
  LogCallback func_;
};

#pragma region namespace
}
#pragma endregion
