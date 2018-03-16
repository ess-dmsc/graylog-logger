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
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

class GraylogConnection {
public:
  GraylogConnection(const std::string &host, int port);
  virtual ~GraylogConnection();
  virtual void SendMessage(std::string msg);
  enum class ConStatus {
    NONE,
    ADDR_LOOKUP,
    ADDR_RETRY_WAIT,
    CONNECT,
    CONNECT_WAIT,
    CONNECT_RETRY_WAIT,
    SEND_LOOP,
    NEW_MESSAGE
  };
  ConStatus GetConnectionStatus();

protected:
  void EndThread();

  void ThreadFunction();
  void MakeConnectionHints();
  void GetServerAddr();
  void ConnectToServer();
  void ConnectWait();
  void NewMessage();
  void SendMessageLoop();
  void CheckConnectionStatus();
  void SetState(ConStatus newState);

  const time_t retryDelay = 10; // In seconds
  time_t endWait;
  int connectionTries{0};

  ConStatus stateMachine{ConStatus::ADDR_LOOKUP};

  std::atomic_bool closeThread{false};

  std::string currentMessage;
  ssize_t bytesSent{0};
  bool firstMessage{true};

  std::string host;
  std::string port;

  std::thread connectionThread;
  int socketFd{-1};  // Socket id (file descriptor)
  addrinfo hints{0}; // Connection hints
  addrinfo *conAddresses{nullptr};
  struct sockaddr_in serverConInfo {
    0
  }; //
  ConcurrentQueue<std::string> logMessages;
#ifdef MSG_NOSIGNAL
  const int sendOpt = MSG_NOSIGNAL;
#else
  const int sendOpt = 0;
#endif
private:
  std::atomic<ConStatus> retConState;
#ifdef _WIN32
  WSADATA wsaData;
#endif
};
