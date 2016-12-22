//
//  KafkaInterface.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "GraylogInterface.hpp"


GraylogInterface::GraylogInterface(std::string host, int port, int queueLength) : GraylogConnection(host, port, queueLength){
}

GraylogInterface::~GraylogInterface() {
    
}

void GraylogInterface::AddMessage(LogMessage &msg) {
    GraylogConnection::logMessages.push(LogMsgToJSON(msg));
}

std::string GraylogInterface::LogMsgToJSON(LogMessage &msg) {
    return "{}";
}

GraylogConnection::GraylogConnection(std::string host, int port, int queueLength) : closeThread(false), host(host), port(std::to_string(port)), queueLength(queueLength), socketFd(-1), conAddresses(nullptr), connectionTries(0), firstMessage(true), connectionThread(&GraylogConnection::ThreadFunction, this) {
}

GraylogConnection::~GraylogConnection() {
    EndThread();
}

void GraylogConnection::EndThread() {
    closeThread = true;
    connectionThread.join();
    if (nullptr != conAddresses) {
        freeaddrinfo(conAddresses);
    }
}

void GraylogConnection::MakeConnectionHints() {
    bzero((void*)&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC; //Accept both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM; //Use TCP
}

void GraylogConnection::GetServerAddr() {
    if (nullptr != conAddresses) {
        freeaddrinfo(conAddresses);
    }
    int res = getaddrinfo(host.c_str(), port.c_str(), &hints, &conAddresses);
    if (-1 == res) {
        //std::cout << "GL: Failed to get addr.-info." << std::endl;
        stateMachine = ConStatus::ADDR_RETRY_WAIT;
        endWait = time(NULL) + retryDelay;
    } else {
        //std::cout << "GL: Changing state to CONNECT." << std::endl;
        stateMachine = ConStatus::CONNECT;
    }
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void GraylogConnection::ConnectToServer() {
    addrinfo *p;
    int response;
//    char s[INET6_ADDRSTRLEN];
//    for (p = conAddresses; p != NULL; p = p->ai_next) {
//        sockaddr_in *addr = (sockaddr_in*)p->ai_addr;
//        std::cout << "IP: " << inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s) << std::endl;
//    }
    for (p = conAddresses; p != NULL; p = p->ai_next) {
        //std::cout << "GL: Connect addr. iteration." << std::endl;
        socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (-1 == socketFd) {
            continue;
        }
        int value = 1;
#ifndef MSG_NOSIGNAL
        setsockopt(socketFd, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value));
#endif
        response = fcntl(socketFd, F_SETFL, O_NONBLOCK);
        if (-1 == response) {
            close(socketFd);
            socketFd = -1;
            continue;
        }
        response = connect(socketFd, p->ai_addr, p->ai_addrlen);
        if (-1 == response and EINPROGRESS == errno) {
            //std::cout << "GL: Now waiting for connection." << std::endl;
            stateMachine = ConStatus::CONNECT_WAIT;
            break;
        } else {
            close(socketFd);
            socketFd = -1;
            continue;
        }
    }
    if (-1 == socketFd) {
        //std::cout << "GL: Failed all connections." << std::endl;
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
        //std::cout << "GL: Timeout on waiting for connection." << std::endl;
        stateMachine = ConStatus::CONNECT;
        close(socketFd);
        socketFd = -1;
        connectionTries++;
        return;
    }
    timeval selectTimeout;
    selectTimeout.tv_sec = 0;
    selectTimeout.tv_usec = 50000;
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(socketFd, &writefds);
    int changes = select(socketFd + 1, NULL, &writefds, NULL, &selectTimeout);
    //std::cout << "GL: select changes: " << changes << std::endl;
    if (1 == changes and FD_ISSET(socketFd, &writefds)) {
        //std::cout << "GL: Ready to write." << std::endl;
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
    } else if (1 == changes and not FD_ISSET(socketFd, &writefds)) {
        assert(false); //Should never be reached
    }
}

void GraylogConnection::NewMessage() {
    //std::cout << "GL: Waiting for message." << std::endl;
    if (logMessages.time_out_peek(currentMessage, 50)) {
        if (firstMessage) {
            firstMessage = false;
        } else {
            assert(logMessages.try_pop());
            bytesSent = 0;
        }
        //std::cout << "GL: Got message to send." << std::endl;
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
    //std::cout << "GL: Send message changes: " << selRes << std::endl;
    if (selRes > 0) {
        if (FD_ISSET(socketFd, &exceptfds)) { //Some error
            //std::cout << "GL: Got send msg exception." << std::endl;
            stateMachine = ConStatus::CONNECT;
            shutdown(socketFd, SHUT_RDWR);
            close(socketFd);
            return;
        }
        if (FD_ISSET(socketFd, &writefds)) {
            //std::cout << "GL: Ready to write." << std::endl;
            if (bytesSent == currentMessage.size() + 1) {
                stateMachine = ConStatus::NEW_MESSAGE;
            } else {
                ssize_t cBytes = send(socketFd, currentMessage.substr(bytesSent, currentMessage.size() - bytesSent).c_str(), currentMessage.size() - bytesSent + 1, sendOpt);
                if (-1 == cBytes) {
                    if (EAGAIN == errno or EWOULDBLOCK == errno) {
                        //std::cout << "GL: Non blocking, got errno:" << errno << std::endl;
                        //Should probably handle this
                    } else {
                        //std::cout << "GL: Send error with errno:" << errno << std::endl;
                        //perror("send:");
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
            if (-1 != socketFd) {
                shutdown(socketFd, SHUT_RDWR);
                close(socketFd);
            }
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
