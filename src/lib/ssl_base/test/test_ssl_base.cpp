#include "stdafx.h"
#include "test_ssl_base.h"
#include "im_ssl_base.h"
#include "im_ssl_base.h"
#include "base/debug.hpp"

#pragma region namespace
namespace test {
#pragma endregion

void FillingNum(unsigned char *d, int l) {
  for (int i = 0; i < l; i++)
    d[i] = (unsigned char)i;
}

// 测试加密的单元
//  arg1 加密函数
//  arg2 解密函数
//  arg3 需要测试的数据长度，0为随机值
bool TestDECell(
  std::function<bool(const void *in, int in_len,
                     void *out, int &out_len,
                     void *out_ext, int &out_ext_len)> func_encrypt,
  std::function<bool(const void *in, int in_len,
                     const void *in_ext, int in_ext_len,
                     void *out, int &out_len)> func_decrypt,
  int buf_size) {

  if (buf_size == 0)
    buf_size = base::debug::GetRandInt(1, 2048);
  base::debug::OutPut("Test Buffer Size:%d", buf_size);

  std::vector<char> buf1, buf2, buf3;
  buf1.resize(buf_size);
  buf2.resize(buf_size + 512);
  buf3.resize(buf_size + 512);
  //base::debug::FillingRand(buf1.data(), buf1.size());
  FillingNum((unsigned char*)buf1.data(), (int)buf1.size());
  buf1[0] = 0;

  std::vector<char> buf_ext;
  buf_ext.resize(1024);

  int out_len = buf2.size();
  int out_ext_len = buf_ext.size();
  if (func_encrypt(buf1.data(), buf_size, buf2.data(), out_len,
                   buf_ext.data(), out_ext_len) == false)
    return false;

  if (func_decrypt(buf2.data(), out_len,
                   buf_ext.data(), out_ext_len,
                   buf3.data(), out_len) == false)
    return false;

  //对比
  for (int i = 0; i < buf_size; i++) {
    if (buf1[i] != buf3[i]) {
      base::debug::OutPut("Diff dencrypt data failed, %d", i);
      return false;
    }
  }

  return true;
}

void TestSHA256() {
  const std::string d = "0123456789abcdef";
  auto sha256 = im::ssl_base::GetSHA256(d.c_str(), d.size());
  auto s = im::ssl_base::Base64Encode(sha256.data(), sha256.size());

}

void TestRSA1() {
  im::ssl_base::CRSAED rsa_pri;
  auto pub_key = rsa_pri.GetPublicKey();
  im::ssl_base::CRSAED rsa_pub(pub_key);

  auto block_size = rsa_pri.GetBlockSize();

  auto TestRSACell = [&rsa_pri, &rsa_pub, block_size] (int data_size) {
    if (data_size <= 0) {
      base::debug::OutPut("block_size too short");
      return false;
    }
    base::debug::OutPut("Test Data Size : %d", data_size);

    auto en_data_size = rsa_pri.GetEncryptSize(data_size);
    if (en_data_size == -1) {
      base::debug::OutPut("Get Encrypt Size Faile");
      return false;
    }

    std::vector<char> tmp_buf1, tmp_buf2, tmp_buf3;
    tmp_buf1.resize(data_size);
    tmp_buf2.resize(en_data_size);
    tmp_buf3.resize(data_size);

    { // 填充
      //FillingNum((unsigned char*)tmp_buf1.data(), tmp_buf1.size());
      base::debug::FillingRand(tmp_buf1.data(), tmp_buf1.size());
    }

    int out_len = 0;
    { // 加密
      out_len = tmp_buf2.size();
      if (rsa_pri.Encrypt_Pri(tmp_buf1.data(), tmp_buf1.size(),
                              tmp_buf2.data(), out_len) == false) {
        base::debug::OutPut("Private Key Encrypt Failed, DES:%s",
                            rsa_pri.GetLastErr_Astd().c_str());
        return false;
      }
    }
    { // 解密
      int out_dc_len = tmp_buf3.size();
      if (rsa_pub.Decrypt_Pub(tmp_buf2.data(), out_len,
                              tmp_buf3.data(), out_dc_len) == false) {
        base::debug::OutPut("Public Key Encrypt Failed, DES:%s",
                            rsa_pub.GetLastErr_Astd().c_str());
        return false;
      }
    }

    { // 校验数据是否正确
      for (int i = 0; i < data_size; i++) {
        if (tmp_buf1[i] != tmp_buf3[i]) {
          base::debug::OutPut("Diff Data Failed, %d", i);
          return false;
        }
      }
    }

    return true;
  };

  if (TestRSACell(123) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestRSACell(block_size) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestRSACell(block_size + 100) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestRSACell(block_size * 7) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestRSACell(block_size * 7 + 23) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  base::debug::OutPut("Test RSA Success!");
}

void TestRSA2() {
  im::ssl_base::CRSAED rsa_pri;
  auto pub_key = rsa_pri.GetPublicKey();
  im::ssl_base::CRSAED rsa_pub(pub_key);

  auto block_size = rsa_pri.GetBlockSize();

  auto TestRSACell = [&rsa_pri, &rsa_pub, block_size] (int data_size) {
    if (data_size <= 0) {
      base::debug::OutPut("block_size too short");
      return false;
    }
    base::debug::OutPut("Test Data Size : %d", data_size);

    auto en_data_size = rsa_pri.GetEncryptSize(data_size);
    if (en_data_size == -1) {
      base::debug::OutPut("Get Encrypt Size Faile");
      return false;
    }

    std::vector<char> tmp_buf1, tmp_buf2, tmp_buf3;
    tmp_buf1.resize(data_size);
    tmp_buf2.resize(en_data_size);
    tmp_buf3.resize(data_size);

    { // 填充
      //FillingNum((unsigned char*)tmp_buf1.data(), tmp_buf1.size());
      base::debug::FillingRand(tmp_buf1.data(), tmp_buf1.size());
    }

    int out_len = 0;
    { // 加密
      out_len = tmp_buf2.size();
      if (rsa_pub.Encrypt_Pub(tmp_buf1.data(), tmp_buf1.size(),
                              tmp_buf2.data(), out_len) == false) {
        base::debug::OutPut("Public Key Encrypt Failed, DES:%s",
                            rsa_pub.GetLastErr_Astd().c_str());
        return false;
      }
    }
    { // 解密
      int out_dc_len = tmp_buf3.size();
      if (rsa_pri.Decrypt_Pri(tmp_buf2.data(), out_len,
                              tmp_buf3.data(), out_dc_len) == false) {
        base::debug::OutPut("Private Key Encrypt Failed, DES:%s",
                            rsa_pri.GetLastErr_Astd().c_str());
        return false;
      }
    }

    { // 校验数据是否正确
      for (int i = 0; i < data_size; i++) {
        if (tmp_buf1[i] != tmp_buf3[i]) {
          base::debug::OutPut("Diff Data Failed, %d", i);
          return false;
        }
      }
    }

    return true;
  };

  if (TestRSACell(123) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestRSACell(block_size) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestRSACell(block_size + 100) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestRSACell(block_size * 7) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestRSACell(block_size * 7 + 23) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  base::debug::OutPut("Test RSA Success!");
}

void TestAES() {
  int block_size = im::ssl_base::g_aes_block_size_;

  auto TestAesCell = [block_size] (int buf_size) {
    if (buf_size == 0) {
      base::debug::OutPut("buf_size too short");
    }

    //生成key
    std::vector<char> key;
    key.resize(im::ssl_base::g_aes_key_size_ / 8);
    base::debug::FillingRand(key.data(), key.size());
    im::ssl_base::CAESED aes_d(key.data());

    std::vector<char> buf1, buf2, buf3;
    int es_out_len = aes_d.GetEncryptSize(buf_size);
    buf1.resize(buf_size);
    buf2.resize(es_out_len);
    buf3.resize(es_out_len);
    base::debug::FillingRand(buf1.data(), buf1.size());

    std::vector<char> buf_vi;
    buf_vi.resize(16);
    base::debug::FillingRand(buf_vi.data(), buf_vi.size());

    // 加密
    if (aes_d.Encrypt(buf1.data(), buf1.size(), buf_vi.data(),
                      buf2.data(), buf2.size()) == false) {
      base::debug::OutPut("Test Aes Failed, Encrypt, Des %s",
                          aes_d.GetLastErr_Astd().c_str());
      return false;
    }

    int bf3_len = buf3.size();
    if (aes_d.Decrypt(buf2.data(), buf2.size(), buf_vi.data(),
                      buf3.data(), buf3.size()) == false) {
      base::debug::OutPut("Test Aes Failed, Decrypt, Des %s",
                          aes_d.GetLastErr_Astd().c_str());
      return false;
    }

    for (int i = 0; i < (int)buf1.size(); i++) {
      if (buf1[i] != buf3[i]) {
        base::debug::OutPut("Diff Data Failed, %d", i);
        return false;
      }
    }

    base::debug::OutPut("Test Aes Success!");
    return true;
  };

  if (TestAesCell(12) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestAesCell(block_size) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestAesCell(block_size + 2) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestAesCell(block_size * 7) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  if (TestAesCell(block_size * 7 + 6) == false) {
    base::debug::OutPut("Run Cell Failed");
    return;
  }

  base::debug::OutPut("Test Aes Success!");

}

void TestToken() {
  auto token_pri = std::make_shared<im::ssl_base::CTokenPrivate>();
  auto token_pub = std::make_shared<im::ssl_base::CTokenPublic>();

  im::ssl_base::STokenPublicKey pub_key;

  if (token_pri->GenerateKey(pub_key) == false) {
    base::debug::OutPut("GenerateKey failed");
  }

  if (token_pub->InitKey(pub_key) == false) {
    base::debug::OutPut("InitKey failed");
  }

  // 尝试加密数据
  std::vector<char> tmp_buf1;
  std::vector<char> tmp_buf2;
  std::vector<char> tmp_buf3;

  tmp_buf1.resize(123123);
  tmp_buf2.resize(123123);
  tmp_buf3.resize(123123);

  token_pri->Encrypt(tmp_buf1.data(), tmp_buf1.size(),
                     tmp_buf2.data(), tmp_buf2.size());
}

#pragma region namespace
}
#pragma endregion

int main() {
  base::debug::SetRand();

  //test::TestSHA256();
  //test::TestToken();
  test::TestRSA1();
  test::TestRSA2();
  //test::TestAES();

  system("pause");
  return 0;
}
