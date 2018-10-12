#pragma once
#include "base/type_def.h"
#include "base/error.hpp"
#include <vector>
#include <memory>


#pragma region Util

struct rsa_st;
typedef struct rsa_st RSA;

#pragma endregion


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

#pragma region RSA

const int g_rsa_bit_max_ = 2048;
const int g_rsa_block_size = g_rsa_bit_max_ / 8;

typedef std::vector<char> RSAPublicKey;

// 使用RSA加解密
//  RSA PKCS1 带有长度信息，因此可以返回数据长度
class CRSAED: public base::error::LastError {
public:
  CRSAED();
  CRSAED(IN RSAPublicKey &key);
  ~CRSAED();

  RSAPublicKey GetPublicKey();
  int GetBlockSize();

  // 计算加密后的数据长度
  int GetEncryptSize(int len);

  // 公钥私钥的加解密
  bool Encrypt_Pub(IN const void *in, IN int in_len,
                   OUT void *out, IN_OUT int &out_len);
  bool Decrypt_Pub(IN const void *in, IN int in_len,
                   OUT void *out, IN_OUT int &out_len);

  bool Encrypt_Pri(IN const void *in, IN int in_len,
                   OUT void *out, IN_OUT int &out_len);
  bool Decrypt_Pri(IN const void *in, IN int in_len,
                   OUT void *out, IN_OUT int &out_len);

private:
  bool DoEncryptData(const void *in, int in_len,
                     void *out, int &out_len,
                     std::function<bool(
                       const void *in, int in_len, void *out)> block_func);
  bool DoDecryptData(const void *in, int in_len,
                     void *out, int &out_len,
                     std::function<int(
                       const void*in, void*out)> block_func);
  RSA *rsa_ = nullptr;
  int rsa_block_size_ = 0;
};

#pragma endregion

#pragma region AES

// key长度使用固定值
const int g_aes_key_size_ = 256;
const int g_aes_block_size_ = 16;

// 使用AES加密
//  AES加密可以不对齐，但是解密需要对齐
class CAESED: public base::error::LastError {
public:

  // 密钥根据 g_aes_key_size_ 来决定长度
  CAESED(void *key);
  ~CAESED();

  int GetEncryptSize(int buf_size);
  int GetBlockSize();

  bool Encrypt(IN const void *in, IN int in_len, IN void *cbc_vi,
               OUT void *out, IN int out_len);
  bool Decrypt(IN const void *in, IN int in_len, IN void *cbc_vi,
               OUT void *out, IN int out_len);
private:
  std::vector<char> key_;
};

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

class CTokenPrivate: public base::error::LastError {
public:

  // 生成key
  bool GenerateKey(STokenPublicKey &pub_key);

  // 加密数据
  //  使用C接口，因为在某些情况下加解密可以使用同一个缓冲区
  bool Encrypt(void* in, size_t in_size, void *out, size_t out_size);
  bool Decrypt(void* in, size_t in_size, void *out, size_t out_size);

private:
  pSTokenKey key_;

  RSA *rsa_ = nullptr;

};

class CTokenPublic:public base::error::LastError {
public:
  bool InitKey(const STokenPublicKey &key);

  bool Encrypt(void* in, size_t in_size, void *out, size_t out_size);
  bool Decrypt(void* in, size_t in_size, void *out, size_t out_size);

private:
  RSA *rsa_ = nullptr;
};

#pragma endregion

#pragma region
}
}
#pragma endregion
