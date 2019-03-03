#pragma once
#include "type_def.h"
#include "base/cross_system.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


namespace base {
namespace _uuid {

// BaseUUID
struct BUID {
  unsigned int d1 = 0;
  unsigned short d2 = 0;
  unsigned short d3 = 0;
  unsigned char d4[8] = {0};
};
StdSharedPtr_Typedef(BUID);

inline std::string Generate_Str(char t) {
  boost::uuids::uuid u = boost::uuids::random_generator()();
  return boost::uuids::to_string(u);
}

inline std::wstring Generate_Str(wchar_t t) {
  boost::uuids::uuid u = boost::uuids::random_generator()();
  return boost::uuids::to_wstring(u);
}

inline void Generate(BUID *uid) {
  if (uid == nullptr)
    return;
  boost::uuids::uuid u = boost::uuids::random_generator()();

  memcpy_s(uid, sizeof(BUID), u.data, sizeof(u.data));
}

inline std::string _GetStr(BUID *uid, char t) {
  std::string buf;
  buf.resize(37);

  sprintf_s(&buf[0], buf.size(),
            "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uid->d1, uid->d2, uid->d3, uid->d4[0],
            uid->d4[1], uid->d4[2],
            uid->d4[3], uid->d4[4], uid->d4[5], uid->d4[6], uid->d4[7]);
  return buf.c_str();
}

inline std::wstring _GetStr(BUID *uid, wchar_t t) {
  std::wstring buf;
  buf.resize(37);

  swprintf_s(&buf[0], buf.size(),
             L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             uid->d1, uid->d2, uid->d3, uid->d4[0],
             uid->d4[1], uid->d4[2],
             uid->d4[3], uid->d4[4], uid->d4[5], uid->d4[6], uid->d4[7]);
  return buf.c_str();
}

template <typename cus_char>
inline cus_string GetStr(BUID *uid) {
  cus_char t = '\0';
  return _GetStr(uid, t);
}

template <typename cus_char>
inline cus_string Generate() {
  cus_char t = '\0';
  return Generate_Str(t);
};

}
}
