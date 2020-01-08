/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Header file of console interface.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/LogUtil.hpp"
#include "graylog_logger/ThreadedExecutor.hpp"

namespace Log {

class ConsoleInterface : public BaseLogHandler {
public:
  explicit ConsoleInterface();
  void addMessage(const LogMessage &Message) override;
  /// \brief Waits for all messages created before the call to flush to be
  /// printed and then flushes the output stream.
  /// \param[in] TimeOut Amount of time to wait for messages to be written.
  /// \return Returns true if queue was emptied and stream flushed before the
  /// time out. Returns false otherwise.
  bool flush(std::chrono::system_clock::duration TimeOut) override;

  /// \brief Are there any more queued messages?
  /// \note The message queue will show as empty before the last message in
  /// the queue has been written to console.
  /// \return Returns true if message queue is empty.
  bool emptyQueue() override;

  /// \brief Number of queued messages.
  /// \return Due to multiple threads accessing this queue, shows approximate
  /// number of messages in the queue.
  size_t queueSize() override;

protected:
  ThreadedExecutor Executor; // Must be last
};

} // namespace Log
