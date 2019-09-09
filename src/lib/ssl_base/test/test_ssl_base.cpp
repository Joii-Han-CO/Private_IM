#include "stdafx.h"
#include "test_ssl_base.h"
#include "im_ssl_base.h"
#include "im_ssl_base.h"


#pragma region namespace
namespace test {
#pragma endregion

void TestSHA256() {
  const std::string d = "0123456789abcdef";
  auto sha256 = im::ssl_base::GetSHA256(d.c_str(), d.size());
  auto s = im::ssl_base::Base64Encode(sha256.data(), sha256.size());

}

#pragma region namespace
}
#pragma endregion

int main() {

  test::TestSHA256();

  return 0;
}
