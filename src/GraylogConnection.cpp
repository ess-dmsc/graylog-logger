/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Implements the networking code for sending messages to a graylog
/// server.
///
//===----------------------------------------------------------------------===//

#include "GraylogConnection.hpp"
#include <chrono>
#include <ciso646>
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

void GraylogConnection::Impl::tryConnect(QueryResult AllEndpoints) {
  asio::ip::tcp::endpoint CurrentEndpoint = AllEndpoints.getNextEndpoint();
  auto HandlerGlue = [this, AllEndpoints](auto &Err) {
    this->connectHandler(Err, AllEndpoints);
  };
  Socket.async_connect(CurrentEndpoint, HandlerGlue);
  setState(Status::CONNECT);
}

GraylogConnection::Impl::Impl(std::string Host, int Port, size_t MaxQueueLength)
    : HostAddress(std::move(Host)), HostPort(std::to_string(Port)), Service(),
      Work(std::make_unique<asio::io_service::work>(Service)), Socket(Service),
      Resolver(Service), ReconnectTimeout(Service, 10s),
      LogMessages(MaxQueueLength) {
  doAddressQuery();
  AsioThread = std::thread(&GraylogConnection::Impl::threadFunction, this);
}

void GraylogConnection::Impl::resolverHandler(
    const asio::error_code &Error,
    asio::ip::tcp::resolver::iterator EndpointIter) {
  if (Error) {
    setState(Status::ADDR_RETRY_WAIT);
    reConnect(ReconnectDelay::LONG);
    return;
  }
  QueryResult AllEndpoints(std::move(EndpointIter));
  tryConnect(AllEndpoints);
}

void GraylogConnection::Impl::connectHandler(const asio::error_code &Error,
                                             const QueryResult &AllEndpoints) {
  if (!Error) {
    setState(Status::SEND_LOOP);
    auto HandlerGlue = [this](auto &Error, auto Size) {
      this->receiveHandler(Error, Size);
    };
    Socket.async_receive(asio::buffer(InputBuffer), HandlerGlue);
    trySendMessage();
    return;
  }
  Socket.close();
  if (AllEndpoints.isDone()) {
    reConnect(ReconnectDelay::LONG);
    return;
  }
  tryConnect(AllEndpoints);
}

void GraylogConnection::Impl::reConnect(ReconnectDelay Delay) {
  auto HandlerGlue = [this](auto & /* Err */) { this->doAddressQuery(); };
  switch (Delay) {
  case ReconnectDelay::SHORT:
    ReconnectTimeout.expires_after(100ms);
    break;
  case ReconnectDelay::LONG: // Fallthrough
  default:
    ReconnectTimeout.expires_after(10s);
    break;
  }
  ReconnectTimeout.async_wait(HandlerGlue);
  setState(Status::ADDR_RETRY_WAIT);
}

void GraylogConnection::Impl::receiveHandler(const asio::error_code &Error,
                                             std::size_t /* BytesReceived */) {
  if (Error) {
    Socket.close();
    reConnect(ReconnectDelay::SHORT);
    return;
  }
  auto HandlerGlue = [this](auto &Error, auto Size) {
    this->receiveHandler(Error, Size);
  };
  Socket.async_receive(asio::buffer(InputBuffer), HandlerGlue);
}

void GraylogConnection::Impl::trySendMessage() {
  if (not Socket.is_open()) {
    return;
  }
  auto HandlerGlue = [this](auto &Err, auto Size) {
    this->sentMessageHandler(Err, Size);
  };
  if (MessageBuffer.size() > MessageAdditionLimit) {
    asio::async_write(Socket, asio::buffer(MessageBuffer), HandlerGlue);
    return;
  }
  std::function<std::string(void)> NewMessageFunc;
  using namespace std::chrono_literals;
  if (LogMessages.wait_dequeue_timed(NewMessageFunc, 10ms)) {
    auto NewMessage = NewMessageFunc();
    if (NewMessage.empty()) {
      if (!MessageBuffer.empty()) {
        asio::async_write(Socket, asio::buffer(MessageBuffer), HandlerGlue);
      } else {
        Service.post([this]() { this->trySendMessage(); });
      }
      return;
    }
    std::copy(NewMessage.begin(), NewMessage.end(),
              std::back_inserter(MessageBuffer));
    MessageBuffer.push_back('\0');
    asio::async_write(Socket, asio::buffer(MessageBuffer), HandlerGlue);
  } else if (!MessageBuffer.empty()) {
    asio::async_write(Socket, asio::buffer(MessageBuffer), HandlerGlue);
  } else {
    Service.post([this]() { this->trySendMessage(); });
  }
}

void GraylogConnection::Impl::sentMessageHandler(const asio::error_code &Error,
                                                 std::size_t BytesSent) {
  if (BytesSent == MessageBuffer.size()) {
    MessageBuffer.clear();
  } else if (BytesSent > 0) {
    std::vector<char> TempVector;
    std::copy(MessageBuffer.begin() + BytesSent, MessageBuffer.end(),
              std::back_inserter(TempVector));
    MessageBuffer = TempVector;
  }
  if (Error) {
    Socket.close();
    return;
  }
  trySendMessage();
}

void GraylogConnection::Impl::doAddressQuery() {
  setState(Status::ADDR_LOOKUP);
  asio::ip::tcp::resolver::query Query(HostAddress, HostPort);
  auto HandlerGlue = [this](auto &Error, auto EndpointIter) {
    this->resolverHandler(Error, EndpointIter);
  };
  Resolver.async_resolve(Query, HandlerGlue);
}

GraylogConnection::Impl::~Impl() {
  Service.stop();
  AsioThread.join();
  try {
    Socket.close();
  } catch (asio::system_error &) {
    // Do nothing
  }
}

GraylogConnection::Impl::Status
GraylogConnection::Impl::getConnectionStatus() const {
  return ConnectionState;
}

void GraylogConnection::Impl::threadFunction() { Service.run(); }

void GraylogConnection::Impl::setState(
    GraylogConnection::Impl::Status NewState) {
  ConnectionState = NewState;
}

bool GraylogConnection::Impl::flush(
    std::chrono::system_clock::duration TimeOut) {
  auto WorkDone = std::make_shared<std::promise<void>>();
  auto WorkDoneFuture = WorkDone->get_future();
  LogMessages.try_enqueue([WorkDone = std::move(WorkDone)]() -> std::string {
    WorkDone->set_value();
    return {};
  });
  return std::future_status::ready == WorkDoneFuture.wait_for(TimeOut);
}

} // namespace Log
