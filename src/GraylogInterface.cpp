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
#include "GraylogConnection.hpp"
#include <ciso646>
#include <cstring>
#include <nlohmann/json.hpp>

namespace Log {

GraylogConnection::GraylogConnection(std::string Host, int Port,
                                     size_t MaxQueueSize)
    : Pimpl(std::make_unique<GraylogConnection::Impl>(std::move(Host), Port,
                                                      MaxQueueSize)) {}

void GraylogConnection::sendMessage(std::string Msg) {
  Pimpl->sendMessage(std::move(Msg));
}

bool GraylogConnection::flush(std::chrono::system_clock::duration TimeOut) {
  return Pimpl->flush(TimeOut);
}

Status GraylogConnection::getConnectionStatus() const {
  return Pimpl->getConnectionStatus();
}

bool GraylogConnection::messageQueueEmpty() { return Pimpl->queueSize() == 0; }

size_t GraylogConnection::messageQueueSize() { return Pimpl->queueSize(); }

GraylogConnection::~GraylogConnection() = default;

GraylogInterface::GraylogInterface(const std::string &Host, const int Port,
                                   const size_t MaxQueueLength)
    : GraylogConnection(Host, Port, MaxQueueLength) {}

void GraylogInterface::addMessage(const LogMessage &Message) {
  sendMessage(logMsgToJSON(Message));
}

std::string GraylogInterface::logMsgToJSON(const LogMessage &Message) {
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;

  nlohmann::json JsonObject;
  JsonObject["short_message"] = Message.MessageString;
  JsonObject["version"] = "1.1";
  JsonObject["level"] = int(Message.SeverityLevel);
  JsonObject["host"] = Message.Host;
  JsonObject["timestamp"] =
      static_cast<double>(
          duration_cast<milliseconds>(Message.Timestamp.time_since_epoch())
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

bool GraylogInterface::flush(std::chrono::system_clock::duration TimeOut) {
  return GraylogConnection::flush(TimeOut);
}

bool GraylogInterface::emptyQueue() { return messageQueueEmpty(); }

size_t GraylogInterface::queueSize() { return messageQueueSize(); }

} // namespace Log
