#pragma once
#include "base/type_def.h"
#include <string>
#include <memory>
#include <fstream>

#include "base/task.hpp"
#include "base/log.hpp"


#pragma region
namespace im {
namespace log {
#pragma endregion

// 初始化参数
struct SLog_InitArgs {
  std::wstring log_path;

  // TODO
  bool print_dbg = false;
  bool print_info = false;
  bool print_warn = true;
  bool print_erro = true;

};
typedef std::shared_ptr<SLog_InitArgs> pSLog_InitArgs;

// 单例的Log
class CLog: public base::b_async::Task {
private:
  CLog();
public:
  ~CLog();
  static CLog* Get();

  // 全局的初始化
  bool Init(SLog_InitArgs *args = nullptr);

  // 模板只能写在这里
  template <typename cus_char, typename ... T1>
  void Print(base::log::EBaseLogType t,
             const char *source_file, const char *func_name, int line_num,
             const cus_char *sz, T1 ... args) {
    if (sz == nullptr || sz[0] == '\0')
      return;

    // 格式化变量...
    auto body_str = base::log::FormatStr(sz, args ...);
    if (body_str.empty())
      return;

    auto header_str = MakeHeader(t, source_file, func_name, line_num);
    auto header_str2 = GetTemplateStr(header_str, sz);

    auto log_str = base::log::FormatStr(header_str2.c_str(), body_str.c_str());

    OutPutBase(log_str);
  }

private:
  void OutPutBase(const std::wstring &d);
  void OutPutBase(const std::string &d);

  std::string MakeHeader(base::log::EBaseLogType t,
                         const char *source_file,
                         const char *func_name, int line_num);

  bool WriteLog(base::log::EBaseLogType t);

  void PrintHeader();

  std::wstring GetTemplateStr(std::string s, const wchar_t *sz);
  std::string GetTemplateStr(std::string s, const char *sz);

  bool is_init_ = false;
  std::shared_ptr<std::ofstream> file_;

  bool print_dbg_ = false;
  bool print_info_ = false;
  bool print_warn_ = true;
  bool print_erro_ = true;
};

#define _PrintBase(type, sz, ...) \
  im::log::CLog::Get()->Print(type, \
                              __FILE__, __FUNCTION__, __LINE__, \
                              sz, ##__VA_ARGS__);
#define PrintBaseDbg(sz, ...) \
  _PrintBase(base::log::EBaseLogType::dbg, sz, ##__VA_ARGS__)
#define PrintBaseInfo(sz, ...) \
  _PrintBase(base::log::EBaseLogType::info, sz, ##__VA_ARGS__)
#define PrintBaseWarn(sz, ...) \
  _PrintBase(base::log::EBaseLogType::warn, sz, ##__VA_ARGS__)
#define PrintBaseErro(sz, ...) \
  _PrintBase(base::log::EBaseLogType::erro, sz, ##__VA_ARGS__)

#pragma region
}
}
#pragma endregion
