#pragma once
#include "im_type_def.h"
#include "im_$(--prj_name--).h"


#pragma region namespace
namespace im {
namespace $(--prj_name--) {
#pragma endregion

// 测试接口
INLINE bool Test_$(--prj_name--)_CPP(IN int test_args) {
  return true;
};

#pragma region namespace
}
}
#pragma endregion
