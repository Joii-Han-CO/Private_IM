#include "pch.h"
#include "im_coding.h"
#include "base/character_conversion.hpp"
#include "iconv.h"

#include <fstream>
#include <array>
#include <vector>
#include <map>


namespace base {
namespace coding {

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

    iconv(iconv_h_, nullptr, nullptr, &in_buf, &in_size);

    return true;
  }

  bool GetCodingList() {
    std::vector<std::string> coding_list;
    iconvlist(CCoding::GetCodingList_DoOne, &coding_list);
    return false;
  }

private:
  static int GetCodingList_DoOne(unsigned int namescount,
                                 const char * const * names,
                                 void* data) {
    if (names == nullptr || data == nullptr)
      return 0;
    //std::vector<std::string> *coding_list = (std::vector<std::string>*)data;
    return 0;
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
  
  return ECodingFileType::None;
}

}
}
