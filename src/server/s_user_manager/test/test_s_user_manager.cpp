#include "pch.h"
#include "test_s_user_manager.h"
#include "im_s_user_manager.h"

#pragma region namespace
namespace test {
#pragma endregion

void Test() {
  if (im::s_user_manager::InitServerUserManager() == false) {
    cout << "init server user manager failed" << endl;
    return;
  }

  _getch();

  if (im::s_user_manager::WaitAndExit() == false) {
    cout << "exit or wait exit faield" << endl;
    return;
  }
}

#pragma region namespace
}
#pragma endregion

int main() {

  test::Test();

  return 0;
}
