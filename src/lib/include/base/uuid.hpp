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
  unsigned int d1;
  unsigned short d2;
  unsigned short d3;
  unsigned char d4[8];
};

inline std::string GenerateUUID_Str(char t) {
  boost::uuids::uuid u = boost::uuids::random_generator()();
  return boost::uuids::to_string(u);
}

inline std::wstring GenerateUUID_Str(wchar_t t) {
  boost::uuids::uuid u = boost::uuids::random_generator()();
  return boost::uuids::to_wstring(u);
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
