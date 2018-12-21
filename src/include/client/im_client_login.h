#pragma once
#include "base/type_def.h"
#include <string>
#include <vector>
#include <functional>
#include "im_config.h"
#include "mqtt_client_base.h"
#include "base/error.hpp"


#pragma region
namespace im {
namespace login {
#pragma endregion

  //////////////////////////////////////////////////////////////////////////
  //  Login 模块
  //    该模块创建消息模块...
  //////////////////////////////////////////////////////////////////////////

// 异步完成回调
typedef std::function<void(bool)> Func_LoginResult;

class Login: public base::error::LastError {
public:
  Login(im::FUNC_StatusChange func_mqtt_status);
  ~Login();

public:
  // 全局初始化(部分异步处理)
  //  密码部分使用sha256,因此传入string就可以了
  bool Init(std::wstring user_name, std::string user_pwd,
            Func_LoginResult func_done);

private:
  bool InitMqtt(std::wstring user_name, std::string user_pwd);

  void MqttStatus(im::EMqttOnlineStatus status);

  // 登陆成功之后订阅
  void SubLoginChannel();
  void SubLoginChannelFinished();

  void LoginMsg(std::vector<char> data);

public:

private:
  im::config::pCConfig global_config_; // 全局配置
  im::pCMqttClientBase mqtt_client_;

  im::FUNC_StatusChange func_mqtt_status_;  // mqtt 状态更新
};

#pragma region
}
}
#pragma endregion
