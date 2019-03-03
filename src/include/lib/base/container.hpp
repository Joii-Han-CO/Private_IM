#pragma once
#include <mutex>

namespace base {

// 同步值，在获取和设置的时候都有锁
template <typename T>
class SyncVal {
public:
  SyncVal(T t):t_(t) {};
  SyncVal() {};

  inline SyncVal& operator=(const SyncVal &t1) {
    this->t_ = t1.t_;
    return *this;
  }

  inline SyncVal& operator=(const T &t1) {
    this->t_ = t1;
    return *this;
  }

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

}
