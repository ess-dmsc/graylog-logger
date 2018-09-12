/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Implements the networking code for sending messages to a graylog
/// server.
///
//===----------------------------------------------------------------------===//

#include "graylog_logger/GraylogConnection.hpp"
#include <chrono>
#include <ciso646>
#include <iostream>
#include <utility>

namespace Log {

using std::chrono_literals::operator""ms;
using std::chrono_literals::operator""s;

struct QueryResult {
  explicit QueryResult(asio::ip::tcp::resolver::iterator &&Endpoints)
      : EndpointIterator(std::move(Endpoints)) {
    while (EndpointIterator != asio::ip::tcp::resolver::iterator()) {
      auto CEndpoint = *EndpointIterator;
      EndpointList.push_back(CEndpoint);
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

void GraylogConnection::tryConnect(QueryResult AllEndpoints) {
  asio::ip::tcp::endpoint CurrentEndpoint = AllEndpoints.getNextEndpoint();
  auto HandlerGlue = [this, AllEndpoints](auto &Err) {
    this->connectHandler(Err, AllEndpoints);
  };
  Socket.async_connect(CurrentEndpoint, HandlerGlue);
  SetState(Status::CONNECT);
}

GraylogConnection::GraylogConnection(std::string Host, int Port)
    : HostAddress(std::move(Host)), HostPort(std::to_string(Port)), Service(),
      Work(new asio::io_service::work(Service)), Socket(Service),
      Resolver(Service), ReconnectTimeout(Service, 10s) {
  doAddressQuery();
  AsioThread = std::thread(&GraylogConnection::ThreadFunction, this);
}

void GraylogConnection::resolverHandler(
    const asio::error_code &Error,
    asio::ip::tcp::resolver::iterator EndpointIter) {
  if (Error) {
    SetState(Status::ADDR_RETRY_WAIT);
    reconnect(ReconnectDelay::LONG);
    return;
  }
  QueryResult AllEndpoints(std::move(EndpointIter));
  tryConnect(AllEndpoints);
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
    reconnect(ReconnectDelay::LONG);
    return;
  }
  tryConnect(AllEndpoints);
}

void GraylogConnection::reconnect(ReconnectDelay Delay) {
  auto HandlerGlue = [this](auto &Err) { this->doAddressQuery(); };
  switch (Delay) {
  case ReconnectDelay::SHORT:
    ReconnectTimeout.expires_after(50ms);
    break;
  case ReconnectDelay::LONG: // Fallthrough
  default:
    ReconnectTimeout.expires_after(10s);
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
    reconnect(ReconnectDelay::SHORT);
    return;
  }
  auto HandlerGlue = [this](auto &Error, auto Size) {
    this->receiveHandler(Error, Size);
  };
  Socket.async_receive(asio::buffer(InputBuffer), HandlerGlue);
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
    reconnect(GraylogConnection::ReconnectDelay::SHORT);
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

} // namespace Log
