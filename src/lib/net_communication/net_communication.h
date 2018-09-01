#pragma once
#include "base/task.hpp"
#include "base/log.hpp"
#include "base/error.hpp"
#include <functional>
#include <vector>
#include <memory>
#include <boost/asio.hpp>

#pragma region namespace
namespace im {
namespace nc {
#pragma endregion

typedef std::function<void()> FUNC_MessageCallback;
typedef std::function<void(const base::log::SBaseLog &func)> FUNC_LogCallback;

// 初始化参数
struct SNetCom_InitArgs {
  std::string host;
  int port = -1;   // 如果为 -1 则自动查找到一个默认端口使用
  bool listener = false;

  FUNC_MessageCallback cb_message;
  FUNC_LogCallback cb_log;
};

enum class ENetCom_ReqStatus {
  none,
};

class CNetCom:
  private base::task::Task,
  private base::log::Log,
  public base::error::LastError {
public:
  CNetCom();
  virtual ~CNetCom();

  // 描述：初始化，异步
  //  cb  初始化完成后的回调
  //        参数1   打开的端口号
  bool Init(const SNetCom_InitArgs &args,
            const std::function<void(int)> &cb);

  // 描述：释放资源，异步
  //  cb    完成后的通知
  bool Release(const std::function<void()> &cb);

  // 描述：发送消息
  typedef std::function<void(SNetCom_InitArgs)> FUNC_SendCallback;
  bool Send(const std::vector<char> &data,
            const FUNC_SendCallback &cb);

  // 描述：请求数据
  typedef std::function<void(ENetCom_ReqStatus, std::vector<char>)>
    FUNC_RequestCallback;
  bool Request(const std::vector<char> &data,
               const FUNC_RequestCallback &cb);

private:
  void LogCallabck(const base::log::SBaseLog &func);

private:
  FUNC_LogCallback cb_log_ = nullptr;

};
typedef std::shared_ptr<CNetCom> pCNetCom;

#pragma region namespace
}
}
#pragma endregion
