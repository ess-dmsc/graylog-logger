
//  KafkaInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "graylog_logger/GraylogInterface.hpp"
#include <nlohmann/json.hpp>
#include <ciso646>
#include <cstring>

GraylogInterface::GraylogInterface(const std::string &host, const int port,
                                   const size_t maxQueueLength)
    : BaseLogHandler(maxQueueLength), GraylogConnection(host, port) {}

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
  nlohmann::json JsonObject;
  JsonObject["short_message"] = msg.message;
  JsonObject["version"] = "1.1";
  JsonObject["level"] = int(msg.severity);
  JsonObject["host"] = msg.host;
  JsonObject["timestamp"] =
      double(std::chrono::system_clock::to_time_t(msg.timestamp));
  JsonObject["_process_id"] = msg.processId;
  JsonObject["_process"] = msg.processName;
  JsonObject["_thread_id"] = msg.threadId;
  for (auto &field : msg.additionalFields) {
    if (AdditionalField::Type::typeStr == field.second.FieldType) {
      JsonObject["_" + field.first] = field.second.strVal;
    } else if (AdditionalField::Type::typeDbl == field.second.FieldType) {
      JsonObject["_" + field.first] = field.second.dblVal;
    } else if (AdditionalField::Type::typeInt == field.second.FieldType) {
      JsonObject["_" + field.first] = field.second.intVal;
    }
  }
  return JsonObject.dump();
}
