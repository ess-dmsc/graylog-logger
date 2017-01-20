//
//  GraylogConnection.hpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-29.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <netdb.h>
#include <mutex>
#include "graylog_logger/ConcurrentQueue.hpp"

class GraylogConnection {
public:
    GraylogConnection(std::string host, int port);
    virtual ~GraylogConnection();
    virtual void SendMessage(std::string msg);
    enum class ConStatus {NONE, ADDR_LOOKUP, ADDR_RETRY_WAIT, CONNECT, CONNECT_WAIT, CONNECT_RETRY_WAIT, SEND_LOOP, NEW_MESSAGE};
    ConStatus GetConnectionStatus();
protected:
    
    const time_t retryDelay = 10.0;
    time_t endWait;
    int connectionTries;
    
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
    void SetState(ConStatus newState);
    
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
    struct sockaddr_in serverConInfo;//
    ConcurrentQueue<std::string> logMessages;
#ifdef MSG_NOSIGNAL
    const int sendOpt = MSG_NOSIGNAL;
#else
    const int sendOpt = 0;
#endif
private:
    mutable std::mutex stateMutex;
    ConStatus retConState;
    ConcurrentQueue<ConStatus> stateQueue;
};
