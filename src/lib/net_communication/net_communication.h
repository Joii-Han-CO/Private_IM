#pragma once
#include "base/task.hpp"
#include "base/log.hpp"
#include "base/error.hpp"
#include <functional>
#include <vector>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#pragma region namespace
namespace im {
namespace nc {
#pragma endregion

const int g_port_max_ = 65535;
const int g_port_tmp_ = 49152;
const int g_net_buf_max_ = 1024 * 8;

typedef boost::asio::ip::tcp boost_tcp;
typedef boost::asio::ip::udp boost_udp;
typedef boost::asio::io_service boost_io_service;

typedef std::function<void()> FUNC_ListenerCallback;
typedef std::function<void()> FUNC_ConnectedCallback;

typedef std::function<void()> FUNC_MessageCallback;
typedef std::function<void()> FUNC_ResponseCallback;
typedef std::function<void(const base::log::SBaseLog &func)> FUNC_LogCallback;

// 初始化参数
struct SNetCom_InitArgs {
  std::string host;
  int port = -1;   // 如果为 -1 则自动查找到一个默认端口使用
  bool listener = false;

  FUNC_ListenerCallback cb_listener;
  FUNC_ConnectedCallback cb_connected;
  FUNC_MessageCallback cb_message;
  FUNC_ResponseCallback cb_responose;
  FUNC_LogCallback cb_log;
};

enum class ENetCom_ReqStatus {
  none,
};

class CNetCom:
  public base::log::Log,
  public base::error::LastError {
public:
  CNetCom();
  virtual ~CNetCom();

  // 描述：初始化，异步
  //  cb  初始化完成后的回调
  //        参数1   打开的端口号
  bool Init(const SNetCom_InitArgs &args, int *port);

 // 描述：释放资源，异步
 //  cb    完成后的通知
  void Release(const std::function<void()> &cb);

  // 描述：发送消息
  typedef std::function<void(SNetCom_InitArgs)> FUNC_SendCallback;
  bool Send(const std::vector<char> &data,
            const FUNC_SendCallback &cb);

  // 描述：请求数据
  typedef std::function<void(ENetCom_ReqStatus, std::vector<char>)>
    FUNC_RequestCallback;
  bool Request(const std::vector<char> &data,
               const FUNC_RequestCallback &cb);

  // 日志
private:
  void LogCallabck(const base::log::SBaseLog &func);

  bool InitCallback(const SNetCom_InitArgs &args);

  // 网络相关
  bool InitNet(const SNetCom_InitArgs &args);

  // 初始化监听者
  bool InitListener();

  // 尝试绑定某个端口
  //  begin  输出起始端口，输出绑定成功的端口。
  std::shared_ptr<boost_tcp::acceptor> BindPort_Sync(int begin);

  bool InitConnector(const std::string &host, int port);

  void HandleResolve(const boost::system::error_code &error,
                     boost_tcp::resolver::iterator ep);

  void InitNetBuf();
  // 释放网络资源
  void ReleaseNet();

  // 网络连接相关

  void HandleAccept(const boost::system::error_code& error);

  void HandleConnect(const boost::system::error_code &error);

  // 某个用户连接上...
  void NetListener();
  void NetConnected();

  void NetStart();

private:
  FUNC_ListenerCallback cb_listener_ = nullptr;
  FUNC_ConnectedCallback cb_connected_ = nullptr;
  FUNC_MessageCallback cb_message_ = nullptr;
  FUNC_ResponseCallback cb_responose_ = nullptr;
  FUNC_LogCallback cb_log_ = nullptr;


  // 网络连接
  std::shared_ptr<boost_io_service> io_service_;
  std::shared_ptr<boost_tcp::acceptor> acceptor_;
  std::shared_ptr<boost_tcp::socket> socket_;

  std::thread thread_server_;
  bool thread_stop_flag_ = false;

  std::vector<char> net_read_buf_;
};
typedef std::shared_ptr<CNetCom> pCNetCom;

#pragma region namespace
}
}
#pragma endregion
