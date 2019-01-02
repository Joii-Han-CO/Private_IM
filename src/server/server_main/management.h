#pragma once


#pragma region
namespace server {
#pragma endregion

// 单例
class CManagement {
public:
  CManagement();
public:
  static CManagement* Get();
  ~CManagement();

  bool Init();

  im::config::pCConfig GetGlobalConfig();

private:
  base::pCThreadPool pool_;
  im::config::pCConfig global_config_;
};


/*
server_global.config

[mqtt_server]
host = 127.0.0.1
port = 1234
admin_name = admin
admin_pwd = 123456

*/

#pragma region
}
#pragma endregion
