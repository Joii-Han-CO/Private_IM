#pragma once
#include "base/type_def.h"
#include <vector>


#pragma region
namespace im {
namespace ssl_base {
#pragma endregion

#pragma region Base64

// Base64相互转换
std::string Base64Encode(IN void *data, IN size_t len);
std::vector<char> Base64Dencode(IN const std::string &d);

#pragma endregion

#pragma region SHA256

// 获取一段数据的sha256值
std::vector<char> GetSHA256(const void *data, size_t len);

// 获取数据sha256的base64编码值
inline std::string GetStrSHA256(const char* str) {
  size_t len = strlen(str);
  auto vec = GetSHA256(str, len);
  return Base64Encode(vec.data(), vec.size());
}

#pragma endregion

#pragma region
}
}
#pragma endregion
