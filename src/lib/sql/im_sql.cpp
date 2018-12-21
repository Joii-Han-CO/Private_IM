#include "stdafx.h"
#include "im_sql.h"

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

bool CSqlBase::SqlExec(sqlite3 *ptr, const std::string &sql,
                       int *rc_code, std::string &err_msg) {
  char *c_err_msg = nullptr;
  int rc = sqlite3_exec(ptr, sql.c_str(), nullptr, nullptr, &c_err_msg);
  if (rc != SQLITE_OK) {
    if (rc_code)
      *rc_code = rc;
    if (c_err_msg)
      err_msg = c_err_msg;
    return false;
  }
  if (rc_code)
    *rc_code = 0;
  err_msg.clear();
  return true;
}

#pragma region namespace
}
}
#pragma endregion
