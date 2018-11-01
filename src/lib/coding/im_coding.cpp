#include "stdafx.h"
#include "im_coding.h"
#include "base/character_conversion.hpp"
#include "iconv.h"

#include <fstream>
#include <array>
#include <vector>
#include <map>


#pragma region namespace
namespace base {
namespace coding {
#pragma endregion

enum class ECodingType {
  GBK,
  UTF8,
  UTF16,
};

class CCoding {
public:
  CCoding(ECodingType from, ECodingType to) {
    iconv_h_ = iconv_open(coding_name_[from], coding_name_[to]);
  }

  ~CCoding() {
    if (iconv_h_)
      iconv_close(iconv_h_);
  }

  bool Conv(const char *buf, int size) {
    if (iconv_h_ == (iconv_t)(-1))
      return false;

    auto sdfsdfsfbuf = "联通";
    char *in_buf = (char*)(sdfsdfsfbuf);
    size_t in_size = (size_t)4;

    char *out_buf = nullptr;
    size_t out_size = 0;

    auto refx = iconv(iconv_h_, nullptr, nullptr, &in_buf, &in_size);

    return true;
  }

  bool GetCodingList() {
    std::vector<std::string> coding_list;
    iconvlist(CCoding::GetCodingList_DoOne, &coding_list);

  }

private:
  static int GetCodingList_DoOne(unsigned int namescount,
                                 const char * const * names,
                                 void* data) {
    if (names == nullptr || data == nullptr)
      return 0;
    std::vector<std::string> *coding_list = (std::vector<std::string>*)data;
  };

  iconv_t iconv_h_ = nullptr;

  std::map<ECodingType, const char*> coding_name_ =
  {{ECodingType::GBK, "GBK"},
  {ECodingType::UTF8, "UTF-8"},
  {ECodingType::UTF16, "UTF-16"},
  };
};

// 转码...
std::wstring Coding_GBK(const char *buf, int size) {
  CCoding cov(ECodingType::UTF16, ECodingType::UTF8);

  cov.Conv(buf, size);

  return std::wstring();
}

// 判断bom
ECodingFileType CheckBomCoding(std::ifstream &f) {
  std::array<UCHAR, 4> buf;
  memset(buf.data(), 0, buf.size());
  f.read((char*)buf.data(), 4);

  if (buf[0] == 0xef && buf[1] == 0xbb && buf[2] == 0xbf)
    return ECodingFileType::UTF8_BOM;
  else if (buf[0] == 0xfe && buf[1] == 0xff)
    return ECodingFileType::UTF16_BE;
  else if (buf[0] == 0xff && buf[1] == 0xfe)
    return ECodingFileType::UTF16_LE;
  else
    return ECodingFileType::None;
}

ECodingFileType GetCoding_U8(const char *path) {
  if (path == nullptr || path[0] == '\0')
    return ECodingFileType::None;
  return GetCoding(base::Utf8ToUtf16(path).c_str());
}

ECodingFileType GetCoding(const wchar_t *path) {
  if (path == nullptr || path[0] == '\0')
    return ECodingFileType::None;
  std::ifstream f(path, std::ios_base::binary);
  if (!f)
    return ECodingFileType::None;

  auto func_re = [&f](ECodingFileType ref = ECodingFileType::None) {
    f.close();
    return ref;
  };

  // size
  f.seekg(0, std::ios_base::end);
  int size = (int)f.tellg();
  bool not_utf8 = false, not_utf16 = false, not_gbk = false;
  if (size == 0)
    return func_re();
  if (size % 2 != 0)
    not_utf16 = true;
  f.seekg(0, std::ios_base::beg);

  // bom
  auto bom_coding = CheckBomCoding(f);
  if (bom_coding != ECodingFileType::None)
    return func_re(bom_coding);

  // 读取文件，判断...
  f.seekg(0, std::ios_base::beg);
  std::vector<char> buf;
  const int block_size = 1024 * 4;
  buf.resize(block_size + 8); // + 8 作为缓冲

  while (size > 0) {
    memset(buf.data(), 0, buf.size());
    int read_size = size > block_size ? block_size : size;
    f.read(buf.data(), read_size);

    size -= read_size;
  }

  return ECodingFileType::None;
}

#pragma region namespace
}
}
#pragma endregion
