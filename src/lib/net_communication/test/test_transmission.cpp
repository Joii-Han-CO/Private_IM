﻿#include "stdafx.h"
#include "test_net_communication.h"
#include "net_communication.h"
#include <base/debug.hpp>
#include <base/character_conversion.hpp>

#pragma region namespace
namespace test {
#pragma endregion

const int test_data_loop_ = 100;

im::nc::pCNetCom g_nc1, g_nc2;
std::vector<std::string> data;

int g_port1 = 0;
int g_port2 = 0;

#pragma region FuncDefine
void CBListener();
void CBConnected();
void CBMessage();
void CBResponse();
void CBLog_L(const base::log::SBaseLog &l) {
  std::string str_type = base::log::Log::FormatTypeA(l.type);
  std::string str_func;
  if (l.func) str_func = l.func;
  std::string str_log;
  if (l.log) str_log = base::Utf16ToGB2312(l.log);

  base::debug::OutPut("[Log]-L-%s--%s:%d---%s",
                      str_type.c_str(), str_func.c_str(),
                      l.line, str_log.c_str());
}
void CBLog_C(const base::log::SBaseLog &l) {
  std::string str_type = base::log::Log::FormatTypeA(l.type);
  std::string str_func;
  if (l.func) str_func = l.func;
  std::string str_log;
  if (l.log) str_log = base::Utf16ToGB2312(l.log);

  base::debug::OutPut("[Log]-C-%s--%s:%d---%s",
                      str_type.c_str(), str_func.c_str(),
                      l.line, str_log.c_str());
}
#pragma endregion

bool Init_1() {
  im::nc::SNetCom_InitArgs args;
  args.host = "127.0.0.1";
  args.port = -1;
  args.listener = true;

  args.cb_listener = CBListener;
  args.cb_message = CBMessage;
  args.cb_responose = CBResponse;
  args.cb_log = CBLog_L;

  if (g_nc1->Init(args, &g_port1) == false)
    return false;
  base::debug::OutPut("Listener port:%d", g_port1);
  return true;
}

bool Init_2() {
  im::nc::SNetCom_InitArgs args;
  args.host = "127.0.0.1";
  args.port = g_port1;
  args.listener = false;

  args.cb_connected = CBConnected;
  args.cb_message = CBMessage;
  args.cb_responose = CBResponse;
  args.cb_log = CBLog_C;

  if (g_nc2->Init(args, &g_port2) == false)
    return false;
  base::debug::OutPut("Connect port:%d", g_port2);
  return true;
}

void CBListener() {
  base::debug::OutPut("L--");
}

void CBConnected() {
  base::debug::OutPut("C--");
}

void CBMessage() {}

void CBResponse() {}

// 生成测试数据
std::string GenerDataList() {
  return std::string();
}

// 测试消息发送
void TestSendMsg(im::nc::pCNetCom nc) {
  for (int i = 0; i < 100; i++) {

  }
}

void TestNC(int argc, char *argv[]) {
  g_nc1 = std::make_shared<im::nc::CNetCom>();
  g_nc2 = std::make_shared<im::nc::CNetCom>();

  base::debug::OutPut("Begin...");
  // 直接在同一个进程测试
  if (Init_1() == false) {
    base::debug::OutPut("Init listener failed");
    return;
  }
  if (Init_2() == false) {
    base::debug::OutPut("Init connector failed");
    return;
  }
  base::debug::OutPut("Init finished.");


  base::debug::OutPut("End, press enter exit...");
  getchar();
  g_nc1->Release(nullptr);
  g_nc1 = nullptr;
  g_nc2->Release(nullptr);
  g_nc2 = nullptr;
};

#pragma region namespace
}
#pragma endregion

