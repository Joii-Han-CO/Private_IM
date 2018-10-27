#pragma once
#include "base/type_def.h"
#include <string>
#include <memory>
#include <fstream>


#pragma region
namespace im {
namespace log {
#pragma endregion

// 初始化参数
struct SLog_InitArgs {
  std::wstring log_path;

  bool print_dbg = false;
  bool print_info = false;
  bool print_warn = true;
  bool print_erro = true;

};
typedef std::shared_ptr<SLog_InitArgs> pSLog_InitArgs;

// 单例的Log
class CLog {
private:
  CLog();
public:
  ~CLog();
  static CLog* Get();

  bool Init(SLog_InitArgs *args = nullptr);

  // 输出到Log中
  void OutPut(const wchar_t *d);
  void OutPut(const char *d);

private:

  bool is_init_ = false;
  std::shared_ptr<std::ifstream> file_;
};

#pragma region
}
}
#pragma endregion
