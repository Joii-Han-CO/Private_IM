#pragma once
#include <memory>
#include "base/log.hpp"
#include "base/error.hpp"

#pragma region
namespace im {
#pragma endregion

class CMqttClientBase:
  public virtual base::LastError,
  public virtual base::Log {
public:
  CMqttClientBase(base::LogCallback func);
  ~CMqttClientBase();
};
typedef std::shared_ptr<CMqttClientBase> pCMqttClientBase;

#pragma region
}
#pragma endregion
