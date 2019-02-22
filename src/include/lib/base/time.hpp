#pragma once
#include "container.hpp"
#include "async.hpp"

#include <time.h>

#include <ctime>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>
#include <iomanip>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <map>
#include <list>

#ifdef _WIN32
#include <windows.h>
#else
#endif

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

  // ��ȡʱ���ֵ������ ΢��
  int64_t GetVal() {
    auto d =
      std::chrono::duration_cast<std::chrono::microseconds>(
        val_ - local_time::time_point());
    return d.count();
  }

  // ��ʱ������ms
  int64_t End() {
    auto end = local_time::now();
    auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - val_);
    return duration.count();
  }

  // ��ʽ��ʱ�� "%Y_%m_%d-%H_%M_%S"
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
StdSharedPtr_Typedef(BaseTime);

class Timer {
  typedef std::map<local_time::time_point, std::list<Func_Result>> TimerMap;
public:
  Timer(): stop_flag_(false) {
    t_ = std::thread(std::bind(&Timer::Run, this));
  };
  ~Timer() {
    Stop();
    WaitExit();
  };

  // val�ȴ�ʱ��
  void AddTimer(uint32_t val, Func_Result func) {
    std::unique_lock<std::mutex> lock(sync_add_);

    auto tp = local_time::now() + std::chrono::milliseconds(val);
    auto it = timer_.find(tp);

    if (it == timer_.end()) {
      std::list<Func_Result> lt;
      lt.push_back(func);
      timer_[tp] = lt;
    }
    else {
      timer_[tp].push_back(func);
    }

    // ����
    add_timer_.Notify();
    cv_.notify_one();
  };

  // ����
  void Stop() {
    stop_flag_ = true;
  };

  // �ȴ��˳�
  void WaitExit() {
    if (t_.joinable())
      t_.join();
  };

private:
  void Run() {
    while (!stop_flag_.Get()) {
      add_timer_.Wait();

      auto now = local_time::now();

      std::list<Func_Result> lt;
      auto t = GetNextTimer(now, lt);
      if (t == local_time::time_point())
        continue;
      else {
        std::unique_lock<std::mutex> lock(cv_lock_);
        if (cv_.wait_until(lock, t) == std::cv_status::timeout) {
          RunTimerFunc(lt);
        }
        else {
          BaseAddTimer(t, lt);
        }
      }
    }
  };

  // �õ�ʱ�䣨���룩����������õ���ʱ��ȵ�ǰʱ��̣����ִ�е�
  int64_t GetMinTimerVal(const local_time::time_point &now,
                         TimerMap::iterator &it) {
    std::unique_lock<std::mutex> lock(sync_add_);

    while (true) {
      it = timer_.begin();
      if (it == timer_.end())
        return -1;

      int64_t ref_val =
        std::chrono::duration_cast<std::chrono::milliseconds>(
          it->first - now).count();
      if (ref_val >= 0)
        return ref_val;
      else {
        auto lt = it->second;
        timer_.erase(it);

        RunTimerFunc(lt);
      }
    }
  };

  local_time::time_point GetNextTimer(const local_time::time_point &now,
                                      std::list<Func_Result> &lt) {
    std::unique_lock<std::mutex> lock(sync_add_);

    while (true) {
      auto it = timer_.begin();
      if (it == timer_.end())
        return local_time::time_point();

      auto tn = it->first;
      lt = it->second;
      timer_.erase(it);

      if (tn <= now)
        RunTimerFunc(lt);
      else
        return tn;
    }
  }

  // ִ���б��е�ȫ������
  void RunTimerFunc(const std::list<Func_Result> &lt) {
    if (lt.empty())
      return;
    for (auto i : lt) {
      if (i != nullptr)
        i();
    }
  }

  void BaseAddTimer(const local_time::time_point &t,
                    const std::list<Func_Result> &lt) {
    auto it = timer_.find(t);
    if (it == timer_.end()) {
      timer_[t] = lt;
    }
    else {
      it->second.insert(it->second.begin(), lt.begin(), lt.end());
    }
  }

  std::thread t_;
  base::SyncVal<bool> stop_flag_ = false;

  std::condition_variable cv_;
  std::mutex cv_lock_;
  base::SyncVal<bool> cv_flag_ = false;

  base::async::Event add_timer_;

  TimerMap timer_;
  std::mutex sync_add_;
};
StdSharedPtr_Typedef(Timer);

#pragma region
}
}
#pragma endregion

