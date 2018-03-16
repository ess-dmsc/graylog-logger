//
//  BaseLogHandlerStandIn.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-20.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include "graylog_logger/LogUtil.hpp"

class BaseLogHandlerStandIn : public BaseLogHandler {
public:
  BaseLogHandlerStandIn() : BaseLogHandler(){};
  virtual void AddMessage(const LogMessage &msg) { cMsg = msg; };
  LogMessage cMsg;
  using BaseLogHandler::MsgStringCreator;
  using BaseLogHandler::MessageParser;
};
