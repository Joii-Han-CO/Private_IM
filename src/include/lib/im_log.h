#pragma once
#include "base/type_def.h"
#include <string>
#include <memory>
#include <fstream>
#include <mutex>

#include "base/task.hpp"
#include "base/log.hpp"
#include "base/format_str.hpp"

// 同步写标示
//  1 异步写
//  0 同步写
#define LogASyncWrite 1

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
  bool output_ctrl = false;   // 输出到控制台
  bool save_file = true;    // 保存文件
};
typedef std::shared_ptr<SLog_InitArgs> pSLog_InitArgs;

// 单例的Log
class CLog
#if LogASyncWrite
  : public base::b_async::Task
#endif
{
private:
  CLog();
public:
  ~CLog();
  static CLog* Get();

  // 全局的初始化
  bool Init(SLog_InitArgs *args = nullptr);

  // 模板只能写在这里
  template <typename cus_char, typename ... T1>
  void Print(base::log::EBaseLogType t, const char *prj_name,
             const char *source_file, const char *func_name, int line_num,
             const cus_char *sz, T1 ... args) {
    if (sz == nullptr || sz[0] == '\0')
      return;
    if (!FilterType(t))
      return;

    std::string header_str = MakeHeader(t, prj_name, source_file,
                                        func_name, line_num);

    // 格式化变量...
    auto body_str = base::format::FormatStr(sz, args ...);
    if (body_str.empty())
      return;

#if LogASyncWrite
    AddTask([this, header_str, body_str]() {
#endif
      std::string log_str =
        header_str + "]:" + base::format::GetStr_Utf8(body_str) + "\n";

      OutPutBase(log_str);
#if LogASyncWrite
    });
#endif
  }

private:
  bool FilterType(base::log::EBaseLogType t);
  void OutPutBase(const std::string &d);

  std::string MakeHeader(base::log::EBaseLogType t,
                         const char *prj_name, const char *source_file,
                         const char *func_name, int line_num);

  bool WriteLog(base::log::EBaseLogType t);

  void PrintHeader();

  bool is_init_ = false;
  std::shared_ptr<std::ofstream> file_;

  bool print_dbg_ = false;
  bool print_info_ = false;
  bool print_warn_ = true;
  bool print_erro_ = true;

  bool print_ctrl_ = false;

#if !LogASyncWrite
  std::mutex write_log_sync_;
#endif

};

#define _PrintBase(type, sz, ...) \
  im::log::CLog::Get()->Print(type, PRJ_NAME,\
                              __FILE__, __FUNCTION__, __LINE__, \
                              sz, ##__VA_ARGS__);
#define PrintLogDbg(sz, ...) \
  _PrintBase(base::log::EBaseLogType::dbg, sz, ##__VA_ARGS__)
#define PrintLogInfo(sz, ...) \
  _PrintBase(base::log::EBaseLogType::info, sz, ##__VA_ARGS__)
#define PrintLogWarn(sz, ...) \
  _PrintBase(base::log::EBaseLogType::warn, sz, ##__VA_ARGS__)
#define PrintLogErro(sz, ...) \
  _PrintBase(base::log::EBaseLogType::erro, sz, ##__VA_ARGS__)

// 测试中，直接调用该函数
inline void InitTestLog() {
  im::log::SLog_InitArgs log_args;
  log_args.print_dbg = true;
  log_args.print_info = true;
  log_args.print_warn = true;
  log_args.print_erro = true;

  log_args.output_ctrl = true;

  im::log::CLog::Get()->Init(&log_args);
}

#pragma region
}
}
#pragma endregion
