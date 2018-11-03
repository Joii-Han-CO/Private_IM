#pragma once
#include "type_def.h"
#include "boost/format.hpp"

#pragma region namespace
namespace base {
namespace format {
#pragma endregion

namespace {
#define cus_format boost::basic_format<cus_char>
#define pcus_format std::shared_ptr<cus_format>

template <typename cus_char>
void IterationArgs(cus_format &fmt) {}

template <typename cus_char, typename T1, typename ...T2>
void IterationArgs(cus_format &fmt, T1 t1, T2 ...t2) {
  fmt = fmt % t1;
  IterationArgs(fmt, t2...);
}

}
template <typename cus_char, typename ... T1>
cus_string FormatStr(const cus_char *sz, T1 ...t) {
  cus_format fmt = cus_format(sz);
  IterationArgs(fmt, t...);
  return fmt.str();
};

#pragma region namespace
}
}
#pragma endregion
