#include "stdafx.h"
#include "test_server_sql.h"
#include "im_server_sql.h"

#include "base/debug.hpp"


#pragma region namespace
namespace test {
#pragma endregion

void TestSetUserInfo(im::server_sql::CServerSql_User &sql,
                     im::server_sql::pSSqlUserInfo info) {
  base::b_async::Condition con;
  bool rc_code = 0;
  std::wstring err_msg;
  auto func = [&con, &rc_code, &err_msg] (bool suc, std::wstring err) {
    rc_code = suc;
    err_msg = err;
    con.NotifyOne();
  };
  sql.SetUserInfo(info, func);
  con.Wait();

  if (rc_code == true)
    base::debug::OutPut("write user sucess");
  else
    base::debug::OutPut(L"write user failed, rc_code:%d, des:%S",
                        rc_code, err_msg.c_str());
}

void TestSetUserInfos(im::server_sql::CServerSql_User &sql) {
  srand((unsigned int)time(0));
  std::wstring buf;
  buf.resize(256);
  auto info = std::make_shared<im::server_sql::SSqlUserInfo>();

  for (int i = 0; i < 100; i++) {
    swprintf_s((wchar_t *)buf.c_str(), buf.size(),
               L"test_%d@gmail.com", rand() % 100000);
    info->email = buf.c_str();

    swprintf_s((wchar_t *)buf.c_str(), buf.size(),
               L"test_%d", rand() % 10000);
    info->name = buf.c_str();

    swprintf_s((wchar_t *)buf.c_str(), buf.size(),
               L"15912%06d", rand() % 1000000);
    info->mobile = buf.c_str();

    swprintf_s((wchar_t *)buf.c_str(), buf.size(),
               L"123456%06d", rand() % 1000000);
    std::string str_buf = base::Utf16ToUtf8(buf);

    TestSetUserInfo(sql, info);
  }
}

void TestGetUserInfo(im::server_sql::CServerSql_User &sql) {
  im::server_sql::pSSqlUserInfo info;
  std::wstring err_msg;

  base::b_async::Condition con;
  auto func = [&info, &err_msg, &con]
  (im::server_sql::pSSqlUserInfo ref_info, std::wstring err) {
    con.NotifyOne();
  };
  sql.GetUserInfo(L"test_4726@gmail.com", L"",
                  L"0g/4Yp7IzsWbZgK8+Ap77I/rzX4BY/wWv7xQAugo5+w=",
                  func);
  con.Wait();
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

  //TestSetUserInfos(sql);
  TestGetUserInfo(sql);


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
