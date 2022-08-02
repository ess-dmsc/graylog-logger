/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Example application making use of the graylog-logger library.
///
//===----------------------------------------------------------------------===//

#include <chrono>
#include <ciso646>
#include <cmath>
#include <graylog_logger/ConsoleInterface.hpp>
#include <graylog_logger/FileInterface.hpp>
#include <graylog_logger/GraylogInterface.hpp>
#include <graylog_logger/Log.hpp>
#include <iostream>
#include <string>

#ifdef _WIN32
#include "getopt.h"
#else
#include <getopt.h>
#endif

void PrintAlternatives();

int main(int argc, char **argv) {
  using namespace Log;
  std::string fileName("messages.log");
  std::string address1("localhost");
  std::string msg;
  int sevLevel = 7;
  unsigned short port = 12201;
  float timeout = 1.0;
  std::string extraKey;
  AdditionalField extraField;
  static struct option long_options[] {
    {"help", no_argument, nullptr, 'h'},
        {"file", optional_argument, nullptr, 'f'},
        {"address", optional_argument, nullptr, 'a'},
        {"port", required_argument, nullptr, 'p'},
        {"time", required_argument, nullptr, 't'},
        {"level", required_argument, nullptr, 'l'},
        {"message", required_argument, nullptr, 'm'},
        {"extra", optional_argument, nullptr, 'e'},
  };
  int option_index = 0;
  while (true) {
    int c = getopt_long(argc, argv, "hf::p:t:l:m:a::e:", long_options,
                        &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
    case 'h':
      PrintAlternatives();
      return 0;
      break;
    case 'f':
      if (optarg != nullptr) {
        fileName = std::string(optarg);
      } else {
        fileName = "";
      }
      break;
    case 'a':
      if (optarg != nullptr) {
        address1 = std::string(optarg);
      } else {
        address1 = "";
      }
      break;
    case 'p':
      if (optarg != nullptr) {
        std::string value(optarg);
        try {
          port = std::stoi(value);
        } catch (std::invalid_argument &e) {
          std::cout << "Unable to convert port to int.\n";
          PrintAlternatives();
          return 0;
        }
      }
      break;
    case 't':
      if (optarg != nullptr) {
        std::string value(optarg);
        try {
          timeout = std::stof(value);
        } catch (std::invalid_argument &e) {
          std::cout << "Unable to convert time to float.\n";
          PrintAlternatives();
          return 0;
        }
      }
      break;
    case 'm':
      if (optarg != nullptr) {
        msg = std::string(optarg);
      }
      break;
    case 'l':
      if (optarg != nullptr) {
        std::string value(optarg);
        try {
          sevLevel = std::stoi(value);
        } catch (std::invalid_argument &e) {
          std::cout << "Unable to convert level to int.\n";
          PrintAlternatives();
          return 0;
        }
        if (sevLevel < 0 or sevLevel > 7) {
          std::cout << "Level is not a value between 0 and 7.\n";
          PrintAlternatives();
          return 0;
        }
      }
      break;
    case 'e':
      if (optarg != nullptr) {
        std::string value(optarg);
        size_t splitLoc = value.find(':');
        if (std::string::npos == splitLoc) {
          break;
        }
        extraKey = value.substr(0, splitLoc);
        extraField = value.substr(splitLoc + 1, value.size() - 1);
        if (extraKey.empty() or extraField.strVal.empty()) {
          extraKey = "";
          std::cout << "Unable to parse extra field: \"" << value << "\"\n";
        }
      }
      break;
    }
  }
  if (msg.empty()) {
    PrintAlternatives();
    return 0;
  }
  std::string commandLineArguments;
  for (int i = 1; i < argc; i++) {
    commandLineArguments += " " + std::string(argv[i]);
  }
  Log::AddField("arguments", commandLineArguments);
  Log::SetMinimumSeverity(Severity::Debug);
  Log::RemoveAllHandlers();
  Log::AddLogHandler(std::make_shared<ConsoleInterface>());

  if (not fileName.empty()) {
    Log::AddLogHandler(std::make_shared<FileInterface>(fileName));
  }

  if (not address1.empty()) {
    Log::AddLogHandler(std::make_shared<GraylogInterface>(address1, port));
  }
  if (not extraKey.empty()) {
    Log::Msg(Severity(sevLevel), msg, {extraKey, extraField});
  } else {
    Log::Msg(Severity(sevLevel), msg);
  }

  Log::FmtMsg(Severity::Info,
              "A formatted string containing an int ({}), a float ({}) and "
              "the string \"{}\".",
              42, 3.14, "hello");

  if (not Log::Flush(
          std::chrono::milliseconds(static_cast<int>(timeout * 1000)))) {
    std::cout
        << "Reached timeout when trying to send message to Graylog-server.\n";
  }

  return 0;
}

void PrintAlternatives() {
  std::cout << "\nusage: console_logger [-h] [-f<file_name>] [-a<address>] "
               "[-p<port>]\n";
  std::cout << "                      [-t<timeout in s>] [-l <level>] "
               "[-m<message>]\n";
  std::cout << "                      [-e<key>:<value>]\n\n";
  std::cout << "This application will write the log message to file and socket "
               "by default.\n";
  std::cout << "To prevent the application from doing this, use the -f and -a "
               "flags but do not\n";
  std::cout << "provide a file name or address. The level paramater is a value "
               "between 0  and 8\n";
  std::cout << "with 0 being \"Emergency\" and 8 indicating a trace message. "
               "The default level\n";
  std::cout << "is 7 (debug). The default file name is \"messages.log\". The "
               "default address\n";
  std::cout << "is \"localhost\" and the default port is 12201. The extra "
               "field parameter requires\n";
  std::cout << "that the key and value of the field is separated using the "
               "colon character.\n\n";
  std::cout << "Example: ./console_logger -t2.0 -m\"This is a log message.\"\n";
}
