#include "stdafx.h"
#include "mqtt_client_base.h"

#pragma region
namespace im {
#pragma endregion

CMqttClientBase::CMqttClientBase(base::LogCallback log_func):
  Log(log_func) {}

CMqttClientBase::~CMqttClientBase() {}

#pragma region
}
#pragma endregion
