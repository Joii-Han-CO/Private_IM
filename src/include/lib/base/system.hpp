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
#ifdef _WIN32
  return _getpid();
#else
  return getpid();
#endif
}

#pragma region
}
}
#pragma endregion
