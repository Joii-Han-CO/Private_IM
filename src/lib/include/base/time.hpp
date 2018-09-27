#pragma once
#include <time.h>
#include <ctime>
#include <chrono>


#pragma region
namespace base {
namespace time {
#pragma endregion

typedef std::chrono::system_clock local_time;

// TODO ����ʱ��
class BaseTime {
public:
  // Ĭ�Ϲ��캯������ʼ��Ϊ��ǰʱ��
  BaseTime() {
    val_ = local_time::now();
  };

  // ���ʱ��
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

