#pragma once
#include <string>


#pragma region
namespace im {
namespace gv {
#pragma endregion

// 客户端配置路径
const std::wstring g_cli_cfg_path = L"global.config";

// 服务端配置路径
const std::wstring g_ser_cfg_path = L"server_global.config";

const std::string g_mqtt_pub_sub_ = "global/login";

#pragma region
}
}
#pragma endregion

