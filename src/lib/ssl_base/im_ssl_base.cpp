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

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
