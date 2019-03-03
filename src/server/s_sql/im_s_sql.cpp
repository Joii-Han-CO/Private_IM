#include "pch.h"
#include "im_s_sql.h"


namespace im {
namespace s_sql {

CSqlManager::CSqlManager() {}

CSqlManager* CSqlManager::Get() {
  static CSqlManager s;
  return &s;
}

CSqlManager::~CSqlManager() {}

bool CSqlManager::Init(std::wstring host,
                       std::wstring name, std::wstring pwd,
                       std::wstring db) {
  sql::Driver *sql_driver = nullptr;

  if (is_init_ == true)
    return true;
  try {
    sql_driver = get_driver_instance();
  }
  catch (...) {
    PrintLogErro("init mysql failed, get_driver_instance");
    return false;
  }
  auto str_host = base::Utf16ToUtf8(host);
  auto str_name = base::Utf16ToUtf8(name);
  auto str_pwd = base::Utf16ToUtf8(pwd);
  auto str_db = base::Utf16ToUtf8(db);

  sql_conn_ = sql_driver->connect(str_host.c_str(),
                                  str_name.c_str(), str_pwd.c_str());
  sql_conn_->setSchema(str_db.c_str());
  sql_statement_ = sql_conn_->createStatement();

  is_init_ = true;
  return true;
}

bool CSqlManager::Uninit() {
  if (is_init_ == false)
    return true;
  sql_conn_->close();
  delete sql_statement_;
  delete sql_conn_;

  sql_statement_ = nullptr;
  sql_conn_ = nullptr;

  is_init_ = false;
  return true;
}


// 获取用户信息
pSSqlUserInfo CSqlManager::GetUserInfo(std::wstring name,
                                       std::wstring pwd) {
  auto str_name = base::Utf16ToUtf8(name);
  auto str_pwd = base::Utf16ToUtf8(pwd);

  std::string sql_exe =
    "select id,name,status,permission from pim_user where name='" +
    str_name + "' and pwd='" + str_pwd + "';";
  auto res = sql_statement_->executeQuery(sql_exe.c_str());
  PrintLogDbg("get user info, sql:%s", sql_exe.c_str());
  if (res == nullptr) {
    PrintLogErro("get user ref info is null");
    return nullptr;
  }
  if (res->next() == false) {
    return nullptr;
  }

  auto info = std::make_shared<SSqlUserInfo>();

  info->id = res->getInt64("id");
  info->user_name = base::Utf8ToUtf16(res->getString("name").c_str());
  info->status = res->getInt("status");
  info->permission = res->getInt("permission");

  res->close();
  delete res;

  return info;
}

bool InitSql() {
  return false;
}

bool UninitSql() {
  return false;
}

}
}
