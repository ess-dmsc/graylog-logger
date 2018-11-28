//
//  FileInterfaceTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "graylog_logger/FileInterface.hpp"
#include "graylog_logger/Log.hpp"
#include <ciso646>
#include <fstream>
#include <gtest/gtest.h>
#include <unistd.h>

using namespace Log;

const std::string usedFileName("testFileName.log");

const std::string fileTestString("Some test string");

void deleteFile(std::string Name) {
  auto Error = unlink(Name.c_str());
  if (Error) {
    throw std::runtime_error("Failed to delete file with error code: " +
                             std::to_string(Error));
  }
}

bool fileExists(std::string Name) {
  struct stat FileStats;
  auto Error = stat(Name.c_str(), &FileStats);
  if (Error == -1 and errno == ENOENT) {
    return false;
  }
  return true;
}

std::string FileTestStringCreator(const LogMessage &msg) {
  return fileTestString;
}

class FileInterfaceStandIn : public FileInterface {
public:
  FileInterfaceStandIn(const std::string &fileName) : FileInterface(fileName){};
  ~FileInterfaceStandIn(){};
  using FileInterface::MessageQueue;
};

class FileInterfaceTest : public ::testing::Test {
public:
  static void SetUpTestCase(){

  };

  static void TearDownTestCase(){

  };

  virtual void SetUp() {
    if (fileExists(usedFileName)) {
      deleteFile(usedFileName);
    }
  };

  virtual void TearDown() {
    if (fileExists(usedFileName)) {
      deleteFile(usedFileName);
    }
  };
};

TEST_F(FileInterfaceTest, LogFileCreationTest) {
  { FileInterfaceStandIn flInt(usedFileName); }
  ASSERT_TRUE(fileExists(usedFileName));
}

TEST_F(FileInterfaceTest, FileWriteMsgTest) {
  {
    LogMessage msg;
    FileInterfaceStandIn flInt(usedFileName);
    flInt.setMessageStringCreatorFunction(FileTestStringCreator);
    flInt.addMessage(msg);
  }
  std::ifstream inStream(usedFileName, std::ios::in);
  std::string logLine;
  std::getline(inStream, logLine);
  ASSERT_EQ(logLine, fileTestString);
}

using std::chrono_literals::operator""ms;

TEST_F(FileInterfaceTest, OpenFileMessages) {
  Log::SetMinimumSeverity(Log::Severity::Info);
  testing::internal::CaptureStdout();
  { FileInterfaceStandIn flInt(usedFileName); }
  std::this_thread::sleep_for(100ms);
  auto StdOutputString = testing::internal::GetCapturedStdout();
  EXPECT_NE(StdOutputString.find("Started logging to log file"),
            std::string::npos);
  EXPECT_EQ(StdOutputString.find("Unable to open log file"), std::string::npos);
  testing::internal::CaptureStdout();
  { FileInterfaceStandIn flInt(""); }
  std::this_thread::sleep_for(100ms);
  StdOutputString = testing::internal::GetCapturedStdout();
  EXPECT_EQ(StdOutputString.find("Started logging to log file"),
            std::string::npos);
  EXPECT_NE(StdOutputString.find("Unable to open log file"), std::string::npos)
      << "Actual std-output was: " << StdOutputString;
}
