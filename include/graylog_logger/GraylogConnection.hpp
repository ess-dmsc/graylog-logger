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
  GraylogConnection(std::string host, int port);
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
  const time_t retryDelay = 10; // In seconds
  time_t endWait;
  int connectionTries;

  ConStatus stateMachine;

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

  int queueLength;

  std::atomic_bool closeThread;

  std::string currentMessage;
  ssize_t bytesSent;
  bool firstMessage;

  std::string host;
  std::string port;

  std::thread connectionThread;
  int socketFd;   // Socket id (file descriptor)
  addrinfo hints; // Connection hints
  addrinfo *conAddresses;
  struct sockaddr_in serverConInfo; //
  ConcurrentQueue<std::string> logMessages;
#ifdef MSG_NOSIGNAL
  const int sendOpt = MSG_NOSIGNAL;
#else
  const int sendOpt = 0;
#endif
private:
  mutable std::mutex stateMutex;
  ConStatus retConState;
  ConcurrentQueue<ConStatus> stateQueue;
#ifdef _WIN32
  WSADATA wsaData;
#endif
};
