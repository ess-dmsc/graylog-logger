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

class GraylogInterface : public BaseLogHandler, private GraylogConnection {
public:
  GraylogInterface(const std::string &host, const int port,
                   const size_t maxQueueLength = 100);
  virtual ~GraylogInterface() = default;
  virtual void AddMessage(const LogMessage &msg) override;
  virtual bool MessagesQueued() override;
  virtual size_t QueueSize() override;
  using GraylogConnection::Status;
  using GraylogConnection::GetConnectionStatus;

protected:
  std::string LogMsgToJSON(const LogMessage &msg);
};
