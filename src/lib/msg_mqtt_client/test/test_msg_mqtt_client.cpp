#include "stdafx.h"
#include "test_msg_mqtt_client.h"
#include "base/character_conversion.hpp"
#include "base/string_operation.hpp"

#include "base/debug.hpp"
#include "mqtt_client_base.h"

#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif

#pragma region namespace
namespace test {
#pragma endregion

#pragma region Dev

std::string g_topic_ = "test_1";

im::pCMqttClientBase gmsg;

#pragma region Callback

void MqttLog(const base::log::SBaseLog &l) {
  base::debug::OutputLogInfo(l);
}

#pragma endregion

#pragma region I/R

// 连接...
bool Test_Init() {
  base::debug::OutPut("Begin...");
  gmsg = std::make_shared<im::CMqttClientBase>(MqttLog);

  // 同步连接
  std::condition_variable wait_conncet;
  std::mutex wait_conncet_mutex;
  bool wait_conncet_flag = false;

  // 初始化连接参数
  im::SMqttConnectInfo connect_info;
  connect_info.host = "127.0.0.1";
  connect_info.port = 1883;

  // 回调
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

  if (gmsg->Connect(connect_info) == false) {
    base::debug::OutPut(L"Connect failed, des:%S",
                        gmsg->GetLastErr_Astd().c_str());
    return false;
  }

  std::unique_lock<std::mutex> lock(wait_conncet_mutex);
  wait_conncet.wait(lock,
                    [&wait_conncet_flag] () {return wait_conncet_flag; });

  base::debug::OutPut("Connect finished");
  return true;
}

void Test_Release() {
  gmsg->Disconnect();
  base::debug::OutPut("Disconnect finished");
}

bool Test_Sub(int argc, char* argv[]) {
  auto args = base::ArgsToMap(argc, argv);
  auto arg_topic = args["t"];
  if (!arg_topic.empty())
    g_topic_ = arg_topic;

  gmsg->Subscribe(g_topic_, [] (std::vector<char>) {});

  return true;
}

bool Test_UnSub() {
  gmsg->Unsubscribe(g_topic_);
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
    if (gmsg->Publish(cmd, std::vector<char>(data.begin(), data.end()),
                      [] () {
      base::debug::OutPut(L"Publish success");
    })
        == false) {
      base::debug::OutPut(L"Publish failed, des:%S",
                          gmsg->GetLastErr_Astd().c_str());
    }
  }
}

void RunDev() {
  if (Test_Init() == false)
    return;

  // 测试消息
  Test_SendMsg();

  // 断开连接
  Test_Release();
}

#pragma endregion

#pragma region Cmd

typedef std::function<bool(const std::string&, const std::string&)> FUNC_Cmd;
std::map < std::string, FUNC_Cmd>  g_cmd_func;

void PrintDes() {
  printf("+----------------------------------------------------------+\n");
  printf("+    c----connect <host> <port>    defualt 127.0.0.1 1883  +\n");
  printf("+    d----disconnect                                       +\n");
  printf("+    s----subscribe <topci>    defualt t1                  +\n");
  printf("+    u----unsubscribe                                      +\n");
  printf("+    m----message <topci> <date>    defualt t2 hello       +\n");
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

bool CmdSubscribe(const std::string &topci, const std::string&) {
  base::debug::OutPut("CmdSubscribe");
  return true;
}

bool CmdUnsubscribe(const std::string&, const std::string&) {
  base::debug::OutPut("CmdUnsubscribe");
  return true;
}

bool CmdMessage(const std::string &topci, const std::string &msg) {
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
    test::RunDev();
  }

  base::debug::WaitEnterGoon("End press enter exit...");
  return 0;
}
