#include "stdafx.h"
#include "im_ssl_base.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "base/rand.hpp"

// Token

#pragma region
namespace im {
namespace ssl_base {
#pragma endregion

struct SToken_PublicBuff {
  unsigned int version;
  struct _KEY {
    char symmetry_key[g_aes_key_size_ / 8];
    char symmetry_vi[g_aes_block_size_];

    int asymmerty_public_key_size;
    char asymmerty_public_key;
  } key;
};
typedef std::shared_ptr<SToken_PublicBuff> pSToken_PublicBuff;

struct SToken_EncryDataHeader {
  int version;
  int size;
  char sha256[32];

  char key[g_aes_key_size_ / 8];
  char vi[g_aes_block_size_];
};

// 这里直接写一组UUID，以后再考虑其他办法
const char *g_init_key_ = "a42f74a027964fd5b23d5186315adaa3";
const char *g_init_vi_ = "142e82f0ae5b4d5c93af6585b94454c";

CToken::CToken() {
  base::im_rand::SetRand();
  InitPrivateKey(pri_);
}

std::vector<char> CToken::GetPublicKey() {

  // 公钥
  auto pub_key = pri_.a->GetPublicKey();
  if (pub_key.empty()) {
    SetLastErr("get rsa public key failed");
    return std::vector<char>();
  }

  int test_buf = sizeof(SToken_PublicBuff);

  // buffer
  CheckoutSHA256 check_buf(sizeof(SToken_PublicBuff) + pub_key.size());
  SToken_PublicBuff *pub_buf = (SToken_PublicBuff*)check_buf.GetBuffer();
  char *pub_key_buf = &pub_buf->key.asymmerty_public_key;

  { // 填充各种Key
    pub_buf->version = g_token_version;
    auto s_key = pri_.s->GetKey();
    memcpy_s(
      pub_buf->key.symmetry_key, sizeof(SToken_PublicBuff::_KEY::symmetry_key),
      s_key.data(), g_aes_key_size_ / 8);
    memcpy_s(
      pub_buf->key.symmetry_vi, sizeof(SToken_PublicBuff::_KEY::symmetry_vi),
      pri_.vi.data(), g_aes_block_size_);

    pub_buf->key.asymmerty_public_key_size = pub_key.size();
    memcpy_s(pub_key_buf, pub_key.size(),
             pub_key.data(), pub_key.size());
    check_buf.UpdateSha256();
  }

  std::vector<char> ref_buf;

  { // 加密
    CAESED ed((void*)g_init_key_);
    int out_size = ed.GetEncryptSize_Ex(check_buf.GetAllBufferSize());
    ref_buf.resize(out_size);
    if (ed.Encrypt_Ex(
      check_buf.GetAllBuffer(), check_buf.GetAllBufferSize(),
      (void*)g_init_vi_,
      ref_buf.data(), ref_buf.size()) == false) {
      SetLastErr("encrypt failed, des:%s", ed.GetLastErr_Astd().c_str());
      return std::vector<char>();
    }
  }
  return ref_buf;
}

bool CToken::SetPublicKey(std::vector<char> &data) {
  if (data.size() % g_aes_block_size_ != 0) {
    SetLastErr("data size failed");
    return false;
  }
  if ((int)data.size() < CheckoutSHA256::GetMinBufferSize()) {
    SetLastErr("data size too short");
    return false;
  }

  std::vector<char> data_e;
  { // 解密
    data_e.resize(data.size());

    auto init_aes = std::make_unique<CAESED>((void*)g_init_key_);
    init_aes->Decrypt_Ex(data.data(), data.size(), (void*)g_init_vi_,
                         data_e.data(), data_e.size());
  }

  CheckoutSHA256 check_buf(data_e.data(), data_e.size());

   // sha256校验
  if (check_buf.CheckoutSha256() == false) {
    SetLastErr("decrypt buff checked sha256 failed");
    return false;
  }

  SToken_PublicBuff *pub_buf = (SToken_PublicBuff*)check_buf.GetBuffer();

  // 校验版本号
  if (pub_buf->version != g_token_version) {
    SetLastErr(
      "version number does not match, now version:%d, this version:%d",
      g_token_version, pub_buf->version);
    return false;
  }

  // 校验公钥长度
  if (data_e.size() <
      sizeof(SToken_PublicBuff) + pub_buf->key.asymmerty_public_key_size) {
    SetLastErr("data too short 2");
    return false;
  }

  { // 设置各个key
    std::vector<char> s_key, s_vi;
    s_key.resize(g_aes_key_size_ / 8);
    s_vi.resize(g_aes_block_size_);
    memcpy_s(pub_buf->key.symmetry_key, g_aes_key_size_ / 8,
             s_key.data(), sizeof(SToken_PublicBuff::_KEY::symmetry_key));
    memcpy_s(pub_buf->key.symmetry_vi, g_aes_block_size_,
             s_vi.data(), sizeof(SToken_PublicBuff::_KEY::symmetry_vi));

    std::vector<char> pub_key;
    pub_key.resize(pub_buf->key.asymmerty_public_key_size);
    memcpy_s(pub_key.data(), pub_key.size(),
             &pub_buf->key.asymmerty_public_key,
             pub_buf->key.asymmerty_public_key_size);

    pub_.a = std::make_shared<CRSAED>(pub_key);
    pub_.s = std::make_shared<CAESED>(s_key.data());
    pub_.vi = s_vi;
  }
  return true;
}

int CToken::GetEncryptSize(int bufsize) {
  return pub_.a->GetEncryptSize(sizeof(SToken_EncryDataHeader)) +
    pub_.s->GetEncryptSize(bufsize);
}

int CToken::Encrypt(IN const void *in, IN int in_len,
                    OUT void *out, IN int out_len) {
  SToken_EncryDataHeader header;
  {
    header.version = g_token_version;
    header.size = in_len;
    // 生成加密信息
    auto sha = GetSHA256(in, in_len);
    memcpy_s(header.sha256, sizeof(SToken_EncryDataHeader::sha256),
             sha.data(), sha.size());

    base::im_rand::FillingRand(header.key, sizeof(header.key));
    base::im_rand::FillingRand(header.vi, sizeof(header.vi));
  }
  int rsa_out_len = out_len;
  if (pub_.a->Encrypt_Pub(&header, sizeof(header), out, rsa_out_len)
      == false) {
    SetLastErr("rsa encrypt failed, des:%s",
               pub_.a->GetLastErr_Astd().c_str());
    return false;
  }
  if (rsa_out_len > out_len) {
    SetLastErr("rsa encrypt failed, des: out buffer len failed");
    return false;
  }

  char *out_body = &(((char*)out)[rsa_out_len]);

  CAESED ed(header.key);
  if (ed.Encrypt(in, in_len, header.vi, out_body, out_len - rsa_out_len)
      == false) {
    SetLastErr("aes encrypt failed, des:%s", ed.GetLastErr_Astd().c_str());
    return false;
  }

  return true;
}

int CToken::Decrypt(IN const void *in, IN int in_len,
                    OUT void **out, IN int *out_len) {
  if (in_len < sizeof(SToken_EncryDataHeader)) {
    SetLastErr("in buffer too short");
    return false;
  }

  int es_header_size = pri_.a->GetEncryptSize(sizeof(SToken_EncryDataHeader));
  std::vector<char> header_buf;
  header_buf.resize(es_header_size);
  int es_out_size = es_header_size;
  if (pri_.a->Decrypt_Pri(in, es_header_size,
                          header_buf.data(), es_out_size) == false) {
    SetLastErr("rsa decrypt failed, des:%s",
               pri_.a->GetLastErr_Astd().c_str());
    return false;
  }
  SToken_EncryDataHeader *header = (SToken_EncryDataHeader*)header_buf.data();
  if (header->version != g_token_version) {
    SetLastErr(
      "version number does not match, now version:%d, this version:%d",
      g_token_version, header->version);
    return false;
  }

  char *in_s = &((char*)in)[es_header_size];
  CAESED ed(header->key);
  char *out_buf = new char[ed.GetEncryptSize(header->size)];
  if (ed.Decrypt(in_s, in_len - es_header_size, header->vi,
                 out_buf, header->size) == false) {
    SetLastErr("aes decrypt failed, des:%s",
               ed.GetLastErr_Astd().c_str());
    return false;
  }

  auto sha = GetSHA256(out_buf, header->size);
  for (int i = 0; i < sizeof(SToken_EncryDataHeader::sha256); i++) {
    if (sha[i] != header->sha256[i]) {
      return false;
    }
  }

  *out = out_buf;
  *out_len = header->size;
  return true;
}

void CToken::ReleaseDecryptBuffer(IN void *out, IN int out_len) {
  delete[] out;
}

void CToken::InitPrivateKey(STokenKey &key) {
  key.a = std::make_shared<CRSAED>();

  std::vector<char> rand_key =
    base::im_rand::FillingRand(g_aes_key_size_ / 8);
  key.s = std::make_shared<CAESED>(rand_key.data());
  key.vi = base::im_rand::FillingRand(g_aes_block_size_);
}

#pragma region
}
}
#pragma endregion
