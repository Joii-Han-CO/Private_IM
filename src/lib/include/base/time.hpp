#pragma once
#include <time.h>
#include <ctime>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>


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

inline std::string PrintTime(const std::string &out_buf) {
  auto n = local_time::now();
  auto ms = n.time_since_epoch();
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ms).count();
  auto const msecs = diff % 1000;
  std::time_t t = local_time::to_time_t(n);
  std::stringstream ss;

#ifdef WIN32
  tm tm_l = {0};
  auto err_n = localtime_s(&tm_l, &t);
  ss << std::put_time(&tm_l, out_buf.c_str());
#else
  ss << std::put_time(localtime(&t), out_buf.c_str());
#endif // WIN32
  return ss.str();
}

#pragma region
}
}
#pragma endregion

