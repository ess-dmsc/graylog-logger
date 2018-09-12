/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Implementation of the base logging class.
///
//===----------------------------------------------------------------------===//

#include "graylog_logger/LoggingBase.hpp"
#include <chrono>
#include <ciso646>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <thread>

#ifdef _WIN32
#include <Winsock2.h>
#include <codecvt>
#include <locale>
#include <process.h>
#include <windows.h>
#define getpid _getpid
#else
#include <unistd.h>
#endif

namespace Log {

#ifdef _WIN32
std::string get_process_name() {
  std::wstring buf;
  buf.resize(260);
  do {
    size_t len =
        GetModuleFileNameW(nullptr, &buf[0], static_cast<size_t>(buf.size()));
    if (len < buf.size()) {
      buf.resize(len);
      break;
    }
    buf.resize(buf.size() * 2);
  } while (buf.size() < 65536);

  int lastSlash = buf.rfind(L"\"");
  if (std::string::npos != lastSlash) {
    buf = buf.substr(lastSlash + 1, buf.size() - 1);
  }
  using convert_typeX = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_typeX, wchar_t> converterX;

  return converterX.to_bytes(buf);
}
#elif defined(__APPLE__) || defined(__APPLE_CC__)
#include <cstring>
#include <mach-o/dyld.h>
#include <sys/syslimits.h>
std::string get_process_name() {
  std::string buf;
  buf.resize(PATH_MAX);
  while (true) {
    auto size = static_cast<uint32_t>(buf.size());
    if (_NSGetExecutablePath(&buf[0], &size) == 0) {
      buf.resize(std::strlen(&buf[0]));
      break;
    }
    buf.resize(size);
  }
  auto lastSlash = buf.rfind('/');
  if (std::string::npos == lastSlash) {
    return buf;
  }
  return buf.substr(lastSlash + 1, buf.size() - 1);
}
#else
#include <vector>
std::string get_process_name() {
  std::vector<std::string> filePaths = {"/proc/self/exe", "/proc/curproc/file",
                                        "/proc/curproc/exe"};
  char pathBuffer[1024];
  for (auto &path : filePaths) {
    int nameLen = readlink(path.c_str(), pathBuffer, sizeof(pathBuffer) - 1);
    if (-1 != nameLen) {
      std::string tempPath(pathBuffer, nameLen);
      auto lastSlash = tempPath.rfind("/");
      if (std::string::npos == lastSlash) {
        return tempPath;
      }
      return tempPath.substr(lastSlash + 1, tempPath.size() - 1);
    }
  }
  return std::to_string(getpid());
}
#endif

LoggingBase::LoggingBase() {
  std::lock_guard<std::mutex> guard1(VectorMutex);
  MinSeverity = Severity::Notice;
  const int stringBufferSize = 100;
  char stringBuffer[stringBufferSize];
  int res;
  res = gethostname(stringBuffer, stringBufferSize);
  std::lock_guard<std::mutex> guard2(BaseMsgMutex);
  if (0 == res) {
    BaseMsg.Host = std::string(stringBuffer);
  }
  BaseMsg.ProcessId = getpid();
  BaseMsg.ProcessName = get_process_name();
}

LoggingBase::~LoggingBase() {
  std::lock_guard<std::mutex> guard(VectorMutex);
  Handlers.clear();
}

void LoggingBase::log(const Severity Level, const std::string &Message) {
  log(Level, Message, std::vector<std::pair<std::string, AdditionalField>>());
}

void LoggingBase::log(
    const Severity Level, const std::string &Message,
    const std::pair<std::string, AdditionalField> &ExtraField) {
  log(Level, Message, std::vector<std::pair<std::string, AdditionalField>>{
                          ExtraField,
                      });
}

void LoggingBase::log(
    const Severity Level, const std::string &Message,
    const std::vector<std::pair<std::string, AdditionalField>> &ExtraFields) {
  if (int(Level) > int(MinSeverity)) {
    return;
  }
  BaseMsgMutex.lock();
  LogMessage cMsg(BaseMsg);
  BaseMsgMutex.unlock();
  for (auto &fld : ExtraFields) {
    cMsg.addField(fld.first, fld.second);
  }
  cMsg.Timestamp = std::chrono::system_clock::now();
  cMsg.MessageString = Message;
  cMsg.SeverityLevel = Level;
  std::ostringstream ss;
  ss << std::this_thread::get_id();
  cMsg.ThreadId = ss.str();
  std::lock_guard<std::mutex> guard(VectorMutex);
  for (auto &ptr : Handlers) {
    ptr->addMessage(cMsg);
  }
}

void LoggingBase::addLogHandler(const LogHandler_P &Handler) {
  std::lock_guard<std::mutex> guard(VectorMutex);
  Handlers.push_back(Handler);
}

void LoggingBase::removeAllHandlers() {
  std::lock_guard<std::mutex> guard(VectorMutex);
  Handlers.clear();
}

std::vector<LogHandler_P> LoggingBase::getHandlers() {
  std::lock_guard<std::mutex> guard(VectorMutex);
  return Handlers;
}

void LoggingBase::setMinSeverity(Severity Level) {
  std::lock_guard<std::mutex> guard(VectorMutex);
  MinSeverity = Level;
}

} // namespace Log
