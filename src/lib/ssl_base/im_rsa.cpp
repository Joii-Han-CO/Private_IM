#include "stdafx.h"
#include "im_ssl_base.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>


#pragma region
namespace im {
namespace ssl_base {
#pragma endregion

class AutoFreeBIO {
public:
  AutoFreeBIO() {
    bio_ = BIO_new(BIO_s_mem());
  }
  ~AutoFreeBIO() {
    if (bio_)
      BIO_free(bio_);
  }

  BIO* Get() {
    return bio_;
  }

private:
  BIO *bio_ = nullptr;
};

const int g_RSA_PSK_last_size_ = 11;

CRSAED::CRSAED() {
  // 生成密钥
  rsa_ = RSA_generate_key(g_rsa_bit_max_, RSA_F4, nullptr, nullptr);
  if (rsa_ == nullptr) {
    SetLastErr("openssl generate rsa key failed");
    return;
  }
  return;
}

CRSAED::CRSAED(IN RSAPublicKey &key) {
  AutoFreeBIO auto_bio;
  BIO *bio = auto_bio.Get();
  if (bio == nullptr) {
    SetLastErr("openssl new bio failed");
    return;
  }

  if (BIO_write(bio, key.data(), key.size()) == 0) {
    SetLastErr("openssl write bio failed");
    return;
  }

  rsa_ = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
  if (rsa_ == nullptr) {
    SetLastErr("openssl read pem rsa public key failed");
    return;
  }

  return;
}

CRSAED::~CRSAED() {
  if (rsa_)
    RSA_free(rsa_);
}

// 获取RSA公钥
RSAPublicKey CRSAED::GetPublicKey() {
  RSAPublicKey pub_key;
  if (rsa_ == nullptr) {
    auto rf_err = base::Utf16ToGB2312(last_error_);
    SetLastErr("rsa init failed, des:%s", rf_err.c_str());
    return pub_key;
  }

  AutoFreeBIO auto_bio;
  auto bio = auto_bio.Get();
  if (PEM_write_bio_RSAPublicKey(bio, rsa_) == 0) {
    SetLastErr("openssl pem write public key failed");
    return pub_key;
  }

  static std::vector<char> tmp_buf;
  if (tmp_buf.empty())
    tmp_buf.resize(1024 * 4);
  auto bio_size = BIO_read(bio, tmp_buf.data(), tmp_buf.size());
  if (bio_size == 0) {
    SetLastErr("openssl bio read buf failed");
    return pub_key;
  }
  pub_key.resize(bio_size);
  memcpy_s(pub_key.data(), pub_key.size(), tmp_buf.data(), bio_size);

  return pub_key;
}

int CRSAED::GetEncryptSize(int len) {
  // 每段数据差11位
  int bs = GetBlockSize();    // 每块数据的输出长度
  int bsc = bs - g_RSA_PSK_last_size_;    // 每块输入长度的数据
  int lb = len % bsc;         // 余数
  int count = len / bsc + (lb == 0 ? 0 : 1);    // 会有多少块数据

  return count * bs;
}

int CRSAED::GetBlockSize() {
  if (rsa_block_size_ == 0 && rsa_ != nullptr) {
    rsa_block_size_ = RSA_size(rsa_);
    return rsa_block_size_;
  }
  return rsa_block_size_;
}

bool CRSAED::Encrypt_Pub(IN const void *in, IN int in_len,
                         OUT void *out, IN_OUT int &out_len) {
  int last_block = 0;
  int rsa_len = GetBlockSize();
  auto lbd_func = [this] (const void *in, int in_len, void *out) {
    int ref = RSA_public_encrypt(
      in_len,
      (const unsigned char*)in, (unsigned char*)out,
      rsa_, RSA_PKCS1_PADDING);

    if (ref == -1) {
      SetLastErr("openssl encrypt failed");
      return false;
    }

    return true;
  };

  if (DoEncryptData(in, in_len, out, out_len,
                    lbd_func) == false)
    return false;
  return true;
}

bool CRSAED::Decrypt_Pub(IN const void *in, IN int in_len,
                         OUT void *out, IN_OUT int &out_len) {
  int last_block = 0;
  int rsa_len = GetBlockSize();
  auto lbd_func = [this, rsa_len] (const void *in, void *out) {
    int ref = RSA_public_decrypt(
      rsa_len,
      (const unsigned char*)in, (unsigned char*)out,
      rsa_, RSA_PKCS1_PADDING);
    if (ref == -1) {
      SetLastErr("rsa public decrypt failed");
    }
    return ref;
  };

  if (DoDecryptData(in, in_len, out, out_len,
                    lbd_func) == false)
    return false;
  return true;
}

bool CRSAED::Encrypt_Pri(IN const void *in, IN int in_len,
                         OUT void *out, IN_OUT int &out_len) {
  int last_block = 0;
  int rsa_len = GetBlockSize();
  auto lbd_func = [this] (const void *in, int in_len, void *out) {
    int ref = RSA_private_encrypt(
      in_len,
      (const unsigned char*)in, (unsigned char*)out,
      rsa_, RSA_PKCS1_PADDING);

    if (ref == -1) {
      SetLastErr("openssl encrypt failed");
      return false;
    }

    return true;
  };

  if (DoEncryptData(in, in_len, out, out_len,
                    lbd_func) == false)
    return false;
  return true;
}

bool CRSAED::Decrypt_Pri(IN const void *in, IN int in_len,
                         OUT void *out, IN_OUT int &out_len) {
  int last_block = 0;
  int rsa_len = GetBlockSize();
  auto lbd_func = [this, rsa_len] (const void *in, void *out) {
    int ref = RSA_private_decrypt(
      rsa_len,
      (const unsigned char*)in, (unsigned char*)out,
      rsa_, RSA_PKCS1_PADDING);
    if (ref == -1) {
      SetLastErr("rsa public decrypt failed");
    }
    return ref;
  };

  if (DoDecryptData(in, in_len, out, out_len,
                    lbd_func) == false)
    return false;
  return true;
}

// 把整段数据分块加密
bool CRSAED::DoEncryptData(const void *in, int in_len,
                           void *out, int &out_len,
                           std::function<bool(
                             const void *in, int in_len,
                             void *out)> block_func) {
  // 校验数据
  if (in == nullptr || in_len == 0 ||
      out == nullptr || out_len == 0 ||
      block_func == nullptr) {
    SetLastErr("parameter error");
    return false;
  }

  int block_size = GetBlockSize();
  int es_out_len = GetEncryptSize(in_len);

  if (out_len < es_out_len) {
    SetLastErr("out buffer too short");
    return false;
  }

  { // 加密
    int in_block_size = block_size - g_RSA_PSK_last_size_;
    int count = in_len / (in_block_size);
    char *tmp_i = (char *)in, *tmp_o = (char *)out;

    for (int i = 0; i < count; i++) {
      if (block_func(tmp_i, in_block_size, tmp_o) == false)
        return false;
      tmp_i = &(tmp_i[in_block_size]);
      tmp_o = &(tmp_o[block_size]);
    }

    int last_block = in_len % in_block_size;
    if (last_block != 0) {
      if (block_func(tmp_i, last_block, tmp_o) == false)
        return false;
    }
  }
  out_len = es_out_len;
  return true;
}

// 解密
bool CRSAED::DoDecryptData(const void * in, int in_len,
                           void * out, int & out_len,
                           std::function<int(
                             const void*in, void*out)> block_func) {
  if (in == nullptr || in_len == 0 ||
      out == nullptr || out_len == 0 ||
      block_func == nullptr) {
    SetLastErr("parameter error");
    return false;
  }
  int block_size = GetBlockSize();
  std::vector<char> tmp_buf;
  tmp_buf.resize(block_size);
  int tmp_len = 0;
  out_len = 0;
  int count = in_len / block_size;
  char *tmp_i = (char *)in, *tmp_o = (char *)out;
  for (int i = 0; i < count; i++) {
    tmp_len = block_func(tmp_i, tmp_o);
    if (tmp_len == -1) {
      return false;
    }

    tmp_i = &(tmp_i[block_size]);
    tmp_o = &(tmp_o[tmp_len]);
    out_len += tmp_len;
  }

  return true;
}

#pragma region
}
}
#pragma endregion
