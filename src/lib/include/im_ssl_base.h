#pragma once
#include "base/type_def.h"
#include <vector>


#pragma region
namespace im {
namespace ssl_base {
#pragma endregion

std::vector<char> GetHash(const std::vector<char> &data);

#pragma region
}
}
#pragma endregion
