#include "stdafx.h"
#include "mqtt_client.h"

#pragma region
namespace im {
#pragma endregion

IMMqttClient::IMMqttClient(IN SMqttClientInitArgs &args):
  base::Log(args.func_log),
  host_(args.host),
  topic_(new mqtt::string_collection(args.topic)),
  func_connected_(args.func_connected),
  func_connection_lost_(args.func_connection_lost),
  func_faild_(args.func_faild) {}

bool IMMqttClient::Connect() {
  PrintInfo("connect begin");

  connOpts_ = std::make_shared<mqtt::connect_options>();
  connOpts_->set_keep_alive_interval(20);
  connOpts_->set_clean_session(false);
  connOpts_->set_automatic_reconnect(true);

  cli_ = std::make_shared<mqtt::async_client>(host_, CLIENT_ID);
  cli_->set_callback(*this);

  if (!BaseConnect())
    return false;

  return true;
}

void IMMqttClient::WaitConnected() {
  if (subscribe_token_)
    subscribe_token_->wait();
  subscribe_token_ = nullptr;
}

bool IMMqttClient::Disconnect() {
  if (cli_ == nullptr) {
    SetLastErr("cli_ is nullptr");
    return false;
  }

  try {
    std::cout << "\nDisconnecting from the MQTT server..." << std::flush;
    cli_->disconnect()->wait();
  }
  catch (const mqtt::exception& exc) {
    PrintErro("mqtt disconnect faild: %s", exc.what());
    SetLastErr("mqtt disconnect faild: %s", exc.what());
    return false;
  }
  cli_ = nullptr;

  return true;
}

bool IMMqttClient::SendMsg(std::string &topic, std::string &msg) {
  mqtt::message_ptr pubmsg = mqtt::make_message(topic, msg);
  pubmsg->set_qos(1);
  cli_->publish(pubmsg);

  return true;
}

#pragma region Callback

void IMMqttClient::OnConSuccess(const mqtt::token* tok) {
  BaseSubscribe();
}

void IMMqttClient::OnConFailure(const mqtt::token* tok) {
  PrintWarn("Connection failed");
  if (++nretry_ > max_nretry_) {
    // ֪ͨ...
    if (func_faild_)
      func_faild_(L"Reconnect more than 5 times");
  }
  Reconnect();
}

void IMMqttClient::OnSubSuccess(const mqtt::token* tok) {
  PrintInfo("Subscribe success, the message id %d", tok->get_message_id());
}

void IMMqttClient::OnSubFailure(const mqtt::token* tok) {
  PrintWarn("Subscribe failure, the message id %d", tok->get_message_id());
  if (func_faild_)
    func_faild_(L"Subscribe failure");
}

void IMMqttClient::connected(const std::string& cause) {
  PrintInfo("connected des:%s", cause.c_str());
  if (func_connected_)
    func_connected_();
}

void IMMqttClient::connection_lost(const std::string& cause) {
  PrintInfo("connection lost des:%s", cause.c_str());

  bool auto_rc = true;
  if (func_connection_lost_)
    auto_rc = func_connection_lost_();

  if (auto_rc) {
    nretry_ = 0;
    Reconnect();
  }
}

void IMMqttClient::message_arrived(mqtt::const_message_ptr msg) {
  PrintInfo("message arrived");
  std::cout << "Message" << std::endl;
}

void IMMqttClient::delivery_complete(mqtt::delivery_token_ptr tok) {
  PrintInfo("delivery complete");
}

#pragma endregion

#pragma region Base

bool IMMqttClient::BaseConnect() {
  if (cb_connect_ == nullptr) {
    cb_connect_ = std::make_shared<ActionListener>(
      [this] (const mqtt::token* tok) {this->OnSubSuccess(tok); },
      [this] (const mqtt::token* tok) {this->OnSubFailure(tok); });
  }

  try {
    cli_->connect(*connOpts_, nullptr,
      (mqtt::iaction_listener&)*cb_connect_);
  }
  catch (const mqtt::exception& exc) {
    PrintErro("mqtt subscribe faild: %s", exc.what());
    SetLastErr("mqtt connect faild: %s", exc.what());
    return false;
  }
  return true;
}

bool IMMqttClient::BaseSubscribe() {
  if (cb_subscribe_ == nullptr) {
    cb_subscribe_ = std::make_shared<ActionListener>(
      [this] (const mqtt::token* tok) {this->OnSubSuccess(tok); },
      [this] (const mqtt::token* tok) {this->OnSubFailure(tok); });
  }

  try {
    std::vector<int> qos = {1};
    subscribe_token_ = cli_->subscribe(topic_, {1}, nullptr,
      (mqtt::iaction_listener&)*cb_subscribe_);
  }
  catch (const mqtt::exception& exc) {
    PrintErro("mqtt subscribe faild: %s", exc.what());
    SetLastErr("mqtt subscribe faild: %s", exc.what());
    return false;
  }
  return true;
}

void IMMqttClient::Reconnect() {
  if (!BaseConnect()) {
    if (func_faild_)
      func_faild_(L"Reconnect faild");
  }
}

#pragma endregion

#pragma region
}
#pragma endregion

