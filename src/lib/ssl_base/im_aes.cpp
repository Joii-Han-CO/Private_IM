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
  if (in == nullptr || in_len <= 0 || out == nullptr) {
    SetLastErr("parameter error");
    return false;
  }
  auto es_out_len = GetEncryptSize(in_len);
  if (out_len < es_out_len) {
    SetLastErr("out buffer too short");
    return false;
  }

  unsigned char *vi = (unsigned char*)cbc_vi;

  AES_KEY aes_key;
  auto ref = AES_set_encrypt_key((unsigned char*)key_.data(),
                                 256, &aes_key);

  auto func_ecb = [&aes_key] (unsigned char *in, unsigned char *out) {
    AES_encrypt(in, out, &aes_key);
  };

  auto func_cbc = [&aes_key, &vi] (unsigned char *in, unsigned char *out) {
    int n;
    for (n = 0; n < 16; n++)
      out[n] = in[n] ^ vi[n];
    AES_encrypt(out, out, &aes_key);
    vi = out;
  };

  std::function<void(unsigned char *in, unsigned char *out)> fc;
  if (vi == nullptr)
    fc = func_ecb;
  else
    fc = func_cbc;

  int count = in_len / g_aes_block_size_;
  unsigned char *tmp_i = (unsigned char*)in;
  unsigned char *tmp_o = (unsigned char*)out;
  for (int i = 0; i < count; i++) {
    fc(tmp_i, tmp_o);
    tmp_i = &(tmp_i[g_aes_block_size_]);
    tmp_o = &(tmp_o[g_aes_block_size_]);
  }

  int last_block = in_len % g_aes_block_size_;
  if (last_block) {
    std::vector<unsigned char> tmp_buf;
    tmp_buf.resize(g_aes_block_size_);
    memcpy_s(tmp_buf.data(), last_block, tmp_i, last_block);
    fc(tmp_buf.data(), tmp_o);
  }

  return true;
}

bool CAESED::Decrypt(IN const void *in, IN int in_len, IN void *cbc_vi,
                     OUT void *out, IN int out_len) {
  if (in == nullptr || in_len <= 0 || out == nullptr) {
    SetLastErr("parameter error");
    return false;
  }
  if (in_len % g_aes_block_size_ != 0) {
    SetLastErr("in buffer checked failed");
    return false;
  }
  if (out_len < in_len) {
    SetLastErr("out buffer too short");
    return false;
  }

  std::vector<char> vi;
  if (cbc_vi) {
    vi.resize(g_aes_block_size_);
    memcpy_s(vi.data(), vi.size(), cbc_vi, g_aes_block_size_);
  }

  AES_KEY aes_key;
  auto ref = AES_set_decrypt_key((unsigned char*)key_.data(),
                                 256, &aes_key);

  auto func_ecb = [&aes_key] (unsigned char *in, unsigned char *out) {
    AES_decrypt(in, out, &aes_key);
  };

  auto func_cbc = [&aes_key, &vi] (unsigned char *in, unsigned char *out) {
    AES_decrypt(in, out, &aes_key);
    unsigned char c;
    for (int n = 0; n < g_aes_block_size_; ++n) {
      c = in[n];
      out[n] = out[n] ^ vi[n];
      vi[n] = c;
    }
  };

  std::function<void(unsigned char *in, unsigned char *out)> fc;
  if (vi.empty())
    fc = func_ecb;
  else
    fc = func_cbc;

  int count = in_len / g_aes_block_size_;
  unsigned char *tmp_i = (unsigned char*)in;
  unsigned char *tmp_o = (unsigned char*)out;
  for (int i = 0; i < count; i++) {
    fc(tmp_i, tmp_o);
    tmp_i = &(tmp_i[g_aes_block_size_]);
    tmp_o = &(tmp_o[g_aes_block_size_]);
  }

  int last_block = in_len % g_aes_block_size_;
  if (last_block) {
    std::vector<unsigned char> tmp_buf;
    tmp_buf.resize(g_aes_block_size_);
    memcpy_s(tmp_buf.data(), last_block, tmp_i, last_block);
    fc(tmp_buf.data(), tmp_o);
  }

  return true;
  return true;
}

#pragma region
}
}
#pragma endregion
