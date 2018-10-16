#pragma once
#include "base/type_def.h"


#ifdef __cplusplus
extern "C" {
  namespace im {
  namespace login {
#endif

  //////////////////////////////////////////////////////////////////////////
  //  Login 模块
  //    该模块创建消息模块...
  //////////////////////////////////////////////////////////////////////////

  class Login {
  public:
    Login();
    ~Login();

    // 全局初始化(部分异步处理)
    bool Init();
  };

#ifdef __cplusplus
  }
  }
}
#endif
