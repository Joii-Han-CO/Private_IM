#include "stdafx.h"
#include "im_server_sql.h"

#pragma region namespace
namespace im {
namespace server_sql {
#pragma endregion

CServerSql_User::CServerSql_User(base::log::LogCallback log_func):
  CSqlBase(log_func) {}

CServerSql_User::~CServerSql_User() {}

#pragma region I/R

bool CServerSql_User::Init(
  std::wstring db_path,
  std::function<void(bool, const std::wstring&)> callback) {
  if (is_init_sql_) {
    SetLastErr("sql already initialized");
    return false;
  }
  if (db_path.empty()) {
    SetLastErr("sql db path is empty.");
    return false;
  }

  auto f = [this, db_path, callback] () {
    std::wstring last_err;
    auto re_val = [callback, &last_err] (bool suc) {
      if (callback)
        callback(suc, last_err);
    };
    int rc = sqlite3_open16((const void*)db_path.c_str(), &sql_ptr_);
    if (rc != SQLITE_OK || sql_ptr_ == nullptr) {
      const char *err_msg = sqlite3_errmsg(sql_ptr_);
      last_err = base::Utf8ToUtf16(base::log::FormatStr(
        "open sqlite db file failed, rc_code:%d, sqlite ptr:%X, des:%s",
        rc, sql_ptr_, err_msg));
      if (sql_ptr_)
        sqlite3_close(sql_ptr_);
      re_val(false);
    }

    if (CheckSql(last_err) == false) {
      re_val(false);
    }

    is_init_sql_ = true;
    re_val(true);
  };
  AddTask(f);
  return true;
}

void CServerSql_User::Release() {
  // 异步调用Release，这样可以防止冲突
  base::b_async::Condition wait;
  auto func = [&wait, this] () {
    if (sql_ptr_)
      sqlite3_close(sql_ptr_);
    is_init_sql_ = false;
    wait.NotifyOne();
  };
  AddTask(func);

  wait.Wait();
}

bool CServerSql_User::CheckSql(std::wstring &last_err) {
  char **sql_res = nullptr;
  int row = 0, col = 0;
  char *err_msg = nullptr;
  int rc = sqlite3_get_table(sql_ptr_, "select * from sqlite_master",
                             &sql_res, &row, &col, &err_msg);
  if (rc != SQLITE_OK) {
    last_err = base::Utf8ToUtf16(base::log::FormatStr(
      "get sqlite table failed, rc_code:%d, des:%s", rc, err_msg));
    return false;
  }
  if (row == 0 && col == 0) {
    if (CreateSqlTab(last_err) == false)
      return false;
  }

  if (sql_res)
    sqlite3_free_table(sql_res);
  return true;
}

bool CServerSql_User::CreateSqlTab(std::wstring &last_err) {
  std::string err_msg;
  int rc_code = 0;

  std::string sql =
    "create table user_info(\n"
    "  id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
    "  name text,\n"
    "  mobile text,\n"
    "  email text,\n"
    "  pwd_hash text,\n"
    "  permission int\n"
    ");";
  if (SqlExec(sql_ptr_, sql, &rc_code, err_msg) == false) {
    last_err = base::Utf8ToUtf16(base::log::FormatStr(
      "create table user_info failed, rc_code:%d, des:%s",
      rc_code, err_msg.c_str()));
    return false;
  }

  return true;
}

bool CServerSql_User::UpdateTab() {
  return true;
}

#pragma endregion

#pragma region UserInfo

bool CServerSql_User::SetUserInfo(pSSqlUserInfo info) {
  if (info->email.empty() && info->mobile.empty()) {
    SetLastErr("e-mail and mobile at least one is not empty");
    return false;
  }
  if (info->pwd_hash.empty()) {
    SetLastErr("password is empty");
    return false;
  }

  // 写入数据库



  return true;
}

pSSqlUserInfo CServerSql_User::GetUserInfo(std::wstring e_mail,
                                           std::wstring mobile) {
  return GetUserInfoBase(e_mail, mobile, std::wstring());
}

bool CServerSql_User::CheckPwd(std::wstring e_mail, std::wstring mobile,
                               std::wstring pwd) {
  auto info = GetUserInfoBase(e_mail, mobile, pwd);
  if (info == nullptr)
    return false;
  return true;
}

pSSqlUserInfo CServerSql_User::GetUserInfoBase(std::wstring e_mail,
                                               std::wstring mobile,
                                               std::wstring pwd) {
  if (e_mail.empty() || mobile.empty()) {
    SetLastErr("e-mail and mobile at least one is not empty");
    return nullptr;
  }

  return pSSqlUserInfo();
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
