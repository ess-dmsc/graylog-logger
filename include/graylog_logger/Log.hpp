/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Header file with declarations of the functions used to interface to
/// the library.
///
//===----------------------------------------------------------------------===//

#pragma once

#include "graylog_logger/LibConfig.hpp"
#include "graylog_logger/LogUtil.hpp"
#include <vector>

#ifdef WITH_FMT
#include "graylog_logger/Logger.hpp"
namespace Log {

template <typename T> struct convert { using type = T; };

template <> struct convert<char const *> { using type = std::string; };

template <> struct convert<char *> { using type = std::string; };

template <typename T> using charStarToString = typename convert<T>::type;

/// \brief Submit a formatted message to the logging library.
///
/// The following fields will be added to the message by the function:
///   - Timestamp
///   - Process name
///   - Process id
///   - Thread id
///   - Host name
///
/// \note If an exception is encountered when generating the formatted text
/// string, a log message showing the `Format` string as well as the error
/// message will be generated in its place.
///
/// \param[in] Level The severity level of the message.
/// \param[in] Format The (fmtlib) format of the text message.
/// \param[in] args The variables to be inserted into the format string.
template <typename... Args>
void FmtMsg(const Severity Level, const std::string Format, Args... args) {
  Logger::Inst().fmt_log<charStarToString<Args>...>(Level, Format, args...);
}
} // namespace Log
#endif

namespace Log {
/// \brief Submit a log message to the logging library.
///
/// The following fields will be added to the message by the function:
///   - Timestamp
///   - Process name
///   - Process id
///   - Thread id
///   - Host name
///
/// \param[in] Level The severity level of the message.
/// \param[in] Message The log message as text.
void Msg(const Severity Level, const std::string &Message);

/// \brief Submit a log message to the logging library.
///
/// The following fields will be added to the message by the function:
///   - Timestamp
///   - Process name
///   - Process id
///   - Thread id
///   - Host name
///
/// \param[in] Level The severity level of the message as an integer value.
/// Should probably fall within the range 0 to 7.
/// \param[in] Message The log message as text.
void Msg(const int Level, const std::string &Message);

/// \brief Submit a log message to the logging library.
///
/// The following fields will be added to the message by the function:
///   - Timestamp
///   - Process name
///   - Process id
///   - Thread id
///   - Host name
///
/// \param[in] Level The severity level of the message.
/// \param[in] Message The log message as text.
/// \param[in] ExtraField An extra field of information about the log message.
void Msg(const Severity Level, const std::string &Message,
         const std::pair<std::string, AdditionalField> &ExtraField);

/// \brief Submit a log message to the logging library.
///
/// The following fields will be added to the message by the function:
///   - Timestamp
///   - Process name
///   - Process id
///   - Thread id
///   - Host name
///
/// \param[in] Level The severity level of the message as an integer value.
/// Should probably fall within the range 0 to 7.
/// \param[in] Message The log message as text.
/// \param[in] ExtraField An extra field of information about the log message.
void Msg(const int Level, const std::string &Message,
         const std::pair<std::string, AdditionalField> &ExtraField);

/// \brief Submit a log message to the logging library.
///
/// The following fields will be added to the message by the function:
///   - Timestamp
///   - Process name
///   - Process id
///   - Thread id
///   - Host name
///
/// \param[in] Level The severity level of the message.
/// \param[in] Message The log message as text.
/// \param[in] ExtraFields Multiple extra meta-data fields about the message.
void Msg(
    const Severity Level, const std::string &Message,
    const std::vector<std::pair<std::string, AdditionalField>> &ExtraFields);

/// \brief Submit a log message to the logging library.
///
/// The following fields will be added to the message by the function:
///   - Timestamp
///   - Process name
///   - Process id
///   - Thread id
///   - Host name
///
/// \param[in] Level The severity level of the message as an integer value.
/// Should probably fall within the range 0 to 7.
/// \param[in] Message The log message as text.
/// \param[in] ExtraFields Multiple extra meta-data fields about the message.
void Msg(
    const int Level, const std::string &Message,
    const std::vector<std::pair<std::string, AdditionalField>> &ExtraFields);

/// \brief Flush log messages in the queues of the log handlers.
///
/// \note Exact implementation depends on that of the currently used log
/// handlers.
/// \note For the built-in log handlers, flush only guarantees that log
/// messages created before the call to Flush() have been handled. Log messages
/// created after the call to Flush() can not be guaranteed to have been
/// handled.
/// \param[in] TimeOut The amount of time to wait for the queued messages to
/// be flushed. Defaults to 500ms.
/// \return Returns true if messages were flushed, false otherwise.
bool Flush(std::chrono::system_clock::duration TimeOut =
               std::chrono::milliseconds(500));

/// \brief Add a default field of meta-data to every message.
///
/// It is possible to override the value of the default message by passing
/// an additional field using the relevant key in one of the Log::Msg()
/// functions.
/// \param[in] Key The key of the new default field.
/// \param[in] Value The default value of the field.
void AddField(const std::string &Key, const AdditionalField &Value);

/// \brief Used to set the maximum severity level.
///
/// Messages above this level will not be passed on. The default level is
/// Severity::Notice, i.e. debug level messages will not be shown.
/// \param[in] Level The maximum severity level.
void SetMinimumSeverity(const Severity Level);

/// \brief Add a log handler that will consume log messages.
///
/// It is possible to use one of the log handlers provided with this library
/// or it is possible to implement your own by creating a class that inherits
/// from Log::BaseLogHandler.
/// \note If you try to add a log handler that inherits from ConsoleLogger,
/// it will replace the existing one if the logging library has one already.
/// \param[in] Handler A shared pointer to the log handler.
void AddLogHandler(const LogHandler_P &Handler);

/// \brief Add a log handler that will consume log messages.
///
/// It is possible to use one of the log handlers provided with this library
/// or it is possible to implement your own by creating a class that inherits
/// from Log::BaseLogHandler.
/// \note If you try to add a log handler that inherits from ConsoleLogger,
/// it will replace the existing one if the logging library has one already.
/// \param[in] Handler A raw pointer to the instance of the log handler.
/// \note The logging library will take ownership of the pointer. You MUST NOT
/// de-allocate it yourself.
template <typename T>
typename std::enable_if<
    std::is_base_of<BaseLogHandler, T>::value /*, some_type*/>::type
AddLogHandler(T *ptr) {
  AddLogHandler(LogHandler_P(dynamic_cast<BaseLogHandler *>(ptr)));
}

/// \brief Add a log handler that will consume log messages.
///
/// It is possible to use one of the log handlers provided with this library
/// or it is possible to implement your own by creating a class that inherits
/// from Log::BaseLogHandler.
/// \note If you try to add a log handler that inherits from ConsoleLogger,
/// it will replace the existing one if the logging library has one already.
/// \param[in] Handler A raw pointer to the base class of the log handler.
/// \note The logging library will take ownership of the pointer. You MUST NOT
/// de-allocate it yourself.
void AddLogHandler(const BaseLogHandler *Handler);

/// \brief Remove all log message handlers.
///
/// Clears the vector of handlers. If no other shared pointer for a handler
/// exists, the handler is de-allocated.
void RemoveAllHandlers();

/// \brief Get a copy of the vector containing the handlers.
/// \return A std::vector containing shared pointers to handlers known by the
/// logging system.
std::vector<LogHandler_P> GetHandlers();
} // namespace Log
