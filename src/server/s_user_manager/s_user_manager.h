#pragma once

namespace im {
namespace s_user_manager {

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

}
}
