#include "stdafx.h"
#include "im_ssl_base.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>

// Token

#pragma region
namespace im {
namespace ssl_base {
#pragma endregion

#pragma region Util

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

#pragma endregion


#pragma region Private

bool CTokenPrivate::GenerateKey(STokenPublicKey &pub_key) {

  // openssl
  rsa_ = RSA_generate_key(g_rsa_bit_max_, RSA_F4, nullptr, nullptr);
  if (rsa_ == nullptr) {
    SetLastErr("openssl generate rsa key failed");
    return false;
  }

  AutoFreeBIO auto_bio;
  BIO *bio = auto_bio.Get();
  if (bio == nullptr) {
    SetLastErr("openssl new bio failed");
    return false;
  }

  if (PEM_write_bio_RSAPublicKey(bio, rsa_) == 0) {
    SetLastErr("openssl pem write public key failed");
    return false;
  }

  static std::vector<char> tmp_buf;
  if (tmp_buf.empty())
    tmp_buf.resize(1024 * 4);
  auto bio_size = BIO_read(bio, tmp_buf.data(), tmp_buf.size());
  if (bio_size == 0) {
    SetLastErr("openssl bio read buf failed");
    return false;
  }
  pub_key.k.resize(bio_size);
  memcpy_s(pub_key.k.data(), pub_key.k.size(), tmp_buf.data(), bio_size);

  return true;
}

bool CTokenPrivate::Encrypt(void* in, size_t in_size,
                            void *out, size_t out_size) {
  return true;
}

bool CTokenPrivate::Decrypt(void* in, size_t in_size,
                            void *out, size_t out_size) {
  return true;
}

#pragma endregion

#pragma region Public

bool CTokenPublic::InitKey(const STokenPublicKey &key) {
  AutoFreeBIO auto_bio;
  BIO *bio = auto_bio.Get();
  if (bio == nullptr) {
    SetLastErr("openssl new bio failed");
    return false;
  }

  if (BIO_write(bio, key.k.data(), key.k.size()) == 0) {
    SetLastErr("openssl write bio failed");
    return false;
  }

  rsa_ = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
  if (rsa_ == nullptr) {
    SetLastErr("openssl read pem rsa public key failed");
    return false;
  }

  return true;
}

bool CTokenPublic::Encrypt(void* in, size_t in_size,
                           void *out, size_t out_size) {
  return true;
}

bool CTokenPublic::Decrypt(void* in, size_t in_size,
                           void *out, size_t out_size) {
  return true;
}

#pragma endregion

#pragma region
}
}
#pragma endregion
