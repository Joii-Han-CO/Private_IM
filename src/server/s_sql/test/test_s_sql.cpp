#include "pch.h"
#include "test_s_sql.h"
#include "im_s_sql.h"

#pragma region namespace
namespace test {
#pragma endregion

  void test_C(){
    std::cout << "test_s_sql_C Begin:" << std::endl;
    im::s_sql::Test_s_sql_C(8);
    std::cout << "test_s_sql_C Begin:" << std::endl;
  }

#pragma region namespace
}
#pragma endregion

int main() {
  
  test::test_C();

  return 0;
}
