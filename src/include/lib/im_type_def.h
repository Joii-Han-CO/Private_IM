#pragma once

// 业务相关的一些类型定义

namespace im {

// 当前客户端所使用的客户端类型
enum class EClientType {
  win = 0,
  mac,
  linux,
  android,
  ios
};

// 定义client类型
#ifdef _WIN32
const EClientType g_client_type_ = EClientType::win;
#else
#endif

}
