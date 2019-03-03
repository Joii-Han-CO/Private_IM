#pragma once
#include <list>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace base {
namespace b_async {

class Task {
public:
  Task() {};
  virtual ~Task() {};

protected:
  void StartTask() {
    if (!thread_)
      thread_.reset(new std::thread(std::bind(&Task::TaskRun, this)));
  };
  void StopTask() {
    lock_break_.lock();
    break_ = true;
    lock_break_.unlock();

    con_v_.notify_all();
  };
  void WaitTask() {
    if (thread_ && thread_->joinable())
      thread_->join();
  }

  void AddTask(Func_Result &&task) {
    std::unique_lock<std::mutex> lock_break(lock_break_);
    if (break_)
      return;
    std::lock_guard<std::mutex> lock(lock_);
    task_list_.emplace_back(std::move(task));
    con_v_.notify_one();
  };

private:

  void TaskRun() {
    while (!break_) {
      Func_Result func;
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
  std::list<Func_Result> task_list_;
  std::mutex lock_;
  std::condition_variable con_v_;

  std::mutex lock_break_;
};

class Condition {
public:
  void NotifyOne() {
    f_ = true;
    c_.notify_one();
  };
  void Wait() {
    std::unique_lock<std::mutex> lock(m_);
    c_.wait(lock, [this]() {return f_; });
  };
private:
  std::condition_variable c_;
  std::mutex m_;
  bool f_ = false;
};

}
}
