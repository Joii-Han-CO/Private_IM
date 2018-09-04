#include "stdafx.h"
#include "net_communication.h"

#pragma region namespace
namespace im {
namespace nc {
#pragma endregion

CNetCom::CNetCom():
  base::log::Log(std::bind(&CNetCom::LogCallabck,
                           this, std::placeholders::_1)) {}

CNetCom::~CNetCom() {}

#pragma region Init

// 初始化
bool CNetCom::Init(const SNetCom_InitArgs &args, int *port) {
  if (InitCallback(args) == false)
    return false;

  if (InitNet(args) == false)
    return false;
  thread_server_ = std::thread([this] () {
    while (!thread_stop_flag_) {
      io_service_->run_one();
    }
    ReleaseNet();
  });

  if (port)
    if (acceptor_)
      *port = acceptor_->local_endpoint().port();
    else if (socket_)
      *port = socket_->local_endpoint().port();
  return true;
}

bool CNetCom::InitCallback(const SNetCom_InitArgs &args) {
  if (args.listener == true) {
    if (!args.cb_listener) {
      SetLastErrAndLog("args.cb_listener is nullptr in listener");
      return false;
    }
    cb_listener_ = args.cb_listener;
  }
  else {
    if (!args.cb_connected) {
      SetLastErrAndLog("args.cb_connected is nullptr in listener");
      return false;
    }
    cb_connected_ = args.cb_connected;
  }

  cb_connected_ = args.cb_connected;
  if (args.cb_message == nullptr) {
    SetLastErrAndLog("args.cb_message is nullptr");
    return false;
  }
  if (args.cb_responose == nullptr) {
    SetLastErrAndLog("args.cb_responose is nullptr");
    return false;
  }
  cb_message_ = args.cb_message;
  cb_responose_ = args.cb_responose;
  cb_log_ = args.cb_log;
  return true;
}

bool CNetCom::InitNet(const SNetCom_InitArgs &args) {
  io_service_ = std::make_shared<boost_io_service>();
  socket_ = std::make_shared<boost_tcp::socket>(*io_service_);

  if (args.listener == true) {
    // 打开端口进行监听
    if (InitListener() == false)
      return false;
  }
  else {
    // 打开对方端口
    if (InitConnector(args.host, args.port) == false)
      return false;
  }

  return true;
}

// 初始化监听者
bool CNetCom::InitListener() {
  acceptor_ = BindPort_Sync(g_port_tmp_);
  try {
    acceptor_->async_accept(
      *socket_,
      boost::bind(&CNetCom::HandleAccept,
                  this,
                  boost::asio::placeholders::error));
  }
  catch (std::exception& e) {
    SetLastErrAndLog("[net]--run async_accept failed,des:%s", e.what());
    return false;
  }

  return true;
}

// 循环绑定端口
std::shared_ptr<boost_tcp::acceptor> CNetCom::BindPort_Sync(int begin) {
  for (int port = begin; port <= g_port_max_; port++) {
    try {
      auto ep = std::make_shared<boost_tcp::endpoint>(
        boost::asio::ip::tcp::v4(), port);
      auto a = std::make_shared<boost_tcp::acceptor>(
        *io_service_, *ep);
      return a;
    }
    catch (boost::system::system_error &e) {
      auto code = e.code().value();
      if (code != 10013) {
        SetLastErrAndLog("[net]--bind port failed, code:%d", code);
        nullptr;
      }
    }
  }
  SetLastErrAndLog("[net]--bind port failed, port exhausted");
  return nullptr;
}

// 初始化连接
bool CNetCom::InitConnector(const std::string &host, int port) {
  boost_tcp::resolver resolver(*io_service_);
  // 这里不会使用域名，因此直接使用同步的方式
  resolver.async_resolve(
    boost_tcp::resolver::query(host, std::to_string(port)),
    boost::bind(&CNetCom::HandleResolve, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::iterator));
  return true;
}

void CNetCom::HandleResolve(const boost::system::error_code &error,
                            boost_tcp::resolver::iterator ep) {
  boost::asio::async_connect(
    *socket_, ep,
    boost::bind(&CNetCom::HandleConnect,
                this,
                boost::asio::placeholders::error));
}

#pragma endregion

#pragma region Release

// 释放
void CNetCom::Release(const std::function<void()> &cb) {
  if (io_service_) {
    thread_stop_flag_ = true;
    io_service_->stop();
  }
  if (thread_server_.joinable())
    thread_server_.join();
}

void CNetCom::ReleaseNet() {
  if (acceptor_)
    acceptor_->close();
  else if (socket_)
    socket_->close();
}

#pragma endregion

bool CNetCom::Send(const std::vector<char> &data,
                   const FUNC_SendCallback &cb) {
  return true;
}

bool CNetCom::Request(const std::vector<char> &data,
                      const FUNC_RequestCallback &cb) {
  return true;
}

#pragma region Unit

void CNetCom::LogCallabck(const base::log::SBaseLog &func) {
  if (cb_log_)
    cb_log_(func);
}

#pragma endregion

#pragma region Net

void CNetCom::HandleAccept(const boost::system::error_code &error) {
  // 这里是否考虑同步...
  if (error) {
    PrintErro("[net]--accept error,des:%d", error.message().c_str());
    return;
  }
  NetListener();
}

void CNetCom::HandleConnect(const boost::system::error_code &error) {
  // 这里是否考虑同步...
  if (error) {
    PrintErro("[net]--accept error,des:%d", error.message().c_str());
    return;
  }
  NetConnected();
}

// 有新的连接
void CNetCom::NetListener() {
  if (cb_listener_)
    cb_listener_();
}

// 已经连接上
void CNetCom::NetConnected() {
  if (cb_connected_)
    cb_connected_();
}

#pragma endregion

#pragma region namespace
}
}
#pragma endregion
