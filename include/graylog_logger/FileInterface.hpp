/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Header file of logging interface.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/LogUtil.hpp"
#include "graylog_logger/ThreadedExecutor.hpp"
#include <fstream>
#include <string>

namespace Log {

class FileInterface : public BaseLogHandler {
public:
  explicit FileInterface(std::string const &Name,
                         const size_t MaxQueueLength = 100);
  void addMessage(const LogMessage &Message) override;

  /// \brief Waits for all messages created before the call to flush to be
  /// printed and then flushes the file stream.
  /// \param[in] TimeOut Amount of time to wait for messages to be written.
  /// \return Returns true if queue was emptied and stream flushed before the
  /// time out. Returns false otherwise.
  bool flush(std::chrono::system_clock::duration TimeOut) override;

  /// \brief Are there any queued messages?
  /// \note The message queue will show as empty before the last message in
  /// the queue has been written.
  /// \return Returns true if message queue is empty.
  bool emptyQueue() override;

  ///  \brief Number of queued messages.
  ///  \return Due to multiple threads accessing this queue, shows approximate
  ///  number of messages in the queue.
  size_t queueSize() override;

  /// \brief See parent class for documentation.
  void setMessageStringCreatorFunction(
      std::function<std::string(const LogMessage &)> ParserFunction) override;

protected:
  std::ofstream FileStream;
  ThreadedExecutor Executor; // Must be last
};

} // namespace Log
