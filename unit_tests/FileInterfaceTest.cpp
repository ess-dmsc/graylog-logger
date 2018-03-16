//
//  FileInterfaceTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "graylog_logger/FileInterface.hpp"
#include <boost/filesystem.hpp>
#include <ciso646>
#include <fstream>
#include <gtest/gtest.h>

const std::string usedFileName("testFileName.log");

const std::string fileTestString("Some test string");

std::string FileTestStringCreator(const LogMessage &msg) {
  return fileTestString;
}

class FileInterfaceStandIn : public FileInterface {
public:
  FileInterfaceStandIn(const std::string &fileName) : FileInterface(fileName){};
  ~FileInterfaceStandIn(){};
  using FileInterface::fName;
  using FileInterface::fileThread;
  using FileInterface::logMessages;
};

class FileInterfaceTest : public ::testing::Test {
public:
  static void SetUpTestCase(){

  };

  static void TearDownTestCase(){

  };

  virtual void SetUp() {
    if (boost::filesystem::exists(usedFileName)) {
      boost::filesystem::remove(usedFileName);
    }
  };

  virtual void TearDown() {
    if (boost::filesystem::exists(usedFileName)) {
      boost::filesystem::remove(usedFileName);
    }
  };
};

TEST_F(FileInterfaceTest, LogFileCreationTest) {
  { FileInterfaceStandIn flInt(usedFileName); }
  ASSERT_TRUE(boost::filesystem::exists(usedFileName));
}

TEST_F(FileInterfaceTest, FileNameSetVarTest) {
  FileInterfaceStandIn flInt(usedFileName);
  ASSERT_EQ(flInt.fName, usedFileName);
}

TEST_F(FileInterfaceTest, FileWriteMsgTest) {
  {
    LogMessage msg;
    FileInterfaceStandIn flInt(usedFileName);
    flInt.SetMessageStringCreatorFunction(FileTestStringCreator);
    flInt.AddMessage(msg);
  }
  std::ifstream inStream(usedFileName, std::ios::in);
  std::string logLine;
  std::getline(inStream, logLine);
  ASSERT_EQ(logLine, fileTestString);
}
