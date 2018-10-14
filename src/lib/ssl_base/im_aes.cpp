#include "stdafx.h"
#include "im_ssl_base.h"
#include <openssl/aes.h>

#pragma region
namespace im {
namespace ssl_base {
#pragma endregion

CAESED::CAESED(void *key) {
  if (key) {
    key_.resize(g_aes_key_size_);
    memcpy_s(key_.data(), key_.size(), key, g_aes_key_size_);
  }
}

CAESED::~CAESED() {

}

int CAESED::GetEncryptSize(int buf_size) {
  return(buf_size / g_aes_block_size_ +
    (buf_size % g_aes_block_size_ == 0 ? 0 : 1)) * g_aes_block_size_;
}

int CAESED::GetBlockSize() {
  return g_aes_block_size_;
}

bool CAESED::Encrypt(IN const void *in, IN int in_len, IN void *cbc_vi,
                     OUT void *out, IN int out_len) {
  unsigned char *vi = (unsigned char*)cbc_vi;

  AES_KEY aes_key;
  auto ref = AES_set_encrypt_key((unsigned char*)key_.data(),
                                 256, &aes_key);
  auto func_ecb = [&aes_key] (const void *in, void *out) {
    AES_encrypt((const unsigned char*)in, (unsigned char*)out, &aes_key);
    return g_aes_block_size_;
  };

  auto func_cbc = [&aes_key, &vi] (const void *in, void *out) {
    int n;
    const unsigned char *ti = (const unsigned char*)in;
    unsigned char *to = (unsigned char*)out;
    for (n = 0; n < 16; n++)
      to[n] = ti[n] ^ vi[n];
    AES_encrypt(to, to, &aes_key);
    vi = to;
    return g_aes_block_size_;
  };

  std::function<int(const void *in, void *out)> fc;
  if (vi == nullptr)
    fc = func_ecb;
  else
    fc = func_cbc;

  auto ref_len = DETemp(in, in_len, out, out_len, g_aes_block_size_, fc);
  if (ref_len <= 0)
    return false;

  return true;
}

bool CAESED::Decrypt(IN const void *in, IN int in_len, IN void *cbc_vi,
                     OUT void *out, IN int out_len) {
  std::vector<char> vi;
  if (cbc_vi) {
    vi.resize(g_aes_block_size_);
    memcpy_s(vi.data(), vi.size(), cbc_vi, g_aes_block_size_);
  }

  AES_KEY aes_key;
  auto ref = AES_set_decrypt_key((unsigned char*)key_.data(),
                                 256, &aes_key);

  auto func_ecb = [&aes_key] (const void *in, void *out) {
    AES_decrypt((const unsigned char*)in, (unsigned char*)out, &aes_key);
    return g_aes_block_size_;
  };

  auto func_cbc = [&aes_key, &vi] (const void *in, void *out) {
    const unsigned char *ti = (const unsigned char*)in;
    unsigned char *to = (unsigned char*)out;

    AES_decrypt(ti, to, &aes_key);
    unsigned char c;
    for (int n = 0; n < g_aes_block_size_; ++n) {
      c = ti[n];
      to[n] = to[n] ^ vi[n];
      vi[n] = c;
    }
    return g_aes_block_size_;
  };

  std::function<int(const void *in, void *out)> fc;
  if (vi.empty())
    fc = func_ecb;
  else
    fc = func_cbc;

  auto ref_len = DETemp(in, in_len, out, out_len, g_aes_block_size_, fc);
  if (ref_len <= 0)
    return false;

  return true;
}

int CAESED::GetEncryptSize_Ex(int buf_size) {
  return GetEncryptSize(buf_size * 2);
}

bool CAESED::Encrypt_Ex(IN const void *in, IN int in_len, IN void *cbc_vi,
                        OUT void *out, IN int out_len) {
  // 循环填充随机数...
  unsigned char *vi = (unsigned char*)cbc_vi;

  int block_size = g_aes_block_size_ / 2;

  AES_KEY aes_key;
  auto ref = AES_set_encrypt_key((unsigned char*)key_.data(),
                                 256, &aes_key);

  std::vector<char> tmp_buf;
  tmp_buf.resize(g_aes_block_size_);
  auto func_ecb =
    [&aes_key, &tmp_buf, block_size] (const void *in, void *out) {
    const unsigned char *ti = (const unsigned char*)in;
    unsigned char *to = (unsigned char*)out;

    memset(tmp_buf.data(), 0, tmp_buf.size());
    for (int i = 0; i < block_size; i++) {
      tmp_buf[i * 2] = ti[i];
      tmp_buf[i * 2 + 1] = (unsigned char)rand();
    }
    AES_encrypt((const unsigned char*)tmp_buf.data(), (unsigned char*)out,
                &aes_key);
    return g_aes_block_size_;
  };

  auto func_cbc =
    [&aes_key, &vi, &tmp_buf, block_size] (const void *in, void *out) {

    const unsigned char *ti = (const unsigned char*)in;
    unsigned char *to = (unsigned char*)out;

    memset(tmp_buf.data(), 0, tmp_buf.size());
    for (int i = 0; i < block_size; i++) {
      tmp_buf[i * 2] = ti[i];
      tmp_buf[i * 2 + 1] = (unsigned char)rand();
    }
    for (int n = 0; n < 16; n++)
      tmp_buf[n] = tmp_buf[n] ^ vi[n];
    AES_encrypt((const unsigned char*)tmp_buf.data(), (unsigned char*)out,
                &aes_key);
    vi = to;
    return g_aes_block_size_;
  };

  std::function<int(const void *in, void *out)> fc;
  if (vi == nullptr)
    fc = func_ecb;
  else
    fc = func_cbc;

  auto ref_len = DETemp(in, in_len, out, out_len, block_size, fc);
  if (ref_len <= 0)
    return false;

  return true;
}

bool CAESED::Decrypt_Ex(IN const void *in, IN int in_len, IN void *cbc_vi,
                        OUT void *out, IN int out_len) {
  // 循环填充随机数...
  std::vector<unsigned char> tmp_vi;
  tmp_vi.resize(g_aes_block_size_);
  memcpy_s(tmp_vi.data(), tmp_vi.size(), cbc_vi, g_aes_block_size_);
  unsigned char *vi = tmp_vi.data();

  int black_size = g_aes_block_size_ / 2;

  AES_KEY aes_key;
  auto ref = AES_set_decrypt_key((unsigned char*)key_.data(),
                                 256, &aes_key);

  std::vector<char> tmp_buf;
  tmp_buf.resize(g_aes_block_size_);
  auto func_ecb =
    [&aes_key, &tmp_buf, black_size] (const void *in, void *out) {
    const unsigned char *ti = (const unsigned char*)in;
    unsigned char *to = (unsigned char*)out;
    AES_decrypt((const unsigned char*)in, (unsigned char*)tmp_buf.data(),
                &aes_key);

    for (int i = 0; i < black_size; i++)
      to[i] = tmp_buf[i * 2];

    return black_size;
  };

  auto func_cbc =
    [&aes_key, &vi, &tmp_buf, black_size] (const void *in, void *out) {
    const unsigned char *ti = (const unsigned char*)in;
    unsigned char *to = (unsigned char*)out;
    AES_decrypt((const unsigned char*)ti, (unsigned char*)tmp_buf.data(),
                &aes_key);

    unsigned char c;
    for (int n = 0; n < g_aes_block_size_; ++n) {
      c = ti[n];
      tmp_buf[n] = tmp_buf[n] ^ vi[n];
      vi[n] = c;
    }

    for (int i = 0; i < black_size; i++)
      to[i] = tmp_buf[i * 2];

    return black_size;
  };

  std::function<int(const void *in, void *out)> fc;
  if (vi == nullptr)
    fc = func_ecb;
  else
    fc = func_cbc;

  auto ref_len = DETemp(in, in_len, out, out_len, g_aes_block_size_, fc);
  if (ref_len <= 0)
    return false;

  return true;
}

std::vector<char> CAESED::GetKey() {
  return key_;
}

#pragma region
}
}
#pragma endregion
