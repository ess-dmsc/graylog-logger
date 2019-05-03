/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Graylog-server interface header file.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/GraylogConnection.hpp"
#include "graylog_logger/LogUtil.hpp"

namespace Log {

class GraylogInterface : public BaseLogHandler, private GraylogConnection {
public:
  GraylogInterface(const std::string &Host, int Port,
                   size_t MaxQueueLength = 100);
  ~GraylogInterface() override = default;
  void addMessage(const LogMessage &Message) override;
  bool emptyQueue() override;
  size_t queueSize() override;
  using GraylogConnection::Status;
  using GraylogConnection::getConnectionStatus;

protected:
  std::string logMsgToJSON(const LogMessage &Message);
};

} // namespace Log
