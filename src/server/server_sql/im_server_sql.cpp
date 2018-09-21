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

bool CServerSql_User::SetUserInfo(
  pSSqlUserInfo info,
  std::function<void(bool suc, std::wstring err)> callback) {

  if (info->email.empty() && info->mobile.empty()) {
    SetLastErr("e-mail and mobile at least one is not empty");
    return false;
  }
  if (info->pwd_hash.empty()) {
    SetLastErr("password is empty");
    return false;
  }

  // 异步...
  auto func = [info, callback, this] () {
    auto e_info = GetUserInfoBase(info->email, info->mobile, L"");
    if (e_info != nullptr) {
      if (callback)
        callback(false, L"This user already exists");
      return;
    }

    // 写入数据库
    std::wstring sql = base::log::FormatStr(
      L"insert into 'user_info' ('name', 'mobile', 'email', 'pwd_hash', 'permission') "
      L"values('%s', '%s', '%s', '%s', %d);",
      info->name.c_str(), info->mobile.c_str(),
      info->email.c_str(), info->pwd_hash.c_str(), (int)info->permission);
    int rc_code = 0;
    std::string err_msg;
    SqlExec(sql_ptr_, base::Utf16ToUtf8(sql), &rc_code, err_msg);
    if (rc_code != SQLITE_OK) {
      auto last_err = base::Utf8ToUtf16(base::log::FormatStr(
        "insert date failed, rc_code:%d, des:%s",
        rc_code, err_msg.c_str()));
      if (callback)
        callback(false, last_err);
      return;
    }
    if (callback)
      callback(true, L"");
  };
  AddTask(func);
  return true;
}

pSSqlUserInfo CServerSql_User::GetUserInfo(std::wstring e_mail,
                                           std::wstring mobile) {
  return GetUserInfoBase(e_mail, mobile, std::wstring());
}

pSSqlUserInfo CServerSql_User::GetUserInfo(std::wstring e_mail,
                                           std::wstring mobile,
                                           std::wstring pwd) {
  return GetUserInfoBase(e_mail, mobile, pwd);
}

pSSqlUserInfo CServerSql_User::GetUserInfoBase(std::wstring e_mail,
                                               std::wstring mobile,
                                               std::wstring pwd) {
  if (e_mail.empty() && mobile.empty()) {
    SetLastErr("e-mail and mobile at least one is not empty");
    return nullptr;
  }

  std::string u8_e_mail = base::Utf16ToUtf8(e_mail);
  std::string u8_mobile = base::Utf16ToUtf8(mobile);
  std::string u8_pwd = base::Utf16ToUtf8(pwd);
  std::string sql =
    "select name,email,mobile,pwd_hash,permission from user_info where ";
  if (!u8_mobile.empty() && !u8_e_mail.empty()) {
    sql += base::log::FormatStr("email='%s' and mobile='%s' limit 2;",
                                u8_e_mail.c_str(), u8_mobile.c_str());
  }
  else if (u8_mobile.empty() && !u8_e_mail.empty()) {
    sql += base::log::FormatStr("email='%s'  limit 1;",
                                u8_e_mail.c_str());
  }
  else if (!u8_mobile.empty() && u8_e_mail.empty()) {
    sql += base::log::FormatStr("mobile='%s'  limit 1;",
                                u8_mobile.c_str());
  }

  // 执行查找命令，因为吐出的数据比较少...
  char **res_ptr = nullptr;
  int row = 0, col = 0;
  char *err_msg = nullptr;
  int rc = sqlite3_get_table(sql_ptr_, sql.c_str(),
                             &res_ptr, &row, &col, &err_msg);
  if (rc != SQLITE_OK) {
    SetLastErr("run get table failed, rc_code:%d, des:%s", rc, err_msg);
    return nullptr;
  }
  if (col != 5 || row != 1) {
    SetLastErr("empty");
    return nullptr;
  }

  std::string o_name, o_email, o_mobile, o_pwd;

  // 校验密码
  if (res_ptr[3 + col])
    o_pwd = res_ptr[3 + col];
  if (!u8_pwd.empty()) {
    if (o_pwd != u8_pwd) {
      SetLastErr("password wrong");
      return nullptr;
    }
  }
  auto ref_info = std::make_shared<SSqlUserInfo>();
  if (res_ptr[0 + col])
    ref_info->name = base::Utf8ToUtf16(res_ptr[0 + col]);
  if (res_ptr[1 + col])
    ref_info->email = base::Utf8ToUtf16(res_ptr[1 + col]);
  if (res_ptr[2 + col])
    ref_info->mobile = base::Utf8ToUtf16(res_ptr[2 + col]);
  if (res_ptr[4 + col])
    ref_info->permission = (ESqlUserPermission)atoi(res_ptr[4 + col]);

  sqlite3_free_table(res_ptr);

  return ref_info;
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
