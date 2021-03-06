﻿#include "stdafx.h"
#include "plugin_mosq.h"
#include <map>


#pragma region
namespace im {
namespace plugin {
#pragma endregion

#pragma region Global

int CPluginMosq::GetVersion() {
  return MOSQ_AUTH_PLUGIN_VERSION;
}

#pragma endregion

#pragma region I/R

CPluginMosq::CPluginMosq():
  sql_user_(std::bind(&CPluginMosq::OnLog, this, std::placeholders::_1)) {}

bool CPluginMosq::Init() {
  return false;
}

void CPluginMosq::Release() {}

bool CPluginMosq::InitSecurity() {
  return true;
}

void CPluginMosq::ReleaseSecurity() {

}

bool CPluginMosq::UserLogin(const struct mosquitto *client,
                            const char *username,
                            const char *password) {
  // 1. 检查用户名密码是否正确
  // 2. 不允许重复登陆
  return true;
}

bool CPluginMosq::UserSubscribe(int access,
                                const struct mosquitto *client,
                                const struct mosquitto_acl_msg *msg) {
  return true;
}

void CPluginMosq::OnLog(const base::log::SBaseLog & func) {

}

#pragma endregion

#pragma region
}
}
#pragma endregion
