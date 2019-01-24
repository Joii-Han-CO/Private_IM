﻿#pragma once
#include <string>


#pragma region
namespace im {
namespace gv {
#pragma endregion

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
*/

const std::string g_ser_mqtt_id = "A0CE5B2B-74BD-4339-8EEA-BB7FC0685892";

// 服务端公共通道
const std::string g_mqtt_pub_sub_ = "global/login";

const std::string g_mqtt_login_sub_ = "login/";

#pragma region
}
}
#pragma endregion

