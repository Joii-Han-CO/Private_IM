#pragma once
#include "base/type_def.h"
#include <string>
#include <map>
#include "base/error.hpp"


#pragma region
namespace im {
namespace config {
#pragma endregion

struct SConfigData;
typedef std::shared_ptr<SConfigData> pSConfigData;

// 主要用于读取用户的配置信息
class CConfig: public base::error::LastError {
public:
  bool Init(const std::wstring &path);

  std::wstring GetVal(const std::wstring &tag, const std::wstring &key);
  std::string GetValA(const std::wstring &tag, const std::wstring &key);
  int GetVal_Int(const std::wstring &tag, const std::wstring &key);

  bool SetVal(const std::wstring &tag, const std::wstring &key,
              std::wstring &val);

private:
  pSConfigData config_;
};
typedef std::shared_ptr<CConfig> pCConfig;

#pragma region
}
}
#pragma endregion
