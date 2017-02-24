
//  KafkaInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <cstring>
#include <ciso646>
#include "graylog_logger/json.h"
#include "graylog_logger/GraylogInterface.hpp"


GraylogInterface::GraylogInterface(std::string host, int port, size_t maxQueueLength) : GraylogConnection(host, port), BaseLogHandler(maxQueueLength) {
}

GraylogInterface::~GraylogInterface() {
    
}

bool GraylogInterface::MessagesQueued() {
    return GraylogConnection::logMessages.size() > 0;
}

size_t GraylogInterface::QueueSize() {
    return GraylogConnection::logMessages.size();
}

void GraylogInterface::AddMessage(const LogMessage &msg) {
    if (GraylogConnection::logMessages.size() < BaseLogHandler::queueLength) {
        SendMessage(LogMsgToJSON(msg));
    }
}

std::string GraylogInterface::LogMsgToJSON(const LogMessage &msg) {
    Json::Value root;
    root["short_message"] = msg.message;
    root["version"] = "1.1";
    root["level"] = int(msg.severity);
    root["host"] = msg.host;
    root["timestamp"] = double(std::chrono::system_clock::to_time_t(msg.timestamp));
    root["_process_id"] = msg.processId;
    root["_process"] = msg.processName;
    root["_thread_id"] = msg.threadId;
    for (auto &field : msg.additionalFields) {
        if (AdditionalField::Type::typeStr == field.second.FieldType) {
            root["_" + field.first] = field.second.strVal;
        } else if (AdditionalField::Type::typeDbl == field.second.FieldType) {
            root["_" + field.first] = field.second.dblVal;
        } else if (AdditionalField::Type::typeInt == field.second.FieldType) {
            root["_" + field.first] = field.second.intVal;
        }
    }
    Json::FastWriter writer;
    return writer.write(root);
}
