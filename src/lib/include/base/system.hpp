#pragma once

#ifdef WIN32
#include <process.h>
#else
#include <unistd.h>
#endif


#pragma region
namespace base {
namespace sys {
#pragma endregion

int GetPID() {
  return _getpid();
}

#pragma region
}
}
#pragma endregion
