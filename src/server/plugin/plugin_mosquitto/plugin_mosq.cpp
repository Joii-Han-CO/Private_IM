#include "pch.h"
#include "plugin_mosq.h"
#include <map>


namespace im {
namespace plugin {


int CPluginMosq::GetVersion() {
  return MOSQ_AUTH_PLUGIN_VERSION;
}


CPluginMosq::CPluginMosq() {}

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

}
}
