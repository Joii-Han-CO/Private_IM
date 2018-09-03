#include "stdafx.h"
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

bool Init_1() {
  im::nc::SNetCom_InitArgs args;
  args.host = "127.0.0.1";
  args.port = -1;
  args.listener = true;

  args.cb_message = [] () {};
  args.cb_responose = [] () {};

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

  args.cb_message = [] () {};
  args.cb_responose = [] () {};

  if (g_nc2->Init(args, &g_port2) == false)
    return false;
  base::debug::OutPut("Connect port:%d", g_port2);
  return true;
}

// 生成测试数据
std::string GenerDataList() {
  return std::string();
}

void TestSendMsg(im::nc::pCNetCom nc) {
  for (int i = 0; i < 100; i++) {

  }
}

void TestNC(int argc, char *argv[]) {
  g_nc1 = std::make_shared<im::nc::CNetCom>();
  g_nc2 = std::make_shared<im::nc::CNetCom>();

  // 直接在同一个进程测试
  if (Init_1() == false) {
    base::debug::OutPut("Init listener failed");
    return;
  }
  if (Init_2() == false) {
    base::debug::OutPut("Init connector failed");
    return;
  }

  // 打开监听
  getchar();

};

#pragma region namespace
}
#pragma endregion

