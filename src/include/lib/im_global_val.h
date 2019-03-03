#pragma once
#include <string>

namespace im {
namespace gv {


// 客户端配置路径
const std::wstring g_cli_cfg_path = L"global.config";
/*
[mqtt]
host=127.0.0.1
port=1883
*/

// 服务端配置路径
const std::wstring g_ser_cfg_path = L"server_global.config";
/*
[mqtt_server]
host=127.0.0.1
port=1883
user_name=server1
admin_pwd=123456

sql_host=tcp://127.0.0.1:3306
sql_name=root
sql_pwd=x
sql_db=pim_db

*/


// server client id
const std::string g_ser_mqtt_id = "A0CE5B2B-74BD-4339-8EEA-BB7FC0685892";

// Mqtt公共通道
const std::string g_mqtt_pub_sub_ = "global/login";

// 登陆通道前缀
const std::string g_mqtt_login_sub_ = "login/";

}
}

