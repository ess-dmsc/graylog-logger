//
//  LogTestServer.hpp
//  BoostAsioTest
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <asio.hpp>
#include <atomic>
#include <string>
#include <thread>
#include <vector>

typedef std::shared_ptr<asio::ip::tcp::socket> sock_ptr;

//------------------------------------------------------------------------------
//     THIS CLASS IS NOT THREAD SAFE AND MAY CRASH AT ANY MOMENT
//------------------------------------------------------------------------------

class LogTestServer {
public:
  LogTestServer(short port);
  ~LogTestServer();
  std::string GetLatestMessage();
  std::error_code GetLastSocketError();
  void CloseAllConnections();
  int GetNrOfConnections();
  int GetReceivedBytes();
  int GetNrOfMessages();
  void ClearReceivedBytes();

private:
  asio::io_service service;
  void ThreadFunction();
  std::thread asioThread;

  asio::ip::tcp::acceptor acceptor;

  void WaitForNewConnection();
  void OnConnectionAccept(const std::error_code &ec, sock_ptr cSock);
  void HandleRead(std::error_code ec, std::size_t bytesReceived,
                  sock_ptr cSock);

  void RemoveSocket(sock_ptr cSock);

  static const int bufferSize = 100;
  char receiveBuffer[bufferSize];
  int nrOfMessagesReceived = 0;

  std::error_code socketError;
  std::atomic_int connections;
  std::atomic_int receivedBytes;

  std::string currentMessage;
  std::string previousMessage;

  std::vector<sock_ptr> existingSockets;

  void CloseFunction();
};
