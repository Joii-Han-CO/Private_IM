#pragma once
#include <time.h>
#include <ctime>
#include <chrono>


#pragma region
namespace base {
namespace time {
#pragma endregion

typedef std::chrono::system_clock local_time;

// TODO 定义时间
class BaseTime {
public:
  // 默认构造函数，初始化为当前时间
  BaseTime() {
    val_ = local_time::now();
  };

  // 输出时间
  BaseTime(time_t t, long ms) {
    val_ = local_time::from_time_t(t);
  }

  time_t GetTimeT(long *ms = nullptr) {
    auto ref_t = local_time::to_time_t(val_);
    if (ms) {
      auto l = local_time::from_time_t(ref_t);
      auto de = val_ - l;
      *ms = (long)de.count();
    }
    return local_time::to_time_t(val_);
  }

private:
  local_time::time_point val_;
};

#pragma region
}
}
#pragma endregion

