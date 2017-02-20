//
//  KafkaInterface.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include "graylog_logger/LogUtil.hpp"
#include "graylog_logger/GraylogConnection.hpp"

class GraylogInterface : public BaseLogHandler, private GraylogConnection {
public:
    GraylogInterface(std::string host, int port, size_t maxQueueLength = 100);
    virtual ~GraylogInterface();
    virtual void AddMessage(const LogMessage &msg) override;
    virtual bool MessagesQueued() override;
    virtual size_t QueueSize() override;
    using GraylogConnection::GetConnectionStatus;
    using GraylogConnection::ConStatus;
protected:
    std::string LogMsgToJSON(const LogMessage &msg);
};
