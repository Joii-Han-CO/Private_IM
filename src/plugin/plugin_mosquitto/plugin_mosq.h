#pragma once
#include <mosquitto_plugin.h>
#include <memory>

#include "include/server/im_server_sql.h"


#pragma region
namespace im {
namespace plugin {
#pragma endregion

class CPluginMosq {
public:
  static int GetVersion();

public:
  CPluginMosq();
  bool Init();
  void Release();

  bool InitSecurity();
  void ReleaseSecurity();

  // 用户登陆
  bool UserLogin(const struct mosquitto *client,
                 const char *username,
                 const char *password);

  // 用户订阅消息
  bool UserSubscribe(int access,
                     const struct mosquitto *client,
                     const struct mosquitto_acl_msg *msg);

private:
  void OnLog(const base::log::SBaseLog &func);

private:
  im::server_sql::CServerSql_User sql_user_;

};
typedef CPluginMosq* pCPluginMosq;

#pragma region
}
}
#pragma endregion
