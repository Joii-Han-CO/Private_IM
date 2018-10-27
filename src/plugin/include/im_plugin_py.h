#pragma once
#include "base/type_def.h"


#ifdef __cplusplus
extern "C" {
  namespace im {
  namespace plugin_py {
#endif

  // 测试接口
  IM_LIB_EXP bool Test_plugin_py_C(IN int test_args);

#ifdef __cplusplus
  }
  }
}
#endif
