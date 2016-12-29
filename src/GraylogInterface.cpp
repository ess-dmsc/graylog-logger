
//  KafkaInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <strings.h>
#include "graylog_logger/json.h"
#include "graylog_logger/GraylogInterface.hpp"


GraylogInterface::GraylogInterface(std::string host, int port, int queueLength) : GraylogConnection(host, port, queueLength){
}

GraylogInterface::~GraylogInterface() {
    
}

void GraylogInterface::AddMessage(LogMessage &msg) {
    SendMessage(LogMsgToJSON(msg));
}

std::string GraylogInterface::LogMsgToJSON(LogMessage &msg) {
    Json::Value root;
    root["short_message"] = msg.message;
    root["version"] = "1.1";
    root["level"] = int(msg.severity);
    root["host"] = msg.host;
    root["timestamp"] = double(std::chrono::system_clock::to_time_t(msg.timestamp));
    root["_process_id"] = msg.processId;
    root["_process"] = msg.processName;
    root["_thread_id"] = msg.threadId;
    Json::FastWriter writer;
    return writer.write(root);
}
