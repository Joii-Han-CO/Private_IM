#include "stdafx.h"
#include "im_config.h"
#include "im_config.h"
#include <SimpleIni.h>


#pragma region namespace
namespace im {
namespace config {
#pragma endregion

struct SConfigData {
  CSimpleIniW ini;
};

const wchar_t *GetSIErrorDes(SI_Error err) {
  switch (err) {
  case SI_OK:
    return L"";
    break;
  case SI_UPDATED:
    return L"An existing value was updated";
    break;
  case SI_INSERTED:
    return L"A new value was inserted";
    break;
  case SI_FAIL:
    return L"Generic failure";
    break;
  case SI_NOMEM:
    return L"Out of memory error";
    break;
  case SI_FILE:
    return L"File error (see errno for detail error)";
    break;
  default:
    return L"";
    break;
  }

  return L"";
}

bool CConfig::Init(const std::wstring &path) {
  config_ = std::make_shared<SConfigData>();

  config_->ini.SetUnicode(true);
  auto err = config_->ini.LoadFile(path.c_str());
  if (err != 0) {
    SetLastErr(L"load %s file failed, des:%s",
               path.c_str(), GetSIErrorDes(err));
    config_ = nullptr;
    return false;
  }
  return true;
}

std::wstring CConfig::GetVal(const std::wstring &tag,
                             const std::wstring &key) {
  if (config_ == nullptr) {
    SetLastErr("init failed");
    return false;
  }
  return config_->ini.GetValue(tag.c_str(), key.c_str(), L"");
}

bool CConfig::SetVal(const std::wstring &tag, const std::wstring &key,
                     std::wstring &val) {
  if (config_ == nullptr) {
    SetLastErr("init failed");
    return false;
  }
  auto err = config_->ini.SetValue(tag.c_str(), key.c_str(), val.c_str());
  if (err != 0) {
    SetLastErr(L"SetVal failed, tag:%s, key:%s, val:%s, des:%s",
               tag.c_str(), key.c_str(), val.c_str(),
               GetSIErrorDes(err));
    return false;
  }
  return true;
}

#pragma region namespace
}
}
#pragma endregion
