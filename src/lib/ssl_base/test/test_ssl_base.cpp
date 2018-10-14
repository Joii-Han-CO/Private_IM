#include "stdafx.h"
#include "test_ssl_base.h"
#include "im_ssl_base.h"
#include "im_ssl_base.h"
#include "base/debug.hpp"
#include "base/rand.hpp"

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
                     void *out, int out_len, std::string &err)> func_encrypt,
  std::function<bool(const void *in, int in_len,
                     void *out, int out_len, std::string &err)> func_decrypt,
  std::function<int(int)> func_get_encrypt_buf_size,
  int buf_size,
  std::string header) {

  if (func_encrypt == nullptr || func_decrypt == nullptr ||
      func_get_encrypt_buf_size == nullptr || buf_size == 0) {
    base::debug::OutPut("TestDECell Args Failed");
    return false;
  }

  int es_out_size = func_get_encrypt_buf_size(buf_size);
  std::vector<char> b1, b2, b3;
  b1 = base::im_rand::FillingRand(buf_size);
  b1.resize(buf_size);
  b2.resize(es_out_size);
  b3.resize(es_out_size);

  std::string err;

  if (func_encrypt(b1.data(), b1.size(), b2.data(), b2.size(), err) == false) {
    base::debug::OutPut("Test %s Failed, Encrypt Des:%s",
                        header.c_str(), err.c_str());
    return false;
  }
  if (func_decrypt(b2.data(), b2.size(), b3.data(), b3.size(), err) == false) {
    base::debug::OutPut("Test %s Failed, Decrypt Des:%s",
                        header.c_str(), err.c_str());
    return false;
  }
  for (int i = 0; i < buf_size; i++) {
    if (b1[i] != b3[i]) {
      base::debug::OutPut("Test %s Failed, Checkout 1 3 Buff Diff:%d-%d",
                          header.c_str(), (int)b1[i], (int)b3[i]);
      return false;
    }
  }

  base::debug::OutPut("Test %s Success!", header.c_str());
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

    auto func_en = [&rsa_pri, &rsa_pub]
    (const void *in, int in_len, void *out, int out_len, std::string &err) {
      if (rsa_pri.Encrypt_Pri(in, in_len, out, out_len) == false) {
        err = rsa_pri.GetLastErr_Astd();
        return false;
      }
      return true;
    };
    auto func_de = [&rsa_pri, &rsa_pub]
    (const void *in, int in_len, void *out, int out_len, std::string &err) {
      if (rsa_pub.Decrypt_Pub(in, in_len, out, out_len) == false) {
        err = rsa_pub.GetLastErr_Astd();
        return false;
      }
      return true;

    };
    auto func_get_encrypt_size = [&rsa_pri, &rsa_pub] (int buf_size) {
      return rsa_pri.GetEncryptSize(buf_size);
    };

    return TestDECell(func_en, func_de, func_get_encrypt_size,
                      data_size, "RSA1");
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

    auto func_en = [&rsa_pri, &rsa_pub]
    (const void *in, int in_len, void *out, int out_len, std::string &err) {
      if (rsa_pub.Encrypt_Pub(in, in_len, out, out_len) == false) {
        err = rsa_pub.GetLastErr_Astd();
        return false;
      }
      return true;
    };
    auto func_de = [&rsa_pri, &rsa_pub]
    (const void *in, int in_len, void *out, int out_len, std::string &err) {
      if (rsa_pri.Decrypt_Pri(in, in_len, out, out_len) == false) {
        err = rsa_pri.GetLastErr_Astd();
        return false;
      }
      return true;

    };
    auto func_get_encrypt_size = [&rsa_pri, &rsa_pub] (int buf_size) {
      return rsa_pri.GetEncryptSize(buf_size);
    };

    return TestDECell(func_en, func_de, func_get_encrypt_size,
                      data_size, "RSA2");
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
}

void TestAES() {
  int block_size = im::ssl_base::g_aes_block_size_;

  auto TestAesCell = [] (int buf_size) {
    if (buf_size == 0) {
      base::debug::OutPut("buf_size too short");
    }

    auto key = base::im_rand::FillingRand(im::ssl_base::g_aes_key_size_ / 8);
    auto vi = base::im_rand::FillingRand(im::ssl_base::g_aes_key_size_);
    im::ssl_base::CAESED aes_d(key.data());

    auto func_en = [&aes_d, &vi] (const void *in, int in_len,
                                  void *out, int out_len, std::string &err) {
      if (aes_d.Encrypt(in, in_len, vi.data(), out, out_len) == false) {
        err = aes_d.GetLastErr_Astd();
        return false;
      }
      return true;
    };
    auto func_de = [&aes_d, &vi] (const void *in, int in_len,
                                  void *out, int out_len, std::string &err) {
      if (aes_d.Decrypt(in, in_len, vi.data(), out, out_len) == false) {
        err = aes_d.GetLastErr_Astd();
        return false;
      }
      return true;
    };
    auto func_get_size = [&aes_d] (int buf_size) {
      return aes_d.GetEncryptSize(buf_size);
    };

    return TestDECell(func_en, func_de, func_get_size, buf_size, "AES");
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
}

void TestAESEx() {
  int block_size = im::ssl_base::g_aes_block_size_;

  auto TestAesCell = [] (int buf_size) {
    if (buf_size == 0) {
      base::debug::OutPut("buf_size too short");
    }

    auto key = base::im_rand::FillingRand(im::ssl_base::g_aes_key_size_ / 8);
    auto vi = base::im_rand::FillingRand(im::ssl_base::g_aes_key_size_);
    im::ssl_base::CAESED aes_d(key.data());

    auto func_en = [&aes_d, &vi] (const void *in, int in_len,
                                  void *out, int out_len, std::string &err) {
      if (aes_d.Encrypt_Ex(in, in_len, vi.data(), out, out_len) == false) {
        err = aes_d.GetLastErr_Astd();
        return false;
      }
      return true;
    };
    auto func_de = [&aes_d, &vi] (const void *in, int in_len,
                                  void *out, int out_len, std::string &err) {
      if (aes_d.Decrypt_Ex(in, in_len, vi.data(), out, out_len) == false) {
        err = aes_d.GetLastErr_Astd();
        return false;
      }
      return true;
    };
    auto func_get_size = [&aes_d] (int buf_size) {
      return aes_d.GetEncryptSize_Ex(buf_size);
    };

    return TestDECell(func_en, func_de, func_get_size, buf_size, "AES_EX");
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
}

void TestToken() {

  // 交换密钥
  im::ssl_base::CToken token1, token2;
  auto pub_buf1 = token1.GetPublicKey();
  token2.SetPublicKey(pub_buf1);
  auto pub_buf2 = token2.GetPublicKey();
  token1.SetPublicKey(pub_buf2);

  // 测试加密...
  auto TestCell = [&token1, &token2] (int buf_size) {
    if (buf_size == 0) {
      base::debug::OutPut("buf_size too short");
    }

    auto func_en = [&token1, &token2]
    (const void *in, int in_len, void *out, int out_len, std::string &err) {

      if (token1.Encrypt(in, in_len, out, out_len) == false) {
        err = token1.GetLastErr_Astd();
        return false;
      }

      return true;
    };
    auto func_de = [&token1, &token2]
    (const void *in, int in_len, void *out, int out_len, std::string &err) {
      void *out_buf = nullptr;
      int out_len_buf = 0;
      if (token2.Decrypt(in, in_len, &out_buf, &out_len_buf) == false) {
        err = token1.GetLastErr_Astd();
        return false;
      }

      // 对比
      if (out_len < out_len_buf) {
        base::debug::OutPut("out buffer too short");
        return false;
      }
      memcpy_s(out, out_len, out_buf, out_len_buf);
      token2.ReleaseDecryptBuffer(out_buf, out_len_buf);

      return true;
    };
    auto func_get_size = [&token1, &token2] (int buf_size) {
      return token1.GetEncryptSize(buf_size);
    };

    return TestDECell(func_en, func_de, func_get_size, buf_size, "Token");
  };

  TestCell(10);
  TestCell(im::ssl_base::g_aes_block_size_);
  TestCell(im::ssl_base::g_aes_block_size_ + 7);
  TestCell(im::ssl_base::g_aes_block_size_ * 5);
  TestCell(im::ssl_base::g_aes_block_size_ * 11 + 13);
  TestCell(im::ssl_base::g_rsa_block_size * 7);
  TestCell(im::ssl_base::g_rsa_block_size * 18 + 9);
}

#pragma region namespace
}
#pragma endregion

int main() {
  base::im_rand::SetRand();

  //test::TestSHA256();
  //test::TestRSA1();
  //test::TestRSA2();
  //test::TestAES();
  //test::TestAESEx();
  test::TestToken();

  system("pause");
  return 0;
}
