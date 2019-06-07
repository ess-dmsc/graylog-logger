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
  GraylogConnection(std::string Host, int Port);
  virtual ~GraylogConnection();
  virtual void sendMessage(std::string Msg);
  virtual Status getConnectionStatus() const;
  virtual bool messageQueueEmpty();
  virtual size_t messageQueueSize();

private:
  class Impl;
  std::unique_ptr<Impl> Pimpl;
};
class GraylogInterface : public BaseLogHandler, public GraylogConnection {
public:
  GraylogInterface(const std::string &Host, int Port,
                   size_t MaxQueueLength = 100);
  ~GraylogInterface() override = default;
  void addMessage(const LogMessage &Message) override;
  bool emptyQueue() override;
  size_t queueSize() override;

protected:
  std::string logMsgToJSON(const LogMessage &Message);
};

} // namespace Log
