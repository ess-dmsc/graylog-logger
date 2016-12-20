//
//  KafkaInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "GraylogInterface.hpp"


GraylogInterface::GraylogInterface(std::vector<std::pair<std::string, std::string> > servers, int queueLength) {
}

GraylogInterface::~GraylogInterface() {
    
}

GraylogConnection::GraylogConnection(std::string host, int port, int queueLength) : closeThread(false), host(host), port(std::to_string(port)), queueLength(queueLength), socketFd(-1), conAddresses(NULL), connectionTries(0), firstMessage(true), connectionThread(&GraylogConnection::ThreadFunction, this) {
}

GraylogConnection::~GraylogConnection() {
    closeThread = true;
    connectionThread.join();
    if (NULL != conAddresses) {
        freeaddrinfo(conAddresses);
    }
}

void GraylogConnection::MakeConnectionHints() {
    bzero((void*)&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC; //Accept both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM; //Use TCP
}

void GraylogConnection::GetServerAddr() {
    if (NULL != conAddresses) {
        freeaddrinfo(conAddresses);
    }
    int res = getaddrinfo(host.c_str(), port.c_str(), &hints, &conAddresses);
    if (-1 == res) {
        stateMachine = ConStatus::ADDR_RETRY_WAIT;
        endWait = time(NULL) + retryDelay;
    } else {
        stateMachine = ConStatus::CONNECT;
    }
}

void GraylogConnection::ConnectToServer() {
    addrinfo *p;
    int response;
    for (p = conAddresses; p != NULL; p = p->ai_next) {
        socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (-1 == socketFd) {
            continue;
        }
        response = fcntl(socketFd, F_SETFL, O_NONBLOCK);
        if (-1 == response) {
            close(socketFd);
            socketFd = -1;
            continue;
        }
        response = connect(socketFd, p->ai_addr, p->ai_addrlen);
        if (-1 == response and EINPROGRESS == errno) {
            stateMachine = ConStatus::CONNECT_WAIT;
            break;
        } else {
            close(socketFd);
            socketFd = -1;
            continue;
        }
    }
    if (-1 == socketFd) {
        connectionTries++;
        stateMachine = ConStatus::CONNECT_RETRY_WAIT;
        if (5 < connectionTries) {
            stateMachine = ConStatus::ADDR_RETRY_WAIT;
            connectionTries = 0;
        }
    }
    endWait = time(NULL) + retryDelay;
}

void GraylogConnection::ConnectWait() {
    if (endWait < time(NULL)) {
        stateMachine = ConStatus::CONNECT;
        close(socketFd);
        socketFd = -1;
        connectionTries++;
        return;
    }
    timeval selectTimeout;
    selectTimeout.tv_sec = 0;
    selectTimeout.tv_usec = 50000;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socketFd, &readfds);
    int changes = select(socketFd + 1, &readfds, NULL, NULL, &selectTimeout);
    if (1 == changes and FD_ISSET(socketFd, &readfds)) {
        bytesSent = 0;
        currentMessage = "";
        firstMessage = true;
        
        stateMachine = ConStatus::NEW_MESSAGE;
    } else if (0 == changes) {
        //timeout
    } else if (-1 == changes) {
        stateMachine = ConStatus::CONNECT;
        close(socketFd);
        socketFd = -1;
        connectionTries++;
        return;
    } else if (1 == changes and not FD_ISSET(socketFd, &readfds)) {
        assert(false); //Should never be reached
    }
}

void GraylogConnection::NewMessage() {
    if (logMessages.time_out_peek(currentMessage, 50)) {
        if (firstMessage) {
            firstMessage = false;
        } else {
            assert(logMessages.try_pop());
            bytesSent = 0;
        }
        stateMachine = ConStatus::SEND_LOOP;
    }
}

void GraylogConnection::SendMessageLoop() {
    timeval selectTimeout;
    selectTimeout.tv_sec = 0;
    selectTimeout.tv_usec = 50000;
    
    fd_set exceptfds;
    FD_ZERO(&exceptfds);
    FD_SET(socketFd, &exceptfds);
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(socketFd, &writefds);
    int selRes = select(socketFd + 1, NULL, &writefds, &exceptfds, &selectTimeout);
    if (selRes > 0) {
        if (FD_ISSET(socketFd, &exceptfds)) { //Some error
            stateMachine = ConStatus::CONNECT;
            shutdown(socketFd, SHUT_RDWR);
            close(socketFd);
            return;
        }
        if (FD_ISSET(socketFd, &writefds)) {
            if (bytesSent == currentMessage.size() + 1) {
                stateMachine = ConStatus::NEW_MESSAGE;
            } else {
                int cBytes = send(socketFd, currentMessage.substr(bytesSent, currentMessage.size() - bytesSent).c_str(), currentMessage.size() - bytesSent + 1, 0);
                if (-1 == cBytes) {
                    if (EAGAIN == errno or EWOULDBLOCK == errno) {
                        //Should probably handle this
                    } else {
                        stateMachine = ConStatus::CONNECT;
                        shutdown(socketFd, SHUT_RDWR);
                        close(socketFd);
                        return;
                    }
                } else if (0 == cBytes) {
                    //Do nothing
                } else {
                    bytesSent += cBytes;
                }
            }
        }
    } else if (0 == selRes) {
        //timeout
    } else if (-1 == selRes) {
        //error
    } else {
        assert(false); //Should never be reached
    }
}

void GraylogConnection::ThreadFunction() {
    auto sleepTime = std::chrono::milliseconds(50);
    stateMachine = ConStatus::ADDR_LOOKUP;
    bytesSent = 0;
    MakeConnectionHints();
    while (true) {
        if (closeThread) {
            break;
        }
        switch (stateMachine) {
            case ConStatus::ADDR_LOOKUP:
                GetServerAddr();
                break;
            case ConStatus::ADDR_RETRY_WAIT:
                if (time(NULL) > endWait) {
                    stateMachine = ConStatus::ADDR_LOOKUP;
                } else {
                    std::this_thread::sleep_for(sleepTime);
                }
                break;
            case ConStatus::CONNECT:
                ConnectToServer();
                break;
            case ConStatus::CONNECT_RETRY_WAIT:
                if (time(NULL) > endWait) {
                    stateMachine = ConStatus::CONNECT;
                } else {
                    std::this_thread::sleep_for(sleepTime);
                }
                break;
            case ConStatus::CONNECT_WAIT:
                ConnectWait();
                break;
            case ConStatus::NEW_MESSAGE:
                NewMessage();
                break;
            case ConStatus::SEND_LOOP:
                SendMessageLoop();
                break;
            default:
                assert(false); //This should never be reached
                break;
        }
    }
}
