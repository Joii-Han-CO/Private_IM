#pragma once
#include <condition_variable>
#include <mutex>

#pragma region
namespace base {
namespace async {
#pragma endregion

class Event {
public:
  void Notify() {
    flag_ = true;
    cv_.notify_one();
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(sync_);
    cv_.wait(lock, [this] () {return flag_; });
  }

private:
  std::condition_variable cv_;
  std::mutex sync_;
  bool flag_ = false;
};

template <typename T>
class SyncVal {
public:
  SyncVal(T t):t_(t) {};
  SyncVal() {};

  void Set(T t) {
    std::unique_lock<std::mutex> lock(lock_);
    t_ = t;
  };
  T Get() {
    std::unique_lock<std::mutex> lock(lock_);
    return t_;
  }
private:
  T t_;
  std::mutex lock_;
};

#pragma region
}
}
#pragma endregion

