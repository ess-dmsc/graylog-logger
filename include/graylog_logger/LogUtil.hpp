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
  /// \param[in] val The float value that will be stored by the struct.
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
  LogMessage() : severity(Severity::Debug){};
  std::string message;
  system_time timestamp;
  int processId;
  std::string processName;
  std::string host;
  Severity severity;
  std::string threadId;
  std::vector<std::pair<std::string, AdditionalField>> additionalFields;
  template <typename valueType>
  void AddField(std::string key, const valueType &val) {
    int fieldLoc = -1;
    for (size_t i = 0; i < additionalFields.size(); i++) {
      if (additionalFields[i].first == key) {
        fieldLoc = i;
        break;
      }
    }
    if (-1 == fieldLoc) {
      additionalFields.push_back({key, val});
    } else {
      additionalFields[fieldLoc] = {key, val};
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
  /// \note If the queue of messages is full; any new messages are discarded
  /// without any indication that this has been done.
  /// \param[in] msg The log message.
  virtual void AddMessage(const LogMessage &msg);
  /// \brief Are there messages in the queue?
  /// \note As messages can be added and removed by several different threads,
  /// expect that the return value will change between two calls.
  /// \return true if there are one or more messages in the queue, otherwise
  /// false.
  virtual bool MessagesQueued();
  /// \brief The number of messages in the queue.
  /// \note As messages can be added and removed by several different threads,
  /// expect that the return value will change between two calls.
  /// \return The number of messages in the queue.
  virtual size_t QueueSize();
  /// \brief Used to
  void SetMessageStringCreatorFunction(
      std::function<std::string(const LogMessage &)> ParserFunction);

protected:
  size_t queueLength;
  /// \brief Pop messages from this queue if implementing a log message
  /// consumer (handler).
  ConcurrentQueue<LogMessage> logMessages;
  /// \brief Can be used to create strings from messages if set.
  std::function<std::string(const LogMessage &)> MessageParser;
  /// \brief The default log message to std::string function.
  std::string MsgStringCreator(const LogMessage &msg);
};

typedef std::shared_ptr<BaseLogHandler> LogHandler_P;
