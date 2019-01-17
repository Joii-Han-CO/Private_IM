#pragma once
#include "base/type_def.h"
#include "im_c_login.h"
#include "im_config.h"

#pragma region namespace
namespace im {
namespace c_framework {
#pragma endregion

class ClientFramework {
private:
  ClientFramework();
public:
  ~ClientFramework();
  static ClientFramework *Get();

  // 客户端全局的初始化
  bool Init();

  im::config::pCConfig GetGlobalConfigPtr();
  im::c_login::pClientLogin GetLoginPtr();

private:
  im::config::pCConfig ptr_global_config_;
  im::c_login::pClientLogin ptr_login_;


};

#define ClientInitGlobal \
  im::c_framework::ClientFramework::Get()->Init()

#define ClientFW \
  im::c_framework::ClientFramework::Get()

#pragma region namespace
}
}
#pragma endregion