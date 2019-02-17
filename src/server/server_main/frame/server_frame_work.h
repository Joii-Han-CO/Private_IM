#pragma once

#pragma region
namespace server {
#pragma endregion

// 单例
class CSFarmeWork {
public:
  CSFarmeWork();
public:
  static CSFarmeWork* Get();
  ~CSFarmeWork();

  bool Init();

  im::config::pCConfig GetGlobalConfig();

  base::pCThreadPool GetPool();

private:

  bool InitSql();

  base::pCThreadPool pool_;
  im::config::pCConfig global_config_;
};

#pragma region
}
#pragma endregion
