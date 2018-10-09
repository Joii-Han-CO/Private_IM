#include "stdafx.h"
#include "im_plugin_mosquitto.h"
#include "..\include\im_plugin_mosquitto.h"
#include "plugin_mosq.h"

// 定义 mosquitto 的各个接口, 调试直接挂载 mosquitto 进程
#pragma region Init/R

int mosquitto_auth_plugin_version(void) {
  return im::plugin::CPluginMosq::GetVersion();
}

// 初始化...
int mosquitto_auth_plugin_init(void **user_data,
                               struct mosquitto_opt *opts,
                               int opt_count) {

  auto  ptr = new im::plugin::CPluginMosq;
  *user_data = (void*)user_data;
  if (ptr->Init() == false) {
    // error
  }
  return 0;
}

int mosquitto_auth_plugin_cleanup(void *user_data,
                                  struct mosquitto_opt *opts,
                                  int opt_count) {
  auto ptr = (im::plugin::pCPluginMosq)user_data;
  ptr->Release();
  delete ptr;
  return 0;
}

int mosquitto_auth_security_init(void *user_data,
                                 struct mosquitto_opt *opts,
                                 int opt_count,
                                 bool reload) {
  auto ptr = (im::plugin::pCPluginMosq)user_data;
  if (!ptr) {
    // error
  }
  if (ptr->InitSecurity() == false) {
    // error
  }
  return 0;
}

int mosquitto_auth_security_cleanup(void *user_data,
                                    struct mosquitto_opt *opts,
                                    int opt_count,
                                    bool reload) {
  auto ptr = (im::plugin::pCPluginMosq)user_data;
  if (!ptr) {
    // error
  }
  ptr->ReleaseSecurity();
  return 0;
}

#pragma endregion

int mosquitto_auth_acl_check(void *user_data,
                             int access,
                             const struct mosquitto *client,
                             const struct mosquitto_acl_msg *msg) {
  auto ptr = (im::plugin::pCPluginMosq)user_data;
  if (ptr == nullptr) {
    // error
  }
  if (ptr->UserSubscribe(access, client, msg) == false) {
    // 没通过校验

  }
  return 0;
}

int mosquitto_auth_unpwd_check(void *user_data,
                               const struct mosquitto *client,
                               const char *username,
                               const char *password) {
  auto ptr = (im::plugin::pCPluginMosq)user_data;
  if (ptr == nullptr) {
    // error
  }
  if (ptr->UserLogin(client, username, password) == false) {
    // 没通过校验

  }
  return 0;
}

int mosquitto_auth_psk_key_get(void *user_data,
                               const struct mosquitto *client,
                               const char *hint,
                               const char *identity,
                               char *key,
                               int max_key_len) {
  auto ptr = (im::plugin::pCPluginMosq)user_data;

  return 0;
}
