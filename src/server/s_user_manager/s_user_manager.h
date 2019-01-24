#pragma once

#pragma region namespace
namespace im {
namespace s_user_manager {
#pragma endregion

class CServerUserManager {
public:
  static CServerUserManager* Get();
private:
  CServerUserManager();
public:
  ~CServerUserManager();

public:
  bool Init();
  bool Exit();
  bool WaitExit();

};

#pragma region namespace
}
}
#pragma endregion
