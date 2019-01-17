#pragma once


#pragma region namespace
namespace server {
#pragma endregion

class ServerUserLogin {
public:
  ServerUserLogin(std::wstring user_name, std::wstring channel_name,
                  im::pCMqttClientBase mqtt);
  ~ServerUserLogin();

  bool Init();

private:
  void RecvMsg(const MsgBuf &buf);

private:
  std::wstring user_name_;
  std::wstring channel_name_;
  im::pCMqttClientBase mqtt_;
};
StdSharedPtr_Typedef(ServerUserLogin);

#pragma region namespace
}
#pragma endregion
