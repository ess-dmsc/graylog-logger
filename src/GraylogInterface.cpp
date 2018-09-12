/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief The interface implementation for sending messages to a graylog
/// server.
///
//===----------------------------------------------------------------------===//

#include "graylog_logger/GraylogInterface.hpp"
#include <ciso646>
#include <cstring>
#include <nlohmann/json.hpp>

namespace Log {

GraylogInterface::GraylogInterface(const std::string &Host, const int Port,
                                   const size_t MaxQueueLength)
    : BaseLogHandler(MaxQueueLength), GraylogConnection(Host, Port) {}

bool GraylogInterface::emptyQueue() {
  return GraylogConnection::LogMessages.size() > 0;
}

size_t GraylogInterface::queueSize() {
  return GraylogConnection::LogMessages.size();
}

void GraylogInterface::addMessage(const LogMessage &Message) {
  if (GraylogConnection::LogMessages.size() < BaseLogHandler::QueueLength) {
    SendMessage(logMsgToJSON(Message));
  }
}

std::string GraylogInterface::logMsgToJSON(const LogMessage &Message) {
  using namespace std::chrono;
  nlohmann::json JsonObject;
  JsonObject["short_message"] = Message.MessageString;
  JsonObject["version"] = "1.1";
  JsonObject["level"] = int(Message.SeverityLevel);
  JsonObject["host"] = Message.Host;
  JsonObject["timestamp"] =
      static_cast<double>(duration_cast<milliseconds>(
                              Message.Timestamp.time_since_epoch())
                              .count()) /
      1000;
  JsonObject["_process_id"] = Message.ProcessId;
  JsonObject["_process"] = Message.ProcessName;
  JsonObject["_thread_id"] = Message.ThreadId;
  for (auto &field : Message.AdditionalFields) {
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
  
  } // namespace Log
