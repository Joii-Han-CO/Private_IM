#pragma once
#include "mqtt/client.h"
#include "base/type_def.h"
#include "base/error.hpp"
#include "base/log.hpp"


#pragma region
namespace im {
#pragma endregion

const std::string CLIENT_ID("sync_subcribe_cpp");

struct SMqttClientInitArgs {
  std::string host;
  std::string topic;

  std::function<void()> func_connected;
  std::function<bool()> func_connection_lost;
  std::function<void(std::wstring)> func_faild;
  base::LogCallback func_log;
};

class IMMqttClient:
  public virtual mqtt::callback,
  public virtual base::LastError,
  public virtual base::Log {
public:
  IMMqttClient(IN SMqttClientInitArgs &args);

  // 连接Mqtt服务器
  bool Connect();

  // 等待连接完成
  void WaitConnected();

  // 断开与服务器的连接
  bool Disconnect();

  bool SendMsg(std::string &topic, std::string &msg);

protected:
  typedef std::function<void(const mqtt::token*)> ActionListenerFunc;
  class ActionListener: public virtual mqtt::iaction_listener {
  public:
    ActionListener(ActionListenerFunc success, ActionListenerFunc failure):
      success_(success), failure_(failure) {};
  protected:
    void on_success(const mqtt::token& tok) override {
      if (success_)
        success_(&tok);
    }
    void on_failure(const mqtt::token& tok) override {
      if (failure_)
        failure_(&tok);
    }
  private:
    ActionListenerFunc success_;
    ActionListenerFunc failure_;
  };
  void OnConSuccess(const mqtt::token* tok);
  void OnConFailure(const mqtt::token* tok);
  void OnSubSuccess(const mqtt::token* tok);
  void OnSubFailure(const mqtt::token* tok);
  void connected(const std::string& cause) override;
  void connection_lost(const std::string& cause) override;
  void message_arrived(mqtt::const_message_ptr msg) override;
  void delivery_complete(mqtt::delivery_token_ptr tok) override;


private:
  bool BaseConnect();
  bool BaseSubscribe();

  void Reconnect();

private:
  std::string host_;
  mqtt::string_collection_ptr topic_;

  std::function<void()> func_connected_;
  std::function<bool()> func_connection_lost_;
  std::function<void(std::wstring)> func_faild_;

  mqtt::connect_options_ptr connOpts_;
  mqtt::async_client_ptr cli_;
  std::shared_ptr<ActionListener> cb_connect_;
  std::shared_ptr<ActionListener> cb_subscribe_;

  mqtt::token_ptr subscribe_token_;

  int nretry_ = 0;  // 断线重试次数
  const int max_nretry_ = 5;
};

#pragma region
}
#pragma endregion
