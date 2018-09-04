//
//  GraylogConnection.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-29.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include "graylog_logger/ConcurrentQueue.hpp"
#include <atomic>
#include <memory>
#include <asio.hpp>
#include <string>
#include <thread>
#include <array>

struct QueryResult;

class GraylogConnection {
public:
  GraylogConnection(const std::string &Host, int Port);
  virtual ~GraylogConnection();
  virtual void SendMessage(std::string msg) {LogMessages.push(msg);};
  enum class Status {
    NONE,
    ADDR_LOOKUP,
    ADDR_RETRY_WAIT,
    CONNECT,
    CONNECT_WAIT,
    CONNECT_RETRY_WAIT,
    SEND_LOOP,
    NEW_MESSAGE
  };
  Status GetConnectionStatus();

protected:
  enum class ReconnectDelay {
    LONG,
    SHORT
  };
  
  bool IsMessagePolling{false};
  
  void ThreadFunction();
  void SetState(Status newState);

  const time_t retryDelay = 10; // In seconds
  time_t endWait;
  int connectionTries{0};

  Status ConnectionState{Status::ADDR_LOOKUP};

  std::atomic_bool closeThread{false};

  std::string CurrentMessage;
  ssize_t TotalBytesSent{0};
  bool firstMessage{true};

  std::string HostAddress;
  std::string HostPort;

  std::thread AsioThread;
  ConcurrentQueue<std::string> LogMessages;
private:
  void resolverHandler(const asio::error_code &Error, asio::ip::tcp::resolver::iterator EndpointIter);
  void connectHandler(const asio::error_code &Error, QueryResult AllEndpoints);
  void sentMessageHandler(const asio::error_code &Error, std::size_t BytesSent);
  void receiveHandler(const asio::error_code &Error, std::size_t BytesReceived);
  void trySendMessage();
  void waitForMessage();
  void doAddressQuery();
  void reConnect(ReconnectDelay Delay);
  
  typedef std::unique_ptr<asio::io_service::work> WorkPtr;
  
  std::array<std::uint8_t, 64> InputBuffer;
  asio::io_service Service;
  WorkPtr Work;
  asio::ip::tcp::socket Socket;
  asio::ip::tcp::resolver Resolver;
  asio::system_timer ReconnectTimeout;
};
