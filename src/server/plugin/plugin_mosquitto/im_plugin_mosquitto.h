#pragma once
#include "lib/base/type_def.h"


#ifdef __cplusplus
extern "C" {
#endif

  IM_LIB_EXP int mosquitto_auth_plugin_version(void);

  IM_LIB_EXP int mosquitto_auth_plugin_init(void **user_data,
                                            struct mosquitto_opt *opts,
                                            int opt_count);

  IM_LIB_EXP int mosquitto_auth_plugin_cleanup(void *user_data,
                                               struct mosquitto_opt *opts,
                                               int opt_count);

  IM_LIB_EXP int mosquitto_auth_security_init(void *user_data,
                                              struct mosquitto_opt *opts,
                                              int opt_count, bool reload);

  IM_LIB_EXP int mosquitto_auth_security_cleanup(void *user_data,
                                                 struct mosquitto_opt *opts,
                                                 int opt_count, bool reload);

  IM_LIB_EXP int mosquitto_auth_acl_check(void *user_data,
                                          int access,
                                          const struct mosquitto *client,
                                          const struct mosquitto_acl_msg *msg);

  IM_LIB_EXP int mosquitto_auth_unpwd_check(void *user_data,
                                            const struct mosquitto *client,
                                            const char *username,
                                            const char *password);

  IM_LIB_EXP int mosquitto_auth_psk_key_get(void *user_data,
                                            const struct mosquitto *client,
                                            const char *hint,
                                            const char *identity,
                                            char *key, int max_key_len);

#ifdef __cplusplus
}
#endif
