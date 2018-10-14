#include "stdafx.h"
#include "im_ssl_base.h"
#include "..\include\im_ssl_base.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>


#pragma region namespace
namespace im {
namespace ssl_base {
#pragma endregion

#pragma region Base64

std::string Base64Encode(IN void *data, IN size_t len) {
  BIO * bmem = NULL;
  BIO * b64 = NULL;
  BUF_MEM * bptr = NULL;

  b64 = BIO_new(BIO_f_base64());
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

  bmem = BIO_new(BIO_s_mem());
  b64 = BIO_push(b64, bmem);
  BIO_write(b64, data, len);
  BIO_flush(b64);
  BIO_get_mem_ptr(b64, &bptr);

  std::string r;
  r.resize(bptr->length + 1);
  memcpy((void*)r.c_str(), bptr->data, bptr->length);
  r[bptr->length] = 0;

  BIO_free_all(b64);

  return r;
}

std::vector<char> Base64Dencode(IN const std::string &d) {
  std::vector<char> r;

  return r;
}

#pragma endregion

#pragma region SHA256

std::vector<char> GetSHA256(const void *data, size_t len) {
  std::vector<char> ref_val;
  if (data == nullptr || len == 0)
    return ref_val;
  ref_val.resize(SHA256_DIGEST_LENGTH);

  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, data, len);
  SHA256_Final((unsigned char*)ref_val.data(), &sha256);

  return ref_val;
}

bool CheckoutBuf_SHA256(const void * d1, const void * d2, size_t len) {
  auto sd1 = im::ssl_base::GetSHA256(d1, len);
  auto sd2 = im::ssl_base::GetSHA256(d2, len);
  return sd1 == sd2;
}

struct SSHA256_CheckBuffer {
  int sha256_size;
  char sha256[32];
  int buf_size;
  char buf;
};

CheckoutSHA256::CheckoutSHA256(int bufsize) {
  if (bufsize == 0)
    return;

  buf_.resize(sizeof(SSHA256_CheckBuffer) + bufsize);

  SSHA256_CheckBuffer *b = (SSHA256_CheckBuffer*)&(buf_[0]);
  b->buf_size = bufsize;
  b_ = buf_.data();
  b_size_ = (int)buf_.size();
}

CheckoutSHA256::CheckoutSHA256(void * buf, int size) {
  b_ = buf;
  b_size_ = size;
}

void * CheckoutSHA256::GetBuffer() {
  if (b_ == nullptr)
    return nullptr;
  SSHA256_CheckBuffer *b = (SSHA256_CheckBuffer*)b_;
  return &b->buf;
}

int CheckoutSHA256::GetBufferSize() {
  if (b_ == nullptr)
    return -1;
  SSHA256_CheckBuffer *b = (SSHA256_CheckBuffer*)b_;
  return b->buf_size;
}

void * CheckoutSHA256::GetAllBuffer() {
  return b_;
}

int CheckoutSHA256::GetAllBufferSize() {
  return b_size_;
}

int CheckoutSHA256::GetMinBufferSize() {
  return sizeof(SSHA256_CheckBuffer);
}

void CheckoutSHA256::UpdateSha256() {
  if (b_ == nullptr)
    return;
  SSHA256_CheckBuffer *b = (SSHA256_CheckBuffer*)b_;

  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, &b->buf, b->buf_size);
  SHA256_Final((unsigned char*)b->sha256, &sha256);
  b->sha256_size = SHA256_DIGEST_LENGTH;
}

bool CheckoutSHA256::CheckoutSha256(const void *d) {
  SSHA256_CheckBuffer *b = (SSHA256_CheckBuffer*)d;
  if (b->sha256_size != SHA256_DIGEST_LENGTH)
    return false;
  if (b->buf_size <= 0)
    return false;

  auto d_sha = GetSHA256(&b->buf, b->buf_size);
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    if (d_sha[i] != b->sha256[i])
      return false;
  }

  return true;
}

bool CheckoutSHA256::CheckoutSha256() {
  if (b_ == nullptr)
    return false;
  return CheckoutSha256(b_);
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
