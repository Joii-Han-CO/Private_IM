#pragma once
#include "base/type_def.h"


// mysql的预定义

namespace sql {
class Connection;
class Statement;
}

namespace im {
namespace s_sql {

struct SSqlUserInfo {
  int64_t id;
  std::wstring user_name;
  int status;
  int permission;
};
StdSharedPtr_Typedef(SSqlUserInfo);

class CSqlManager {
private:
  CSqlManager();
public:
  static CSqlManager* Get();
  ~CSqlManager();

public:
  bool Init(std::wstring host,
            std::wstring name, std::wstring pwd, std::wstring db);

  bool Uninit();

  // 获取用户信息
  pSSqlUserInfo GetUserInfo(std::wstring name, std::wstring pwd);

private:
  bool is_init_ = false;

  sql::Connection *sql_conn_ = nullptr;
  sql::Statement *sql_statement_ = nullptr;

};

}
}
