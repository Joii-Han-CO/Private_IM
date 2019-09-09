#pragma once
#include "base/type_def.h"
#include "im_sql_base.h"

#include <3rd_lib/sqlite3/sqlite3.h>

#pragma region
namespace im {
namespace server_sql {
#pragma endregion

enum class ESqlUserPermission {
  Admin = 0,
  User,
};

// 从数据库获取的用户信息
struct SSqlUserInfo:
  public std::enable_shared_from_this<SSqlUserInfo> {
  std::shared_ptr<SSqlUserInfo> GetPtr() {
    return shared_from_this();
  }
  std::wstring name;
  std::wstring mobile;
  std::wstring email;
  std::wstring pwd_hash;
  ESqlUserPermission permission = ESqlUserPermission::User;
};
typedef std::shared_ptr<SSqlUserInfo> pSSqlUserInfo;

// 各种服务端的数据库相关操作
class CServerSql_User: im::sql_base::CSqlBase {
public:

public:
  CServerSql_User(base::log::LogCallback log_func);
  ~CServerSql_User();

#pragma region I/R

public:
  // 初始化，异步
  bool Init(std::wstring db_path,
            std::function<void(bool, const std::wstring&)> callback);
  void Release();

private:
  // 检查数据表结构
  bool CheckSql(std::wstring &last_err);

  // 创建Sql表
  bool CreateSqlTab(std::wstring &last_err);

  // 如果现有数据库比较旧，可以更新
  bool UpdateTab();

#pragma endregion

#pragma region UserInfo
public:
  // 写入用户信息
  bool SetUserInfo(pSSqlUserInfo info);

  // 获取用户信息，通过 e-mail 或者 手机号 其中之一获取用户信息
  //  获取用户信息不会获取密码
  pSSqlUserInfo GetUserInfo(std::wstring e_mail, std::wstring mobile);

  // 校验密码
  //  先通过e-mail或者手机号获取用户信息，再去比对密码的哈希
  bool CheckPwd(std::wstring e_mail, std::wstring mobile, std::wstring pwd);

private:
  // 当输入密码的时候会自动匹配密码才能获取数据, 如果不输入则会查找用户数据, 但不会输出密码
  pSSqlUserInfo GetUserInfoBase(std::wstring e_mail, std::wstring mobile,
                                std::wstring pwd);

#pragma endregion

private:
  sqlite3 *sql_ptr_ = nullptr;
  bool is_init_sql_ = false;

};

#pragma region
}
}
#pragma endregion
