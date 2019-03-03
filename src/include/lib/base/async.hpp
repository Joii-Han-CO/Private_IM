#pragma once
#include <condition_variable>
#include <mutex>
#include "type_def.h"

namespace base {
namespace async {

class Event {
public:
  void Notify() {
    flag_ = true;
    cv_.notify_one();
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(sync_);
    cv_.wait(lock, [this]() {
      return flag_;
    });
  }

private:
  std::condition_variable cv_;
  std::mutex sync_;
  bool flag_ = false;
};

// 异步初始化
class AsyncInit {
public:
  AsyncInit() {};
  virtual ~AsyncInit() {};

protected:
  Func_AsyncResult init_finished_;
  void InitFinished(bool suc) {
    if (init_finished_)
      init_finished_(suc);
  };
};

}
}
