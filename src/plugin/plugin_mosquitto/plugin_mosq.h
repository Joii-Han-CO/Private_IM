#pragma once
#include <mosquitto_plugin.h>
#include <memory>

#pragma region
namespace im {
namespace plugin {
#pragma endregion

class CPluginMosq: public std::enable_shared_from_this<CPluginMosq> {
public:
  static int GetVersion();

public:
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

};
typedef CPluginMosq* pCPluginMosq;


#pragma region
}
}
#pragma endregion
