#pragma once
#include <time.h>
#include <ctime>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#else
#endif


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

  BaseTime(int64_t v) {
    TIME_ZONE_INFORMATION tmp;
    GetTimeZoneInformation(&tmp);
    auto mTime = std::chrono::microseconds(v +
      (int64_t)(tmp.Bias * tmp.DaylightBias * 1000));
    val_ = std::chrono::time_point<std::chrono::system_clock,
      std::chrono::microseconds>(mTime);
  }

  BaseTime(local_time::time_point t) {
    val_ = t;
  }

  void Now() {
    val_ = local_time::now();
  }

  int64_t GetVal() {
    auto d =
      std::chrono::duration_cast<std::chrono::microseconds>(
        val_ - local_time::time_point());
    return d.count();
  }

  // 计时，返回ms
  int64_t End() {
    auto end = local_time::now();
    auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - val_);
    return duration.count();
  }

  // 格式化时间 "%Y_%m_%d-%H_%M_%S"
  template <typename cus_char>
  cus_string PrintTime(const cus_char *fmt) {
    auto n = local_time::now();
    std::time_t t = local_time::to_time_t(n);
    cus_stringstream ss;

#ifdef WIN32
    tm tm_l = {0};
    auto err_n = localtime_s(&tm_l, &t);
    if (err_n != 0)
      return cus_string();
    ss << std::put_time(&tm_l, fmt);
#else
    ss << std::put_time(localtime(&t), fmt);
#endif // WIN32
    std::chrono::milliseconds ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(
        n.time_since_epoch());

    ss << std::setw(3) << std::setfill<cus_char>('0') << (ms.count() % 1000);
    return ss.str();
  };

  static std::string PrintTimeA() {
    BaseTime b;
    return b.PrintTime("%Y_%m_%d-%H_%M_%S");
  }

  static std::wstring PrintTimeW() {
    BaseTime b;
    return b.PrintTime(L"%Y_%m_%d-%H_%M_%S");
  }

private:
  local_time::time_point val_;
};

#pragma region
}
}
#pragma endregion

