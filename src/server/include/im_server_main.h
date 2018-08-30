#pragma once
#include "base/type_def.h"


#ifdef __cplusplus
extern "C" {
  namespace im {
  namespace server_main {
#endif

  // 测试接口
  IM_LIB_EXP bool Test_server_main_C(IN int test_args);

#ifdef __cplusplus
  }
  }
}
#endif
