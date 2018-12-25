// from
// https://www.cnblogs.com/lzpong/p/6397997.html
#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#pragma region
namespace base {
#pragma endregion

class CThreadPool {
public:
  inline CThreadPool(unsigned short size = 4):stoped {false} {
    idlThrNum = size < 1 ? 1 : size;
    for (size = 0; size < idlThrNum; ++size) {   //初始化线程数量
      pool.emplace_back(
        [this] { // 工作线程函数
        while (!this->stoped) {
          std::function<void()> task;
          {   // 获取一个待执行的 task
            std::unique_lock<std::mutex> lock {this->m_lock};
            this->cv_task.wait(lock,
                               [this] {
              return this->stoped.load() || !this->tasks.empty();
            }
            ); // wait 直到有 task
            if (this->stoped && this->tasks.empty())
              return;
            task = std::move(this->tasks.front()); // 取一个 task
            this->tasks.pop();
          }
          idlThrNum--;
          task();
          idlThrNum++;
        }
      }
      );
    }
  }
  inline ~CThreadPool() {
    stoped.store(true);
    cv_task.notify_all(); // 唤醒所有线程执行
    for (std::thread& thread : pool) {
        //thread.detach(); // 让线程“自生自灭”
      if (thread.joinable())
        thread.join(); // 等待任务结束， 前提：线程一定会执行完
    }
  }

public:
  template<class F, class... Args>
  auto Commit(F&& f, Args&&... args)->std::future<decltype(f(args...))> {
    if (stoped.load())
      throw std::runtime_error("commit on ThreadPool is stopped.");

    using RetType = decltype(f(args...));
    auto task = std::make_shared<std::packaged_task<RetType()> >(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...)
      );
    std::future<RetType> future = task->get_future();
    {
      std::lock_guard<std::mutex> lock {m_lock};
      tasks.emplace(
        [task]() { // push(Task{...})
        (*task)();
      }
      );
    }
    cv_task.notify_one(); // 唤醒一个线程执行

    return future;
  }

  int IdlCount() { return idlThrNum; }

private:
  using Task = std::function<void()>;
  std::vector<std::thread> pool;
  std::queue<Task> tasks;
  std::mutex m_lock;
  std::condition_variable cv_task;
  std::atomic<bool> stoped;
  std::atomic<int>  idlThrNum;
};
typedef std::shared_ptr<CThreadPool> pCThreadPool;

#pragma region
}
#pragma endregion

