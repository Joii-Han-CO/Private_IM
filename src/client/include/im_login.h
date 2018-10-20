#pragma once
#include "base/type_def.h"
#include <string>
#include <functional>
#include "im_config.h"
#include "base/error.hpp"


#pragma region define

namespace im {
class CMqttClientBase;
typedef std::shared_ptr<CMqttClientBase> pCMqttClientBase;
}

#pragma endregion


#pragma region
namespace im {
namespace login {
#pragma endregion

  //////////////////////////////////////////////////////////////////////////
  //  Login 模块
  //    该模块创建消息模块...
  //////////////////////////////////////////////////////////////////////////

// 异步完成回调
typedef std::function<void(bool)> LoginFuncResult;

class Login: public base::error::LastError {
public:
  Login();
  ~Login();

public:
  // 全局初始化(部分异步处理)
  bool Init(LoginFuncResult func_done);
#pragma region Init

private:
  bool InitMqtt();

#pragma endregion

public:
  // 设置用户名密码
  bool SetUserName(std::wstring name, std::wstring pwd,
                   LoginFuncResult func_done);

private:
  im::config::pCConfig global_config_; // 全局配置
  im::pCMqttClientBase mqtt_client_;
};

#pragma region
}
}
#pragma endregion
