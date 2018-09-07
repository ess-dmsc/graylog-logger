//
//  KafkaInterface.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

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
