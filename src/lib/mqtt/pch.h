// 预编译头
#pragma once

// STL
#include <string>
#include <mutex>
#include <condition_variable>

// Mqtt
#include <mosquitto.h>

#ifdef _WIN32
#include <windows.h>
#else
#endif


#include "base/type_def.h"
#include "im_type_def.h"
