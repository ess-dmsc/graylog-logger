/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Header file of the networking code.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/ConnectionStatus.hpp"
#include "graylog_logger/GraylogInterface.hpp"
#include <array>
#include <asio.hpp>
#include <atomic>
#include <concurrentqueue/blockingconcurrentqueue.h>
#include <functional>
#include <memory>
#include <string>
#include <thread>

namespace Log {

struct QueryResult;

/// \todo Implement timeouts in the ASIO code in case we ever have problems with
/// bad connections.

class GraylogConnection::Impl {
public:
  using Status = Log::Status;
  Impl(std::string Host, int Port, size_t MaxQueueLength);
  virtual ~Impl();
  virtual void sendMessage(std::string Msg) {
    auto MsgFunc = [=]() { return Msg; };
    LogMessages.try_enqueue(MsgFunc);
  };
  Status getConnectionStatus() const;
  virtual bool flush(std::chrono::system_clock::duration TimeOut);
  virtual size_t queueSize() { return LogMessages.size_approx(); }

protected:
  enum class ReconnectDelay { LONG, SHORT };

  void threadFunction();
  void setState(Status NewState);

  std::atomic<Status> ConnectionState{Status::ADDR_LOOKUP};

  std::atomic_bool closeThread{false};

  std::vector<char> MessageBuffer;

  std::string HostAddress;
  std::string HostPort;

  std::thread AsioThread;
  moodycamel::BlockingConcurrentQueue<std::function<std::string(void)>>
      LogMessages;

private:
  const size_t MessageAdditionLimit{3000};
  void resolverHandler(const asio::error_code &Error,
                       asio::ip::tcp::resolver::iterator EndpointIter);
  void connectHandler(const asio::error_code &Error,
                      const QueryResult &AllEndpoints);
  void sentMessageHandler(const asio::error_code &Error, std::size_t BytesSent);
  void receiveHandler(const asio::error_code &Error, std::size_t BytesReceived);
  void trySendMessage();
  void waitForMessage();
  void doAddressQuery();
  void reConnect(ReconnectDelay Delay);
  void tryConnect(QueryResult AllEndpoints);

  typedef std::unique_ptr<asio::io_service::work> WorkPtr;

  std::array<std::uint8_t, 64> InputBuffer{};
  asio::io_service Service;
  WorkPtr Work;
  asio::ip::tcp::socket Socket;
  asio::ip::tcp::resolver Resolver;
  asio::system_timer ReconnectTimeout;
};

} // namespace Log
