
//  KafkaInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "graylog_logger/GraylogInterface.hpp"
#include <ciso646>
#include <cstring>
#include <nlohmann/json.hpp>

GraylogInterface::GraylogInterface(const std::string &host, const int port,
                                   const size_t maxQueueLength)
    : BaseLogHandler(maxQueueLength), GraylogConnection(host, port) {}

bool GraylogInterface::MessagesQueued() {
  return GraylogConnection::LogMessages.size() > 0;
}

size_t GraylogInterface::QueueSize() {
  return GraylogConnection::LogMessages.size();
}

void GraylogInterface::AddMessage(const LogMessage &msg) {
  if (GraylogConnection::LogMessages.size() < BaseLogHandler::queueLength) {
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
      static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
                              msg.timestamp.time_since_epoch())
                              .count()) /
      1000;
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
