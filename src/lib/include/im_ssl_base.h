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

// 使用 sha256 对比两块数据是否相同
bool CheckoutBuf_SHA256(const void *d1, const void *d2, size_t len);

// 获取数据sha256的base64编码值
inline std::string GetStrSHA256(const char* str) {
  size_t len = strlen(str);
  auto vec = GetSHA256(str, len);
  return Base64Encode(vec.data(), vec.size());
}

class CheckoutSHA256 {
public:
  // 输入需要存储的缓冲大小
  CheckoutSHA256(int bufsize);
  CheckoutSHA256(void * buf, int size);

  // 获取buffer指针
  void *GetBuffer();

  // 获取buffer长度
  int GetBufferSize();

  // 获取整个buffer
  void *GetAllBuffer();

  // 获取整个buffer的长度
  int GetAllBufferSize();

  // 获取可能的最小缓冲大小
  static int GetMinBufferSize();

  // 通过buffer更新sha256
  void UpdateSha256();

  // 校验当前的数据是否匹配
  static bool CheckoutSha256(const void *d);
  bool CheckoutSha256();
private:
  std::vector<char> buf_;
  void *b_;
  int b_size_;
};

#pragma endregion

// 加密模块的基类
class DEBase:public base::error::LastError {
public:
  virtual ~DEBase() {};
protected:
  int DETemp(IN const void *in, IN  int in_len,
             OUT void *out, IN int out_len,
             IN int block_size,
             IN std::function<int(const void *in, void *out)> cell) {
    if (in == nullptr || in_len == 0) {
      SetLastErr("in parameter error");
      return -1;
    }
    if (out == nullptr || out_len == 0) {
      SetLastErr("out parameter error");
      return -1;
    }

    int es_out_len = 0;
    int count = in_len / block_size;
    char *tmp_i = (char*)in, *tmp_o = (char*)out;
    int ref_len;
    for (int i = 0; i < count; i++) {
      ref_len = cell(tmp_i, tmp_o);
      if (ref_len <= 0)
        return -1;
      tmp_i = &(tmp_i[block_size]);
      tmp_o = &(tmp_o[ref_len]);
      es_out_len += ref_len;
    }

    int last_block = in_len % block_size;
    if (last_block != 0) {
      std::vector<char> tmp_buf;
      tmp_buf.resize(block_size);
      memcpy_s(tmp_buf.data(), tmp_buf.size(), tmp_i, last_block);
      ref_len = cell(tmp_buf.data(), tmp_o);
      if (ref_len < 0)
        return -1;
      es_out_len += ref_len;
    }
    return es_out_len;
  }
};

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
typedef std::shared_ptr<CRSAED> pCRSAED;

#pragma endregion

#pragma region AES

// key长度使用固定值
const int g_aes_key_size_ = 256;
const int g_aes_block_size_ = 16;

// 使用AES加密
//  AES加密可以不对齐，但是解密需要对齐
class CAESED: public DEBase {
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

  // 加密数据
  //  数据中间插入随机数...
  int GetEncryptSize_Ex(int buf_size);
  bool Encrypt_Ex(IN const void *in, IN int in_len, IN void *cbc_vi,
                  OUT void *out, IN int out_len);
  bool Decrypt_Ex(IN const void *in, IN int in_len, IN void *cbc_vi,
                  OUT void *out, IN int out_len);

  std::vector<char> GetKey();
private:
  std::vector<char> key_;
};
typedef std::shared_ptr<CAESED> pCAESED;

#pragma endregion

#pragma region Token
// 业务相关

const unsigned int g_token_version = 1;

class CToken: public DEBase {
public:
  CToken();

  // 交换公钥私钥
  std::vector<char> GetPublicKey();
  bool SetPublicKey(std::vector<char> &data);

  int GetEncryptSize(int bufsize);

  // 加解密数据
  //  解密时无法预估数据长度，因此设计成输出buff，由用户处理
  int Encrypt(IN const void *in, IN int in_len,
              OUT void *out, IN int out_len);
  int Decrypt(IN const void *in, IN int in_len,
              OUT void **out, IN int *out_len);
  void ReleaseDecryptBuffer(IN void *out, IN int out_len);

private:
  struct STokenKey {
    pCRSAED a;
    pCAESED s;
    std::vector<char> vi;
  };

  void InitPrivateKey(STokenKey &key);

  STokenKey pub_;
  STokenKey pri_;
};

#pragma endregion

#pragma region
}
}
#pragma endregion
