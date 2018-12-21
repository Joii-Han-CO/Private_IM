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

inline std::string PrintTime() {
  auto n = local_time::now();
  std::time_t t = local_time::to_time_t(n);
  std::stringstream ss;

#ifdef WIN32
  tm tm_l = {0};
  auto err_n = localtime_s(&tm_l, &t);
  if (err_n != 0)
    return "";
  ss << std::put_time(&tm_l, "%Y_%m_%d-%H_%M_%S");
#else
  ss << std::put_time(localtime(&t), out_buf.c_str());
#endif // WIN32

  std::chrono::milliseconds ms =
    std::chrono::duration_cast<std::chrono::milliseconds>(
      n.time_since_epoch());

  ss << "_" << std::setw(3) << std::setfill('0') << (ms.count() % 1000);
  return ss.str();
}

// 微秒 计时...
class Timer_ms {
public:
  Timer_ms() {
    start_ = local_time::now();
  }

  long long End() {
    auto end = local_time::now();
    auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start_);

    return duration.count();
  }

private:
  std::chrono::time_point<local_time> start_;
};

#pragma region
}
}
#pragma endregion

