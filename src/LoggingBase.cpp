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
  std::lock_guard<std::mutex> guard1(vectorMutex);
  minSeverity = Severity::Notice;
  const int stringBufferSize = 100;
  char stringBuffer[stringBufferSize];
  int res;
  res = gethostname(stringBuffer, stringBufferSize);
  std::lock_guard<std::mutex> guard2(baseMsgMutex);
  if (0 == res) {
    baseMsg.Host = std::string(stringBuffer);
  }
  baseMsg.ProcessId = getpid();
  baseMsg.ProcessName = get_process_name();
}

LoggingBase::~LoggingBase() {
  std::lock_guard<std::mutex> guard(vectorMutex);
  handlers.clear();
}

void LoggingBase::Log(const Severity sev, const std::string &message) {
  Log(sev, message, std::vector<std::pair<std::string, AdditionalField>>());
}

void LoggingBase::Log(
    const Severity sev, const std::string &message,
    const std::pair<std::string, AdditionalField> &extraField) {
  Log(sev, message, std::vector<std::pair<std::string, AdditionalField>>{
                        extraField,
                    });
}

void LoggingBase::Log(
    const Severity sev, const std::string &message,
    const std::vector<std::pair<std::string, AdditionalField>> &extraFields) {
  if (int(sev) > int(minSeverity)) {
    return;
  }
  baseMsgMutex.lock();
  LogMessage cMsg(baseMsg);
  baseMsgMutex.unlock();
  for (auto &fld : extraFields) {
    cMsg.AddField(fld.first, fld.second);
  }
  cMsg.Timestamp = std::chrono::system_clock::now();
  cMsg.MessageString = message;
  cMsg.SeverityLevel = sev;
  std::ostringstream ss;
  ss << std::this_thread::get_id();
  cMsg.ThreadId = ss.str();
  std::lock_guard<std::mutex> guard(vectorMutex);
  for (auto &ptr : handlers) {
    ptr->addMessage(cMsg);
  }
}

void LoggingBase::AddLogHandler(const LogHandler_P &handler) {
  std::lock_guard<std::mutex> guard(vectorMutex);
  handlers.push_back(handler);
}

void LoggingBase::RemoveAllHandlers() {
  std::lock_guard<std::mutex> guard(vectorMutex);
  handlers.clear();
}

std::vector<LogHandler_P> LoggingBase::GetHandlers() {
  std::lock_guard<std::mutex> guard(vectorMutex);
  return handlers;
}

void LoggingBase::SetMinSeverity(Severity sev) {
  std::lock_guard<std::mutex> guard(vectorMutex);
  minSeverity = sev;
}

  } // namespace Log
