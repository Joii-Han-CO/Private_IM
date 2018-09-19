#include "stdafx.h"
#include "im_sql_base.h"

#pragma region namespace
namespace im {
namespace sql_base {
#pragma endregion

CSqlBase::CSqlBase(base::log::LogCallback log_func):
  Log(log_func) {
  StartTask();
}

CSqlBase::~CSqlBase() {
  StopTask();
  WaitTask();
}

#pragma region namespace
}
}
#pragma endregion
