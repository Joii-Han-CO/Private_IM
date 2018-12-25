#pragma once
#include "base/type_def.h"
#include <string>
#include <vector>
#include <functional>
#include "im_config.h"
#include "im_mqtt.h"
#include "base/error.hpp"


#pragma region
namespace im {
namespace login {
#pragma endregion

  //////////////////////////////////////////////////////////////////////////
  //  Login 模块
  //    该模块创建消息模块...
  //////////////////////////////////////////////////////////////////////////

class Login: public base::error::LastError {
public:
  Login();
  ~Login();

public:
  // 连接
  bool Connect(std::wstring user_name, std::string user_pwd,
               Func_AsyncResult func_done);

  // 断线重连
  bool Reconnect(Func_AsyncResult func_done);

  // 断开连接
  bool Disconnect(Func_AsyncResult func_done);

  // 注册状态变更的回调函数
  uint32_t RegStatusCallback(im::FUNC_StatusChange func);

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

//  std::list<>
};

#pragma region
}
}
#pragma endregion
