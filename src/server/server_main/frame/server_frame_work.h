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

  void Uninit();

  im::config::pCConfig GetGlobalConfig();

  base::pCThreadPool GetPool();

  base::time::pTimer GetTimer();
private:

  bool InitSql();

  base::pCThreadPool pool_;
  im::config::pCConfig global_config_;
  std::weak_ptr<im::CMqttClient> wp_mqtt_;
  base::time::pTimer timer_;
};

#pragma region
}
#pragma endregion
