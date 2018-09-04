//
//  GraylogConnection.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-29.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "graylog_logger/GraylogConnection.hpp"
#include <chrono>
#include <ciso646>
#include <iostream>
#include <utility>

struct QueryResult {
  explicit QueryResult(asio::ip::tcp::resolver::iterator &&Endpoints)
      : EndpointIterator(std::move(Endpoints)) {
    while (EndpointIterator != asio::ip::tcp::resolver::iterator()) {
      auto CEndpoint = *EndpointIterator;
      EndpointList.emplace_back(CEndpoint);
      ++EndpointIterator;
    }
    std::sort(EndpointList.begin(), EndpointList.end(), [](auto &a, auto &b) {
      return a.address().is_v6() < b.address().is_v6();
    });
  }
  asio::ip::tcp::endpoint getNextEndpoint() {
    if (NextEndpoint < EndpointList.size()) {
      return EndpointList[NextEndpoint++];
    }
    return {};
  }
  bool isDone() const { return NextEndpoint >= EndpointList.size(); }
  asio::ip::tcp::resolver::iterator EndpointIterator;
  std::vector<asio::ip::tcp::endpoint> EndpointList;
  int NextEndpoint{0};
};

GraylogConnection::GraylogConnection(std::string Host, int Port)
    : HostAddress(std::move(Host)), HostPort(std::to_string(Port)), Service(),
      Work(new asio::io_service::work(Service)), Socket(Service),
      Resolver(Service), ReconnectTimeout(Service, std::chrono::seconds(10)) {
  doAddressQuery();
  AsioThread = std::thread(&GraylogConnection::ThreadFunction, this);
}

void GraylogConnection::resolverHandler(
    const asio::error_code &Error,
    asio::ip::tcp::resolver::iterator EndpointIter) {
  if (Error) {
    SetState(Status::ADDR_RETRY_WAIT);
    reConnect(ReconnectDelay::LONG);
    return;
  }
  QueryResult AllEndpoints(std::move(EndpointIter));
  auto CurrentEndpoint = AllEndpoints.getNextEndpoint();
  auto HandlerGlue = [this, AllEndpoints](auto &Err) {
    this->connectHandler(Err, AllEndpoints);
  };
  Socket.async_connect(CurrentEndpoint, HandlerGlue);
  SetState(Status::CONNECT);
}

void GraylogConnection::connectHandler(const asio::error_code &Error,
                                       QueryResult AllEndpoints) {
  if (!Error) {
    SetState(Status::SEND_LOOP);
    auto HandlerGlue = [this](auto &Error, auto Size) {
      this->receiveHandler(Error, Size);
    };
    Socket.async_receive(asio::buffer(InputBuffer), HandlerGlue);
    trySendMessage();
    IsMessagePolling = true;
    return;
  }
  Socket.close();
  if (AllEndpoints.isDone()) {
    reConnect(ReconnectDelay::LONG);
    return;
  }
  asio::ip::tcp::endpoint CurrentEndpoint = AllEndpoints.getNextEndpoint();
  auto HandlerGlue = [this, AllEndpoints](auto &Err) {
    this->connectHandler(Err, AllEndpoints);
  };
  Socket.async_connect(CurrentEndpoint, HandlerGlue);
  SetState(Status::CONNECT);
}

void GraylogConnection::reConnect(ReconnectDelay Delay) {
  auto HandlerGlue = [this](auto &Err) { this->doAddressQuery(); };
  switch (Delay) {
  case ReconnectDelay::SHORT:
    ReconnectTimeout.expires_after(std::chrono::milliseconds(50));
    break;
  case ReconnectDelay::LONG:
  default:
    ReconnectTimeout.expires_after(std::chrono::seconds(10));
    break;
  }
  ReconnectTimeout.async_wait(HandlerGlue);
  SetState(Status::ADDR_RETRY_WAIT);
}

void GraylogConnection::receiveHandler(const asio::error_code &Error,
                                       __attribute__((unused))
                                       std::size_t BytesReceived) {
  if (Error) {
    Socket.close();
    reConnect(ReconnectDelay::SHORT);
    return;
  }
}

void GraylogConnection::trySendMessage() {
  if (not Socket.is_open()) {
    IsMessagePolling = false;
    return;
  }
  bool PopResult = LogMessages.try_pop(CurrentMessage);
  if (PopResult) {
    auto HandlerGlue = [this](auto &Err, auto Size) {
      this->sentMessageHandler(Err, Size);
    };
    asio::async_write(
        Socket, asio::buffer(CurrentMessage.c_str(), CurrentMessage.size() + 1),
        HandlerGlue);
  } else {
    Service.post([this]() { this->waitForMessage(); });
  }
}

void GraylogConnection::sentMessageHandler(const asio::error_code &Error,
                                           std::size_t BytesSent) {
  if (Error or BytesSent != CurrentMessage.size() + 1) {
    Socket.close();
    return;
  }
  trySendMessage();
}

void GraylogConnection::waitForMessage() {
  if (not Socket.is_open()) {
    IsMessagePolling = false;
    return;
  }
  const int WaitTimeMS = 20;
  std::string ThrowAwayMessage;
  if (LogMessages.time_out_peek(ThrowAwayMessage, WaitTimeMS)) {
    trySendMessage();
    return;
  }
  Service.post([this]() { this->waitForMessage(); });
}

void GraylogConnection::doAddressQuery() {
  if (IsMessagePolling) {
    reConnect(GraylogConnection::ReconnectDelay::SHORT);
  }
  SetState(Status::ADDR_LOOKUP);
  asio::ip::tcp::resolver::query Query(HostAddress, HostPort);
  auto HandlerGlue = [this](auto &Error, auto EndpointIter) {
    this->resolverHandler(Error, EndpointIter);
  };
  Resolver.async_resolve(Query, HandlerGlue);
}

GraylogConnection::~GraylogConnection() {
  Service.stop();
  Work.reset();
  AsioThread.join();
  Socket.close();
}

GraylogConnection::Status GraylogConnection::GetConnectionStatus() const {
  return ConnectionState;
}

void GraylogConnection::ThreadFunction() { Service.run(); }

void GraylogConnection::SetState(GraylogConnection::Status NewState) {
  ConnectionState = NewState;
}
