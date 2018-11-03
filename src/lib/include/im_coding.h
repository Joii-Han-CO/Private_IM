#pragma once
#include "base/type_def.h"

#include <string>
#include <boost/format.hpp>


// 处理文本\数据编码的库...

#pragma region
namespace base {
namespace coding {
#pragma endregion

enum class ECodingFileType {
  None,
  GBK,
  UTF8,
  UTF8_BOM,
  UTF16,
  UTF16_LE,
  UTF16_BE,
};

std::wstring Coding_GBK(const char *buf, int size);

std::wstring Coding_UTF8(const char *buf, int size);

ECodingFileType GetCoding_U8(const char *path);

ECodingFileType GetCoding(const wchar_t *path);

#pragma region
}
}
#pragma endregion
