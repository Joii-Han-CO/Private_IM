#pragma once
#include "base/type_def.h"
#include <vector>


#pragma region
namespace im {
namespace ssl_base {
#pragma endregion

std::vector<char> GetSHA256(const void *data, size_t len);

std::string Base64Encode(IN void *data, IN size_t len);
std::vector<char> Base64Dencode(IN const std::string &d);

#pragma region
}
}
#pragma endregion
