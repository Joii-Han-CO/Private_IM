#pragma once
#include <mutex>

#pragma region namespace
namespace base {
#pragma endregion

// ͬ��ֵ���ڻ�ȡ�����õ�ʱ������
template <typename T>
class SyncVal {
public:
  SyncVal(T t):t_(t) {};
  SyncVal() {};

  SyncVal& operator=(const SyncVal &t1) {
    this->t_ = t1.t_;
    return *this;
  }

  SyncVal& operator=(const T &t1) {
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

#pragma region namespace
}
#pragma endregion
