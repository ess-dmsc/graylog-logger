//
//  LogTestServer.hpp
//  BoostAsioTest
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#pragma once

#include <string>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <vector>

using namespace boost::asio;
using namespace boost::system::errc;

typedef std::shared_ptr<ip::tcp::socket> sock_ptr;


//------------------------------------------------------------------------------
//     THIS CLASS IS NOT THREAD SAFE AND MAY CRASH AT ANY MOMENT
//------------------------------------------------------------------------------

class LogTestServer {
public:
    LogTestServer(short port);
    ~LogTestServer();
    std::string GetLatestMessage();
    errc_t GetLastSocketError();
    void CloseAllConnections();
    int GetNrOfConnections();
private:
    io_service service;
    void ThreadFunction();
    std::thread asioThread;
    
    ip::tcp::acceptor acceptor;
    
    void WaitForNewConnection();
    void OnConnectionAccept(const boost::system::error_code &ec, sock_ptr cSock);
    void HandleRead(boost::system::error_code ec, std::size_t bytesReceived, sock_ptr cSock);
    
    void RemoveSocket(sock_ptr cSock);
    
    static const int bufferSize = 100;
    char receiveBuffer[bufferSize];
    
    errc_t socketError;
    int connections;
    
    std::string currentMessage;
    std::string previousMessage;
    
    std::vector<sock_ptr> existingSockets;
};
