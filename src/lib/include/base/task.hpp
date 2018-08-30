#pragma once
#include <list>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

#pragma region namespace
namespace base {
namespace task {
#pragma endregion

class Task {
public:
  Task() {};
  ~Task() {};

protected:
  void StartTask() {
    if (!thread_)
      thread_.reset(new std::thread(std::bind(&Task::TaskRun, this)));
  };
  void StopTask() {
    break_ = true;
    con_v_.notify_all();
  };

  void AddTask(std::function<void(void)>&& task) {
    std::lock_guard<std::mutex> lock(lock_);
    task_list_.emplace_back(std::move(task));
    con_v_.notify_one();
  };

private:

  void TaskRun() {
    while (!break_) {
      std::function<void(void)> func;
      {
        std::unique_lock<std::mutex> lock(lock_);
        while (task_list_.empty()) {
          if (break_)
            return;
          con_v_.wait(lock);
        }
        func = task_list_.front();
        task_list_.pop_front();
      }
      func();
    }
  }

  bool break_ = false;
  std::unique_ptr<std::thread> thread_;

  //消息队列
  std::list<std::function<void(void)>> task_list_;
  std::mutex lock_;
  std::condition_variable con_v_;
};

#pragma region namespace
}
}
#pragma endregion
