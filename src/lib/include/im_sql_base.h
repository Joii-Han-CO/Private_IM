#pragma once
#include "base/type_def.h"
#include "base/error.hpp"
#include "base/log.hpp"
#include "base/task.hpp"


#pragma region
namespace im {
namespace sql_base {
#pragma endregion

// 描述：主要为了处理Sql的基类
//  错误、日志的处理
//  添加任务

class CSqlBase:
  public base::error::LastError,
  public base::log::Log,
  public base::task::Task {

public:
  CSqlBase(base::log::LogCallback log_func);
  ~CSqlBase();

};

#pragma region
}
}
#pragma endregion
