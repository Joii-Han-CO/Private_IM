#pragma once


#pragma region namespace
namespace server {
#pragma endregion

/*
登陆流程:

c-->s   公共通道发送客户端登陆信息
s-->c   私有通道发送成功消息
c-->s   返回收到信息

* 第一次私有通道主要是为了测试通道是否建立成功...
*/


class ServerUserLogin {
public:
  ServerUserLogin(uint32_t id,
                  std::wstring user_name, std::wstring channel_name,
                  im::pCMqttClient mqtt,
                  std::function<void(uint32_t)> exit_func);
  ~ServerUserLogin();

  bool Init();

  bool Uninit();

  bool SendMsg(cMsgBuf buf, const Func_AsyncResult func);

private:
  void RecvMsg(cMsgBuf buf);


private:
  bool SendLoginStatus();

  // 收到客户端登陆消息返回
  void ClientLoginRes(const im::msg_proto::pMsg_UserLoginClientRes msg);

  // 用户登陆成功之后触发
  void LoginSuccess();

  // 触发登出操作
  void LogoutExit();

private:
  uint32_t id_;
  std::wstring user_name_;
  std::wstring channel_name_;
  im::pCMqttClient mqtt_;
  std::function<void(uint32_t)> exit_func_ = nullptr;
};
StdSharedPtr_Typedef(ServerUserLogin);

#pragma region namespace
}
#pragma endregion
