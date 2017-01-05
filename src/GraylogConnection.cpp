//
//  GraylogConnection.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-29.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include "graylog_logger/GraylogConnection.hpp"
#include <utility>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctime>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include <vector>

GraylogConnection::GraylogConnection(std::string host, int port, int queueLength) : closeThread(false), host(host), port(std::to_string(port)), queueLength(queueLength), socketFd(-1), conAddresses(NULL), connectionTries(0), firstMessage(true) {
    retConState = GraylogConnection::ConStatus::NONE;
    connectionThread = std::thread(&GraylogConnection::ThreadFunction, this);
}

GraylogConnection::~GraylogConnection() {
    EndThread();
}

GraylogConnection::ConStatus GraylogConnection::GetConnectionStatus() {
    GraylogConnection::ConStatus tempState = GraylogConnection::ConStatus::NONE;
    while (not stateQueue.empty()) {
        stateQueue.try_pop(tempState);
    }
    if (GraylogConnection::ConStatus::NONE != tempState) {
        std::lock_guard<std::mutex> lock(stateMutex);
        retConState = tempState;
    }
    return retConState;
}

void GraylogConnection::EndThread() {
    closeThread = true;
    connectionThread.join();
    if (NULL != conAddresses) {
        freeaddrinfo(conAddresses);
        conAddresses = NULL;
    }
}

void GraylogConnection::MakeConnectionHints() {
    bzero((void*)&hints, sizeof(hints));
    //hints.ai_family = AF_UNSPEC; //Accept both IPv4 and IPv6
    hints.ai_family = AF_INET; //Accept only IPv4
    hints.ai_socktype = SOCK_STREAM; //Use TCP
}

void GraylogConnection::GetServerAddr() {
    if (NULL != conAddresses) {
        freeaddrinfo(conAddresses);
        conAddresses = NULL;
    }
    int res = getaddrinfo(host.c_str(), port.c_str(), &hints, &conAddresses);
    if (-1 == res or NULL == conAddresses) {
        //std::cout << "GL: Failed to get addr.-info." << std::endl;
        SetState(ConStatus::ADDR_RETRY_WAIT);
        endWait = time(NULL) + retryDelay;
    } else {
        //std::cout << "GL: Changing state to CONNECT." << std::endl;
        SetState(ConStatus::CONNECT);
    }
}
void GraylogConnection::ConnectToServer() {
    addrinfo *p;
    int response;
    for (p = conAddresses; p != NULL; p = p->ai_next) {
        //std::cout << "GL: Connect addr. iteration." << std::endl;
        socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (-1 == socketFd) {
            continue;
        }
        int value = 1;
#ifdef SO_NOSIGPIPE
        setsockopt(socketFd, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value));
#endif
        //setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, const void *, socklen_t);
        response = fcntl(socketFd, F_SETFL, O_NONBLOCK);
        if (-1 == response) {
            close(socketFd);
            socketFd = -1;
            continue;
        }
        response = connect(socketFd, p->ai_addr, p->ai_addrlen);
        if (-1 == response and EINPROGRESS == errno) {
            //std::cout << "GL: Now waiting for connection." << std::endl;
            SetState(ConStatus::CONNECT_WAIT);
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
        SetState(ConStatus::CONNECT_RETRY_WAIT);
        if (5 < connectionTries) {
            SetState(ConStatus::ADDR_RETRY_WAIT);
            connectionTries = 0;
        }
    }
    endWait = time(NULL) + retryDelay;
}

void GraylogConnection::ConnectWait() {
    if (endWait < time(NULL)) {
        //std::cout << "GL: Timeout on waiting for connection." << std::endl;
        SetState(ConStatus::CONNECT);
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
        
        SetState(ConStatus::NEW_MESSAGE);
    } else if (0 == changes) {
        //timeout
    } else if (-1 == changes) {
        SetState(ConStatus::CONNECT);
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
        bytesSent = 0;
        //std::cout << "GL: Got message to send." << std::endl;
        SetState(ConStatus::SEND_LOOP);
    }
}

void GraylogConnection::SendMessage(std::string msg) {
    logMessages.push(msg);
}

void GraylogConnection::CheckConnectionStatus() {
    timeval selectTimeout;
    selectTimeout.tv_sec = 0;
    selectTimeout.tv_usec = 0;
//    fd_set exceptfds;
//    FD_ZERO(&exceptfds);
//    FD_SET(socketFd, &exceptfds);
//    fd_set readfds;
//    FD_ZERO(&readfds);
//    FD_SET(socketFd, &readfds);
//    fd_set writefds;
//    FD_ZERO(&writefds);
//    FD_SET(socketFd, &writefds);
//    int selRes = select(socketFd + 1, &readfds, NULL, &exceptfds, &selectTimeout);
//    //std::cout << "Connection check: " << selRes << std::endl;
//    if(selRes and FD_ISSET(socketFd, &exceptfds)) {
//        SetState(ConStatus::CONNECT);
//        shutdown(socketFd, SHUT_RDWR);
//        close(socketFd);
//        return;
//    }
//    char b;
//    if (FD_ISSET(socketFd, &readfds)) {
//        ssize_t peekRes = recv(socketFd, &b, 1, MSG_PEEK);
//        //std::cout << "Peek check: " << peekRes << std::endl;
//        if (-1 == peekRes) {
//            //perror("peek");
//            SetState(ConStatus::CONNECT);
//            shutdown(socketFd, SHUT_RDWR);
//            close(socketFd);
//            return;
//        }
//    }
    pollfd ufds[1];
    ufds[0].fd = socketFd;
#ifdef POLLRDHUP
    ufds[0].events = POLLIN | POLLOUT | POLLRDHUP;
#else
    ufds[0].events = POLLIN | POLLOUT;
#endif
    int pollRes = poll(ufds, 1, 10);
    //std::cout << "Number of poll results: " << pollRes << std::endl;
    if (pollRes == 1) {
        //std::cout << "Poll result: " << ufds[0].revents << std::endl;
        if (ufds[0].revents & POLLERR) {
            SetState(ConStatus::CONNECT);
            shutdown(socketFd, SHUT_RDWR);
            close(socketFd);
            //std::cout << "Got poll error." << std::endl;
#ifdef POLLRDHUP
        } else if (ufds[0].revents & POLLHUP or ufds[0].revents & POLLRDHUP) {
#else
        } else if (ufds[0].revents & POLLHUP) {
#endif
            //std::cout << "Got poll hup." << std::endl;
            SetState(ConStatus::CONNECT);
            shutdown(socketFd, SHUT_RDWR);
            close(socketFd);
        } else if (ufds[0].revents & POLLNVAL) {
            //std::cout << "Got poll nval." << std::endl;
        }
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
            SetState(ConStatus::CONNECT);
            shutdown(socketFd, SHUT_RDWR);
            close(socketFd);
            return;
        }
        if (FD_ISSET(socketFd, &writefds)) {
            //std::cout << "GL: Ready to write." << std::endl;
            {
                ssize_t cBytes = send(socketFd, currentMessage.substr(bytesSent, currentMessage.size() - bytesSent).c_str(), currentMessage.size() - bytesSent + 1, sendOpt);
                //std::cout << "GL: Sent bytes: " << cBytes << std::endl;
                if (-1 == cBytes) {
                    if (EAGAIN == errno or EWOULDBLOCK == errno) {
                        //std::cout << "GL: Non blocking, got errno:" << errno << std::endl;
                        //Should probably handle this
                    } else {
                        //std::cout << "GL: Send error with errno:" << errno << std::endl;
                        //perror("send:");
                        SetState(ConStatus::CONNECT);
                        shutdown(socketFd, SHUT_RDWR);
                        close(socketFd);
                        return;
                    }
                } else if (0 == cBytes) {
                    //std::cout << "No bytes sent." << std::endl;
                    //Do nothing
                } else {
                    bytesSent += cBytes;
                    if (bytesSent == currentMessage.size() + 1) {
                        assert(logMessages.try_pop());
                        SetState(ConStatus::NEW_MESSAGE);
                    }
                }
            }
        }
    } else if (0 == selRes) {
        //std::cout << "Got send timeout." << std::endl;
        //timeout
    } else if (-1 == selRes) {
        //error
    } else {
        assert(false); //Should never be reached
    }
}

void GraylogConnection::ThreadFunction() {
    auto sleepTime = std::chrono::milliseconds(50);
    SetState(ConStatus::ADDR_LOOKUP);
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
                    SetState(ConStatus::ADDR_LOOKUP);
                } else {
                    std::this_thread::sleep_for(sleepTime);
                }
                break;
            case ConStatus::CONNECT:
                ConnectToServer();
                break;
            case ConStatus::CONNECT_RETRY_WAIT:
                if (time(NULL) > endWait) {
                    SetState(ConStatus::CONNECT);
                } else {
                    std::this_thread::sleep_for(sleepTime);
                }
                break;
            case ConStatus::CONNECT_WAIT:
                ConnectWait();
                break;
            case ConStatus::NEW_MESSAGE:
                NewMessage();
                CheckConnectionStatus();
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

bool GraylogConnection::MessagesQueued() {
    return logMessages.size() > 0;
}

void GraylogConnection::SetState(GraylogConnection::ConStatus newState) {
    static std::vector<std::string> stateToStr = {"NONE", "ADDR_LOOKUP", "ADDR_RETRY_WAIT", "CONNECT", "CONNECT_WAIT", "CONNECT_RETRY_WAIT", "SEND_LOOP", "NEW_MESSAGE"};
    std::cout << "Switched state to: " << stateToStr.at(int(newState)) << std::endl;
    stateMachine = newState;
    stateQueue.push(newState);
}
