//
//  LogTestServer.cpp
//  AsioTest
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "LogTestServer.hpp"
#include <ciso646>

LogTestServer::LogTestServer(short port)
    : service(),
      acceptor(service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
  socketError.clear();
  connections = 0;
  receivedBytes = 0;
  WaitForNewConnection();
  acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  asioThread = std::thread(&LogTestServer::ThreadFunction, this);
}

void LogTestServer::WaitForNewConnection() {
  sock_ptr cSock(new asio::ip::tcp::socket(service));
  acceptor.async_accept(*cSock.get(),
                        std::bind(&LogTestServer::OnConnectionAccept, this,
                                  std::placeholders::_1, cSock));
}

LogTestServer::~LogTestServer() {
  CloseAllConnections();
  acceptor.close();
  asioThread.join();
}

void LogTestServer::CloseFunction() {
  for (auto sock : existingSockets) {
    if (sock->is_open()) {
      try {
        sock->shutdown(asio::socket_base::shutdown_both);
      } catch (std::exception &e) {
      }
    }
    sock->close();
  }
  existingSockets.clear();
}

void LogTestServer::CloseAllConnections() {
  service.post(std::bind(&LogTestServer::CloseFunction, this));
}

std::string LogTestServer::GetLatestMessage() {
  std::string tempStr = previousMessage;
  previousMessage = "";
  return tempStr;
}

void LogTestServer::ThreadFunction() { service.run(); }

void LogTestServer::OnConnectionAccept(const std::error_code &ec,
                                       sock_ptr cSock) {
  socketError = ec;
  if (asio::error::basic_errors::operation_aborted == ec or
      asio::error::basic_errors::bad_descriptor == ec) {
    return;
  } else if (ec) {
    return;
  }
  existingSockets.push_back(cSock);
  connections++;
  cSock->async_read_some(asio::buffer(receiveBuffer, bufferSize),
                         std::bind(&LogTestServer::HandleRead, this,
                                   std::placeholders::_1, std::placeholders::_2,
                                   cSock));
}

void LogTestServer::HandleRead(std::error_code ec, std::size_t bytesReceived,
                               sock_ptr cSock) {
  socketError = ec;
  if (asio::error::operation_aborted == ec) {
    RemoveSocket(cSock);
    connections--;
    WaitForNewConnection();
    return;
  } else if (ec) {
    RemoveSocket(cSock);
    connections--;
    WaitForNewConnection();
    return;
  }
  receivedBytes += bytesReceived;
  for (int j = 0; j < bytesReceived; j++) {
    if ('\0' == receiveBuffer[j]) {
      previousMessage = currentMessage;
      currentMessage = "";
    } else {
      currentMessage += receiveBuffer[j];
    }
  }
  cSock->async_read_some(asio::buffer(receiveBuffer, bufferSize),
                         std::bind(&LogTestServer::HandleRead, this,
                                   std::placeholders::_1, std::placeholders::_2,
                                   cSock));
}

void LogTestServer::RemoveSocket(sock_ptr cSock) {
  for (int i = 0; i < existingSockets.size(); i++) {
    if (existingSockets[i] == cSock) {
      existingSockets.erase(existingSockets.begin() + i);
      return;
    }
  }
}

std::error_code LogTestServer::GetLastSocketError() {
  auto tempError = socketError;
  socketError.clear();
  return tempError;
}

int LogTestServer::GetNrOfConnections() { return connections; }

int LogTestServer::GetReceivedBytes() { return receivedBytes; }

void LogTestServer::ClearReceivedBytes() { receivedBytes = 0; }
