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
#include <sys/types.h>
#include <thread>

#ifdef _WIN32
#include <WinSock2.h>
#include <codecvt>
#include <locale>
#include <process.h>
// clang-format off
#include <Windows.h>
// clang-format on
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

  int lastSlash = buf.rfind(L'\"');
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
  Executor.SendWork([=](){
    const int stringBufferSize = 100;
    char stringBuffer[stringBufferSize];
    int res;
    res = gethostname(static_cast<char *>(stringBuffer), stringBufferSize);
    if (0 == res) {
      BaseMsg.Host = std::string(static_cast<char *>(stringBuffer));
    }
    BaseMsg.ProcessId = getpid();
    BaseMsg.ProcessName = get_process_name();
    // We want to wait for this to finnish, make it so
  });
}

LoggingBase::~LoggingBase() {
  removeAllHandlers();
  // Flushing here?
}

void LoggingBase::addLogHandler(const LogHandler_P Handler) {
  Executor.SendWork([=]() {
    Handlers.push_back(Handler);
  });
}

void LoggingBase::removeAllHandlers() {
  Executor.SendWork([=]() {
    Handlers.clear();
  });
}

std::vector<LogHandler_P> LoggingBase::getHandlers() {
  return Handlers;
}

void LoggingBase::setMinSeverity(Severity Level) {
  Executor.SendWork([=]() {
    MinSeverity = Level;
  });
}

} // namespace Log
