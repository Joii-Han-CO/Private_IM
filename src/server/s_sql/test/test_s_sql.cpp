#include "pch.h"
#include "test_s_sql.h"
#include "im_s_sql.h"



#pragma region namespace
namespace test {
#pragma endregion

void Test() {
  im::s_sql::CSqlManager::Get()->Init(L"tcp://127.0.0.1:3306",
                                      L"root", L"x", L"pim_db");

  auto info = im::s_sql::CSqlManager::Get()->GetUserInfo(L"server1", 
                                                         L"123456");

  im::s_sql::CSqlManager::Get()->Uninit();
}

#pragma region namespace
}
#pragma endregion

int main() {


  test::Test();

  return 0;
}
