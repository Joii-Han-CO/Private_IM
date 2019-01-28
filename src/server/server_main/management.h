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

  bool InitSql();

  base::pCThreadPool pool_;
  im::config::pCConfig global_config_;
};

#pragma region
}
#pragma endregion
