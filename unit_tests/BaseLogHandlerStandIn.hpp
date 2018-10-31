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
  BaseLogHandlerStandIn() : BaseLogHandler(){};
  virtual void addMessage(const LogMessage &Message) {
    CurrentMessage = Message;
  };
  LogMessage CurrentMessage;
  using BaseLogHandler::MessageParser;
  using BaseLogHandler::messageToString;
};
