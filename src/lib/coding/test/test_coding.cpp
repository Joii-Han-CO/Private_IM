#include "pch.h"
#include "test_coding.h"
#include "im_coding.h"
#include "base/debug.hpp"
#include "base/format_str.hpp"

#include <map>


#pragma region namespace
namespace test {
#pragma endregion

using base::coding::ECodingFileType;

void TestCoding() {
  const std::map<base::coding::ECodingFileType, std::wstring> tf_path = {
    {ECodingFileType::None, L"E:\\tmpx"},
  {ECodingFileType::GBK, L"E:\\tmp\\coding\\gb2312.c"},
  {ECodingFileType::UTF8, L"E:\\tmp\\coding\\utf8.c"},
  {ECodingFileType::UTF8_BOM, L"E:\\tmp\\coding\\utf8_bom.c"},
  {ECodingFileType::UTF16_LE, L"E:\\tmp\\coding\\utf16_le.c"},
  {ECodingFileType::UTF16_BE, L"E:\\tmp\\coding\\utf16_be.c"},
  {ECodingFileType::None, L"E:\\tmp\\coding\\empty.c"}};

  const char * tmp_d = "联通";
  base::coding::Coding_GBK(tmp_d, strlen(tmp_d));

  for (auto it : tf_path) {
    if (base::coding::GetCoding(it.second.c_str()) != it.first) {
      base::debug::OutPut(L"Failed %s", it.second.c_str());
    }
  }
}

void TestFormat() {
  auto ref1 = base::format::FormatStr("%1%, %2%, %3%", 1, 2, "abc");
  auto ref2 = base::format::FormatStr(L"%1%, %2%, %3%", 1, 2, L"abc");
}

#pragma region namespace
}
#pragma endregion

int main() {
  //test::TestCoding();
  test::TestFormat();

  system("pause");
  return 0;
}
