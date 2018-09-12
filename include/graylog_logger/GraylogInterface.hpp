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
  GraylogInterface(const std::string &Host, const int Port,
                   const size_t MaxQueueLength = 100);
  virtual ~GraylogInterface() = default;
  virtual void addMessage(const LogMessage &Message) override;
  virtual bool emptyQueue() override;
  virtual size_t queueSize() override;
  using GraylogConnection::Status;
  using GraylogConnection::GetConnectionStatus;

protected:
  std::string logMsgToJSON(const LogMessage &Message);
};

  } // namespace Log
