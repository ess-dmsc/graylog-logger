//
//  KafkaInterface.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <vector>
#include <utility>
#include <string>
#include <thread>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctime>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <atomic>
#include <iostream>
#include "ConcurrentQueue.hpp"
#include "LogUtil.hpp"

class GraylogConnection {
public:
    GraylogConnection(std::string host, int port, int queueLength = 100);
    ~GraylogConnection();
protected:
    const time_t retryDelay = 10.0;
    time_t endWait;
    int connectionTries;
    enum class ConStatus {NONE, ADDR_LOOKUP, ADDR_RETRY_WAIT, CONNECT, CONNECT_WAIT, CONNECT_RETRY_WAIT, SEND_LOOP, NEW_MESSAGE};
    ConStatus stateMachine;
    
    void EndThread();
    
    void ThreadFunction();
    void MakeConnectionHints();
    void GetServerAddr();
    void ConnectToServer();
    void ConnectWait();
    void NewMessage();
    void SendMessageLoop();
    void CheckConnectionStatus();
    
    ConcurrentQueue<std::string> logMessages;
    int queueLength;
    
    std::atomic_bool closeThread;
    
    std::string currentMessage;
    int bytesSent;
    bool firstMessage;
    
    std::string host;
    std::string port;
    
    std::thread connectionThread;
    int socketFd;       //Socket id (file descriptor)
    addrinfo hints;     //Connection hints
    addrinfo *conAddresses;
    //struct sockaddr_in serverConInfo;//
//#ifdef MSG_NOSIGNAL
//    const int sendOpt = MSG_NOSIGNAL;
//#else
    const int sendOpt = 0;
//#endif
};

class GraylogInterface : public BaseLogHandler, private GraylogConnection {
public:
    GraylogInterface(std::string host, int port, int queueLength = 100);
    ~GraylogInterface();
    virtual void AddMessage(LogMessage &msg);
protected:
    std::string LogMsgToJSON(LogMessage &msg);
};
