//
//  BaseLogHandlerStandIn.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-20.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include "graylog_logger/LogUtil.hpp"

using namespace Log;

class BaseLogHandlerStandIn : public BaseLogHandler {
public:
  BaseLogHandlerStandIn() {};
  void addMessage(const LogMessage &Message) override {
    CurrentMessage = Message;
  };
  LogMessage CurrentMessage;
  using BaseLogHandler::MessageParser;
  using BaseLogHandler::messageToString;
  bool flush(std::chrono::system_clock::duration) override { return true; }
  bool emptyQueue() override { return true; }
  size_t queueSize() override { return 0; }
};
