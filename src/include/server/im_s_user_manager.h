#pragma once
#include "base/type_def.h"


#ifdef __cplusplus
extern "C" {
  namespace im {
  namespace s_user_manager {
#endif

  IM_LIB_EXP bool InitServerUserManager();
  IM_LIB_EXP bool WaitAndExit();

#ifdef __cplusplus
  }
  }
}
#endif
