//
//  GraylogInterfaceTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <gtest/gtest.h>
#include <thread>
#include "GraylogInterface.hpp"
#include "LogTestServer.hpp"

class GraylogConnectionStandIn : public GraylogConnection {
public:
    GraylogConnectionStandIn(std::string host, int port, int queueLength = 100) : GraylogConnection(host, port, queueLength) {};
    ~GraylogConnectionStandIn() {};
    using GraylogConnection::logMessages;
    using GraylogConnection::EndThread;
};

const int testPort = 2526;
const std::chrono::milliseconds sleepTime(100);

class GraylogConnectionCom : public ::testing::Test{
public:
    static void SetUpTestCase() {
        logServer = new LogTestServer(testPort);
    };
    
    static void TearDownTestCase() {
        delete logServer;
    };
    
    virtual void SetUp() {
    };
    
    virtual void TearDown() {
        logServer->CloseAllConnections();
        logServer->GetLastSocketError();
        logServer->GetLatestMessage();
        logServer->ClearReceivedBytes();
    };
    
    static LogTestServer *logServer;
};

LogTestServer *GraylogConnectionCom::logServer = NULL;

//TEST_F(GraylogConnectionCom, UnknownHostTest) {
//    GraylogConnectionStandIn con("no_host", testPort);
//    ASSERT_EQ(logServer->GetNrOfConnections(), 0);
//    ASSERT_EQ(logServer->GetLatestMessage().size(), 0);
//    ASSERT_EQ(logServer->GetLastSocketError(), int(errc_t::success));
//}

//TEST_F(GraylogConnectionCom, ConnectionTest) {
//    ASSERT_EQ(0, logServer->GetNrOfConnections());
//    ASSERT_EQ(0, logServer->GetLatestMessage().size());
//    ASSERT_EQ(int(errc_t::success), logServer->GetLastSocketError());
//    {
//        GraylogConnectionStandIn con("localhost", testPort);
//        std::this_thread::sleep_for(sleepTime);
//        ASSERT_EQ(1, logServer->GetNrOfConnections());
//        ASSERT_EQ(logServer->GetLatestMessage().size(), 0);
//        ASSERT_EQ(logServer->GetLastSocketError(), int(errc_t::success));
//    }
//    std::this_thread::sleep_for(sleepTime);
//    ASSERT_EQ(0, logServer->GetNrOfConnections());
//    ASSERT_EQ(0, logServer->GetLatestMessage().size());
//    ASSERT_EQ(int(errc_t::success), logServer->GetLastSocketError());
//}

TEST_F(GraylogConnectionCom, CloseConnectionTest) {
    {
        GraylogConnectionStandIn con("localhost", testPort);
        
        std::this_thread::sleep_for(sleepTime);
        
        ASSERT_EQ(1, logServer->GetNrOfConnections());
        
        logServer->CloseAllConnections();
        
        std::this_thread::sleep_for(sleepTime);
        
//        con.logMessages.push("Hello");
        
        std::this_thread::sleep_for(sleepTime);
        
        ASSERT_EQ(1, logServer->GetNrOfConnections()) << "Failed to reconnect after connection was closed remotely.";
    }
    std::this_thread::sleep_for(sleepTime);
    ASSERT_EQ(0, logServer->GetNrOfConnections());
}
