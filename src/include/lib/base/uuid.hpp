#pragma once
#include "type_def.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


#pragma region
namespace base {
namespace _uuid {
#pragma endregion

struct BaseUUID {
  unsigned int d1 = 0;
  unsigned short d2 = 0;
  unsigned short d3 = 0;
  unsigned char d4[8] = {0};
};
StdSharedPtr_Typedef(BaseUUID);

inline std::string GenerateUUID_Str(char t) {
  boost::uuids::uuid u = boost::uuids::random_generator()();
  return boost::uuids::to_string(u);
}

inline std::wstring GenerateUUID_Str(wchar_t t) {
  boost::uuids::uuid u = boost::uuids::random_generator()();
  return boost::uuids::to_wstring(u);
}

inline void GenerateUUID(BaseUUID *uid) {
  if (uid == nullptr)
    return;
  boost::uuids::uuid u = boost::uuids::random_generator()();

  memcpy_s(uid, sizeof(BaseUUID), u.data, sizeof(u.data));
}

template <typename cus_char>
inline cus_string GenerateUUID() {
  cus_char t = '\0';
  return GenerateUUID_Str(t);
};

#pragma region
}
}
#pragma endregion
