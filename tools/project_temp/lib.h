#pragma once
#include "im_type_def.h"


#ifdef __cplusplus
extern "C" {
  namespace im {
  namespace $(--prj_name--) {
#endif

  // 测试接口
  IM_LIB_EXP bool Test_$(--prj_name--)_C(IN int test_args);

#ifdef __cplusplus
  }
  }
}
#endif
