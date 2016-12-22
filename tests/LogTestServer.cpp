//
//  LogTestServer.cpp
//  BoostAsioTest
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "LogTestServer.hpp"

LogTestServer::LogTestServer(short port) : service(), acceptor(service, ip::tcp::endpoint(ip::tcp::v6(), port)){
    socketError = errc_t::success;
    connections = 0;
    WaitForNewConnection();
    asioThread = std::thread(&LogTestServer::ThreadFunction, this);
}

void LogTestServer::WaitForNewConnection() {
    sock_ptr cSock(new ip::tcp::socket(service));
    acceptor.async_accept( *cSock.get(), boost::bind( &LogTestServer::OnConnectionAccept, this, placeholders::error, cSock) );
}

LogTestServer::~LogTestServer() {
    CloseAllConnections();
    acceptor.close();
    asioThread.join();
}

void LogTestServer::CloseAllConnections() {
    std::for_each(existingSockets.begin(), existingSockets.end(), [this](sock_ptr sock){sock->close();});
    existingSockets.clear();
}

std::string LogTestServer::GetLatestMessage() {
    std::string tempStr = previousMessage;
    previousMessage = "";
    return tempStr;
}

void LogTestServer::ThreadFunction() {
    service.run();
}

void LogTestServer::OnConnectionAccept(const boost::system::error_code &ec, sock_ptr cSock) {
    if (errc_t::operation_canceled == ec or errc_t::bad_file_descriptor == ec) {
        return;
    } else if (ec) {
        //std::cout << "OnConnectionAccept(): " << ec.message() << std::endl;
        return;
    }
    //std::cout << "Accepting connection." << std::endl;
    existingSockets.push_back(cSock);
    connections++;
    cSock->async_read_some(buffer(receiveBuffer, bufferSize), boost::bind(&LogTestServer::HandleRead, this, placeholders::error, placeholders::bytes_transferred, cSock));
}

void LogTestServer::HandleRead(boost::system::error_code ec, std::size_t bytesReceived, sock_ptr cSock) {
    if (errc_t::operation_canceled == ec) {
        RemoveSocket(cSock);
        connections--;
        WaitForNewConnection();
        return;
    } else if (ec) {
        //std::cout << "HandleRead(): " << ec.message() << std::endl;
        RemoveSocket(cSock);
        connections--;
        WaitForNewConnection();
        return;
    }
    for (int j = 0; j < bytesReceived; j++) {
        if ('\0' == receiveBuffer[j]) {
            previousMessage = currentMessage;
            currentMessage = "";
        } else {
            currentMessage += receiveBuffer[j];
        }
    }
    cSock->async_read_some(buffer(receiveBuffer, bufferSize), boost::bind(&LogTestServer::HandleRead, this, placeholders::error, placeholders::bytes_transferred, cSock));
}

void LogTestServer::RemoveSocket(sock_ptr cSock) {
    for (int i = 0; i < existingSockets.size(); i++) {
        if (existingSockets[i] == cSock) {
            existingSockets.erase(existingSockets.begin() + i);
            return;
        }
    }
}

errc_t LogTestServer::GetLastSocketError() {
    errc_t tempError = socketError;
    socketError = errc_t::success;
    return tempError;
}

int LogTestServer::GetNrOfConnections() {
    return connections;
}
