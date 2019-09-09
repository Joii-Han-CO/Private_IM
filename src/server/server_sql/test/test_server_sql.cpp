#include "stdafx.h"
#include "test_server_sql.h"
#include "im_server_sql.h"

#include "base/debug.hpp"


#pragma region namespace
namespace test {
#pragma endregion

void TestSetUserInfo(im::server_sql::CServerSql_User &sql) {
  im::server_sql::SSqlUserInfo info;

  info.email = L"test@gmail.com";
  info.mobile = L"13111111111";

  sql.SetUserInfo(info.GetPtr());
}

void TestManger() {

#pragma region Init
  base::debug::OutPut("TestSqlUser Begin...");
  im::server_sql::CServerSql_User sql(nullptr);
  const std::wstring db_path = L"test.db";
  base::b_async::Condition wait_cb;
  bool ref_val = true;
  auto func_init =
    [&wait_cb, &ref_val] (bool suc, const std::wstring &err) {
    ref_val = suc;
    wait_cb.NotifyOne();
  };
  sql.Init(db_path, func_init);
  wait_cb.Wait();
#pragma endregion

  TestSetUserInfo(sql);

#pragma region Release
  base::debug::WaitEnterGoon("Wait Enter Release");
  sql.Release();
  base::debug::WaitEnterGoon("Wait Enter Exit");
  base::debug::OutPut("TestSqlUser End");
#pragma endregion
}

#pragma region namespace
}
#pragma endregion

int main() {

  test::TestManger();

  return 0;
}
