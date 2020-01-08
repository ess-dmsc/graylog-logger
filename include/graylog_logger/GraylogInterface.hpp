/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Graylog-server interface header file.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/ConnectionStatus.hpp"
#include "graylog_logger/LogUtil.hpp"

namespace Log {
class GraylogConnection {
public:
  using Status = Log::Status;
  GraylogConnection(std::string Host, int Port, size_t MaxQueueSize);
  virtual ~GraylogConnection();
  virtual void sendMessage(std::string Msg);
  virtual Status getConnectionStatus() const;
  virtual bool messageQueueEmpty();
  virtual size_t messageQueueSize();
  virtual bool flush(std::chrono::system_clock::duration TimeOut);

private:
  class Impl;
  std::unique_ptr<Impl> Pimpl;
};
class GraylogInterface : public BaseLogHandler, public GraylogConnection {
public:
  GraylogInterface(const std::string &Host, int Port,
                   size_t MaxQueueLength = 1000);
  ~GraylogInterface() override = default;
  void addMessage(const LogMessage &Message) override;
  /// \brief Waits for all messages created before the call to flush to be
  /// transmitted.
  /// \param[in] TimeOut Amount of time to wait for messages to be transmitted.
  /// \return Returns true if messages were transmitted before the time out.
  /// Returns false otherwise.
  /// \note It is possible (though unlikely) that not all log messages have
  /// been transmitted even if flush() returns true.
  bool flush(std::chrono::system_clock::duration TimeOut) override;

  /// \brief Are there any more queued messages?
  /// \note The message queue will show as empty before the last message in
  /// the queue has been transmitted.
  /// \return Returns true if message queue is empty.
  bool emptyQueue() override;

  /// \brief Number of queued messages.
  /// \return Due to multiple threads accessing this queue, shows approximate
  /// number of messages in the queue.
  size_t queueSize() override;

protected:
  std::string logMsgToJSON(const LogMessage &Message);
};

} // namespace Log
