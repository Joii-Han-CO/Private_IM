#pragma once
#include "base/type_def.h"
#include <vector>
#include <memory>


#pragma region
namespace im {
namespace ssl_base {
#pragma endregion

#pragma region Base64

// Base64相互转换
std::string Base64Encode(IN void *data, IN size_t len);
std::vector<char> Base64Dencode(IN const std::string &d);

#pragma endregion

#pragma region SHA256

// 获取一段数据的sha256值
std::vector<char> GetSHA256(const void *data, size_t len);

// 获取数据sha256的base64编码值
inline std::string GetStrSHA256(const char* str) {
  size_t len = strlen(str);
  auto vec = GetSHA256(str, len);
  return Base64Encode(vec.data(), vec.size());
}

#pragma endregion

#pragma region Token

// 私钥信息
struct STokenPrivateKey {
  std::vector<char> k;
};
typedef std::shared_ptr<STokenPrivateKey> pSTokenPrivateKey;

// 公钥信息
struct STokenPublicKey {
  std::vector<char> k;
};
typedef std::shared_ptr<STokenPublicKey> pSTokenPublicKey;

// 成对的...
struct STokenKey {
  pSTokenPrivateKey pri;
  pSTokenPublicKey pub;
};
typedef std::shared_ptr<STokenKey> pSTokenKey;

class CTokenPrivate {
public:

  // 生成key
  bool GenerateKey();

  // 加密数据
  //  使用C接口，因为在某些情况下加解密可以使用同一个缓冲区
  bool Encrypt(void* in, size_t in_size, void *out, size_t out_size);
  bool Decrypt(void* in, size_t in_size, void *out, size_t out_size);

private:
  pSTokenKey key_;
};

class CTokenPublic {
public:
  bool InitKey(pSTokenPublicKey key);

  bool Encrypt(void* in, size_t in_size, void *out, size_t out_size);
  bool Decrypt(void* in, size_t in_size, void *out, size_t out_size);

public:


};

#pragma endregion

#pragma region
}
}
#pragma endregion
