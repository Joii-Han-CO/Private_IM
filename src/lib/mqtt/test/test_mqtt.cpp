#include "pch.h"
#include "test_mqtt.h"
#include "base/character_conversion.hpp"
#include "base/string_operation.hpp"
#include "base/debug.hpp"
#include "base/async.hpp"

#include "im_mqtt.h"

#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif

#pragma region namespace
namespace test {
#pragma endregion

#pragma region Dev

#pragma region global val

im::pCMqttClientBase gmsg_;
std::map<std::string, std::string> g_args_;
std::string g_topic_ = "t1";
bool g_is_init_ = false, g_is_sub_ = false;

#pragma endregion

#pragma region Callback

void MqttLog(const base::log::SBaseLog &l) {
  base::debug::OutputLogInfo(l);
}

void MqttMsg(const std::string &topic, const MsgBuf &data) {
  // 处理消息...
  base::debug::OutPut("[Msg]--%s--size:%d", topic.c_str(), data.size());
}

#pragma endregion

#pragma region I/R

// sync connect
bool Test_Init() {
  base::debug::OutPut("Begin...");
  gmsg_ = std::make_shared<im::CMqttClientBase>(MqttLog);

  // sync connect
  std::condition_variable wait_conncet;
  std::mutex wait_conncet_mutex;
  bool wait_conncet_flag = false;

  // init args
  im::SMqttConnectInfo connect_info;
  connect_info.host = "127.0.0.1";
  connect_info.port = 1883;

  connect_info.user_name = "user_test_1";
  connect_info.user_pwd = "123";

  // status callback
  connect_info.cb_status_change =
    [&wait_conncet, &wait_conncet_flag] (im::EMqttOnlineStatus status) {
    base::debug::OutPut(
      "[StatusChange]--%s",
      im::CMqttClientBase::FormatOnlineStatusA(status).c_str());
    if (status == im::EMqttOnlineStatus::connected) {
      wait_conncet_flag = true;
      wait_conncet.notify_all();
    }
  };

  // connect
  if (gmsg_->Connect(connect_info) == false) {
    base::debug::OutPut(L"Connect failed, des:%S",
                        gmsg_->GetLastErr_Astd().c_str());
    return false;
  }

  // wait connect finished
  std::unique_lock<std::mutex> lock(wait_conncet_mutex);
  wait_conncet.wait(lock,
                    [&wait_conncet_flag] () {return wait_conncet_flag; });

  base::debug::OutPut("Connect finished");
  g_is_init_ = true;
  return true;
}

void Test_Release() {
  g_is_init_ = false;
  gmsg_->Disconnect();
  gmsg_ = nullptr;
  base::debug::OutPut("Disconnect finished");
}

bool Test_Sub() {
  auto arg_topic = g_args_["t"];
  if (!arg_topic.empty())
    g_topic_ = arg_topic;

  // sync subscribe
  base::async::Event wait_sub;

  auto func_finished = [&wait_sub] () {
    wait_sub.Notify();
  };
  auto func_msg = [] (const MsgBuf &data) {
    return MqttMsg(g_topic_, data);
  };

  if (gmsg_->Subscribe(g_topic_, func_finished, func_msg) == false) {

    base::debug::OutPut(L"Subscribe failed, des:%S",
                        gmsg_->GetLastErr_Astd().c_str());
    return false;
  }

  wait_sub.Wait();
  base::debug::OutPut(L"Subcribe init success");
  g_is_sub_ = true;
  return true;
}

bool Test_UnSub() {
  g_is_sub_ = false;
  if (gmsg_->Unsubscribe(g_topic_) == false) {
    base::debug::OutPut(L"Unsubscribe failed, des:%S",
                        gmsg_->GetLastErr_Astd().c_str());
    return false;
  }
  base::debug::OutPut(L"Unsubcribe init success");
  return true;
}

#pragma endregion

void Test_SendMsg() {
  base::debug::OutPut("Loop");
  while (true) {
    std::string cmd, data;
    std::cin >> cmd >> data;
    if (cmd == "q")
      break;
    if (gmsg_->Publish(cmd, MsgBuf(data.begin(), data.end()),
                       [] () {
      base::debug::OutPut(L"Publish success");
    })
        == false) {
      base::debug::OutPut(L"Publish failed, des:%S",
                          gmsg_->GetLastErr_Astd().c_str());
    }
  }
}

void RunDev(int argc, char* argv[]) {
  g_args_ = base::ArgsToMap(argc, argv);
  base::debug::OutPut("Begin Test");

  if (Test_Init() == false)
    goto DEV_MQTT_END;

  if (Test_Sub() == false)
    goto DEV_MQTT_END;

  // 测试消息
  Test_SendMsg();

DEV_MQTT_END:
  if (g_is_sub_)
    Test_UnSub();
  if (g_is_init_)
    Test_Release();
  base::debug::WaitEnterGoon("End, wait press enter to exit...");
}

#pragma endregion

#pragma region Cmd

typedef std::function<bool(const std::string&, const std::string&)> FUNC_Cmd;
std::map < std::string, FUNC_Cmd>  g_cmd_func;

void PrintDes() {
  printf("+----------------------------------------------------------+\n");
  printf("+    c----connect <host> <port>    defualt 127.0.0.1 1883  +\n");
  printf("+    d----disconnect                                       +\n");
  printf("+    s----subscribe <topic>    defualt t1                  +\n");
  printf("+    u----unsubscribe                                      +\n");
  printf("+    m----message <topic> <date>    defualt t2 hello       +\n");
  printf("+    t----test <loop time>    100                          +\n");
  printf("+    q----quit                                             +\n");
  printf("+----------------------------------------------------------+\n");
}

bool CmdConnect(const std::string &host, const std::string &port) {
  base::debug::OutPut("CmdConnect");
  return true;
}

bool CmdDisonnect(const std::string&, const std::string&) {
  base::debug::OutPut("CmdDisonnect");
  return true;
}

bool CmdSubscribe(const std::string &topic, const std::string&) {
  base::debug::OutPut("CmdSubscribe");
  return true;
}

bool CmdUnsubscribe(const std::string&, const std::string&) {
  base::debug::OutPut("CmdUnsubscribe");
  return true;
}

bool CmdMessage(const std::string &topic, const std::string &msg) {
  base::debug::OutPut("CmdMessage");
  return true;
}

bool CmdLoopSendMsg(const std::string &loop_time, const std::string&) {
  base::debug::OutPut("CmdLoopSendMsg");
  return true;
}

bool CmdQuit(const std::string&, const std::string&) {
  base::debug::OutPut("CmdQuit");
  return false;
}

void RegCmdFunc() {
  g_cmd_func.insert(
    std::pair<std::string, FUNC_Cmd>("c", CmdConnect));
  g_cmd_func.insert(
    std::pair<std::string, FUNC_Cmd>("d", CmdDisonnect));
  g_cmd_func.insert(
    std::pair<std::string, FUNC_Cmd>("s", CmdSubscribe));
  g_cmd_func.insert(
    std::pair<std::string, FUNC_Cmd>("u", CmdUnsubscribe));
  g_cmd_func.insert(
    std::pair<std::string, FUNC_Cmd>("m", CmdMessage));
  g_cmd_func.insert(
    std::pair<std::string, FUNC_Cmd>("t", CmdLoopSendMsg));
  g_cmd_func.insert(
    std::pair<std::string, FUNC_Cmd>("q", CmdQuit));
}

// 进入控制界面
void RunCmd() {
  RegCmdFunc();

  std::string cmd_buf;
  cmd_buf.resize(1024);
  std::string cmd, arg1, arg2;

  while (true) {
    PrintDes();
    std::cin.getline((char*)cmd_buf.c_str(), cmd_buf.size());
    auto v = base::SpliteStr<std::string>(cmd_buf, " ");
    if (v.size() >= 1)
      cmd = v[0];
    if (v.size() >= 2)
      arg1 = v[1];
    if (v.size() >= 3)
      arg2 = v[2];
    auto it = g_cmd_func.find(cmd);
    if (it != g_cmd_func.end() && it->second) {
      auto br = it->second(arg1, arg2);
      if (br == false)
        break;
    }
  }
}

#pragma endregion

#pragma region namespace
}
#pragma endregion

int main(int argc, char* argv[]) {
  if (argc == 1) {
    test::RunCmd();
  }
  else if (std::string(argv[1]) == "dev") {
    test::RunDev(argc, argv);
  }

  base::debug::WaitEnterGoon("End press enter exit...");
  return 0;
}
