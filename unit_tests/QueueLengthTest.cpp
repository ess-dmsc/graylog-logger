//
//  QueueLengthTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2017-01-20.
//  Copyright © 2017 European Spallation Source. All rights reserved.
//

#include <ciso646>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "graylog_logger/GraylogInterface.hpp"
#include "graylog_logger/ConsoleInterface.hpp"
#include "graylog_logger/FileInterface.hpp"

class ConsoleInterfaceStandIn : public ConsoleInterface {
public:
    ConsoleInterfaceStandIn(int queueSize) : ConsoleInterface(queueSize) {};
    using ConsoleInterface::ExitThread;
};

class FileInterfaceStandIn : public FileInterface {
public:
    FileInterfaceStandIn(int queueSize) : FileInterface("messages.log", queueSize) {};
    using FileInterface::ExitThread;
};

class QueueLength : public ::testing::Test{
public:
    static void SetUpTestCase() {
    };
    
    static void TearDownTestCase() {
    };
    
    virtual void SetUp() {
    };
    
    virtual void TearDown() {
    };
};


LogMessage GetLogMsg() {
    LogMessage retMsg;
    retMsg.host = "Some host";
    retMsg.message = "This is some multi line\n error message with \"quotes\".";
    retMsg.processId = 667;
    retMsg.processName = "some_process_name";
    retMsg.severity = Severity::Alert;
    retMsg.threadId = "0xff0011aacc";
    retMsg.timestamp = std::chrono::system_clock::now();
    return retMsg;
}

void TestFunc(BaseLogHandler *basePtr, int testLimit) {
    LogMessage usedMsg = GetLogMsg();
    for (int i = 0; i < testLimit; i++) {
        basePtr->AddMessage(usedMsg);
        ASSERT_EQ(i + 1, basePtr->QueueSize());
    }
    for (int u = 0; u < 10; u++) {
        basePtr->AddMessage(usedMsg);
        ASSERT_EQ(testLimit, basePtr->QueueSize());
    }
}

TEST_F(QueueLength, ConsoleInterfaceTest) {
    int queueLength = 50;
    auto console = new ConsoleInterfaceStandIn(queueLength);
    console->ExitThread();
    TestFunc((BaseLogHandler*)console, queueLength);
    delete console;
}

TEST_F(QueueLength, FileInterfaceTest) {
    int queueLength = 50;
    auto file = new FileInterfaceStandIn(queueLength);
    file->ExitThread();
    TestFunc((BaseLogHandler*)file, queueLength);
    delete file;
}

TEST_F(QueueLength, GraylogInterfaceTest) {
    int queueLength = 50;
    auto file = new GraylogInterface("some_addr", 22222, queueLength);
    TestFunc((BaseLogHandler*)file, queueLength);
    delete file;
}
