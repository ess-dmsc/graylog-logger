/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Contains basic structures and classes required by the graylog-logger
/// library.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/ConcurrentQueue.hpp"
#include <chrono>
#include <functional>
#include <memory>
#include <string>

namespace Log {

typedef std::chrono::time_point<std::chrono::system_clock> system_time;

/// \brief Severity levels known by the system.
///
/// Based on syslog severity levels.
/// \note That there are two names for severity level 6 (Informational and
/// Info).
enum class Severity : int {
  Emergency = 0,
  Alert = 1,
  Critical = 2,
  Error = 3,
  Warning = 4,
  Notice = 5,
  Informational = 6,
  Info = 6,
  Debug = 7,
};

/// \brief Used to store multiple different types for the extra fields provided
/// to the library.
struct AdditionalField {
  /// \brief Sets the instance of this struct to contain an empty string.
  AdditionalField() : FieldType(Type::typeStr), intVal(0), dblVal(0){};

  /// \brief Sets the instance of this struct to contain a floating point value
  /// (double).
  /// \param[in] val The floating-point value that will be stored by the struct.
  AdditionalField(double val)
      : FieldType(Type::typeDbl), intVal(0), dblVal(val){};
  /// \brief Sets the instance of this struct to contain a std::string.
  /// \param[in] val The std::string value that will be stored by the struct.
  AdditionalField(const std::string &val)
      : FieldType(Type::typeStr), strVal(val), intVal(0), dblVal(0){};

  /// \brief Sets the instance of this struct to contain a signed integer value.
  /// \param[in] val The signed integer value that will be stored by the struct.
  AdditionalField(std::int64_t val)
      : FieldType(Type::typeInt), intVal(val), dblVal(0){};

  /// \brief The enum class used to keep track of which data type it is that we
  /// are using.
  enum class Type : char {
    typeStr = 0,
    typeDbl = 1,
    typeInt = 2,
  } FieldType;
  std::string strVal;
  std::int64_t intVal;
  double dblVal;
};

/// \brief The log message struct used by the logging library to pass messages
/// to the different consumers.
struct LogMessage {
  LogMessage() : SeverityLevel(Severity::Debug){};
  std::string MessageString;
  system_time Timestamp;
  int ProcessId;
  std::string ProcessName;
  std::string Host;
  Severity SeverityLevel;
  std::string ThreadId;
  std::vector<std::pair<std::string, AdditionalField>> AdditionalFields;
  template <typename valueType>
  void AddField(std::string Key, const valueType &Value) {
    int FieldLoc = -1;
    for (size_t i = 0; i < AdditionalFields.size(); i++) {
      if (AdditionalFields[i].first == Key) {
        FieldLoc = i;
        break;
      }
    }
    if (-1 == FieldLoc) {
      AdditionalFields.push_back({Key, Value});
    } else {
      AdditionalFields[FieldLoc] = {Key, Value};
    }
  }
};

/// \brief The base class used to implement log message consumers.
///
/// Inherit from this class when implementing your own log message handler.
/// Implements queuing of log messages.
class BaseLogHandler {
public:
  /// \brief Does minimal set-up of the this class.
  /// \param[in] maxQueueLength The maximum number of log messages stored.
  BaseLogHandler(const size_t maxQueueLength = 100);
  virtual ~BaseLogHandler() = default;
  /// \brief Called by the logging library when a new log message is created.
  /// \note If the queue of messages is full, any new messages are discarded
  /// without any indication that this has been done.
  /// \param[in] Message The log message.
  virtual void addMessage(const LogMessage &Message);
  /// \brief Are there messages in the queue?
  /// \note As messages can be added and removed by several different threads,
  /// expect that the return value will change between two calls.
  /// \return true if there are one or more messages in the queue, otherwise
  /// false.
  virtual bool emptyQueue();
  /// \brief The number of messages in the queue.
  /// \note As messages can be added and removed by several different threads,
  /// expect that the return value will change between two calls.
  /// \return The number of messages in the queue.
  virtual size_t queueSize();
  /// \brief Used to set a custom log message to std::string formatting
  /// function.
  ///
  /// \note Use of this functionality can result in crashes if the function
  /// uses resources that are de-allocated before the logging system is
  /// de-allocated.
  /// \param[in] ParserFunction The new custom message parsing function.
  void setMessageStringCreatorFunction(
      std::function<std::string(const LogMessage &)> ParserFunction);

protected:
  size_t QueueLength;
  /// \brief Pop messages from this queue if implementing a log message
  /// consumer (handler).
  ConcurrentQueue<LogMessage> MessageQueue;
  /// \brief Can be used to create strings from messages if set.
  std::function<std::string(const LogMessage &)> MessageParser{nullptr};
  /// \brief The default log message to std::string function.
  std::string messageToString(const LogMessage &Message);
};

typedef std::shared_ptr<BaseLogHandler> LogHandler_P;

} // namespace Log
