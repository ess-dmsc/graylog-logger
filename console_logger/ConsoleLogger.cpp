//
//  main.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <chrono>
#include <iostream>
#include <string>
#include <getopt.h>
#include <graylog_logger/Log.hpp>
#include <graylog_logger/GraylogInterface.hpp>
#include <graylog_logger/FileInterface.hpp>
#include <graylog_logger/ConsoleInterface.hpp>

void PrintAlternatives();

int main(int argc, char **argv) {
    std::string fileName("messages.log");
    std::string address("localhost");
    std::string msg;
    int sevLevel = 7;
    unsigned short port = 12201;
    float timeout = 1.0;
    int c;
    static struct option long_options[] {
        {"help", no_argument, 0, 'h'},
        {"file", optional_argument, 0, 'f'},
        {"address", optional_argument, 0, 'a'},
        {"port", required_argument, 0, 'p'},
        {"time", required_argument, 0, 't'},
        {"level", required_argument, 0, 'l'},
        {"message", required_argument, 0, 'm'},
    };
    int option_index = 0;
    while (true) {
        c = getopt_long(argc, argv, "hf::p:t:l:m:a::", long_options, &option_index);
        if (c == -1) {
            break;
        }
        switch (c) {
            case 'h':
                PrintAlternatives();
                return 0;
                break;
            case 'f':
                if (optarg) {
                    fileName = std::string(optarg);
                } else {
                    fileName = "";
                }
                break;
            case 'a':
                if (optarg) {
                    address = std::string(optarg);
                } else {
                    address = "";
                }
                break;
            case 'p':
                if (optarg) {
                    std::string value(optarg);
                    try {
                        port = std::stoi(value);
                    } catch (std::invalid_argument &e) {
                        std::cout << "Unable to convert port to int." << std::endl;
                        PrintAlternatives();
                        return 0;
                    }
                }
                break;
            case 't':
                if (optarg) {
                    std::string value(optarg);
                    try {
                        timeout = std::stof(value);
                    } catch (std::invalid_argument &e) {
                        std::cout << "Unable to convert time to float." << std::endl;
                        PrintAlternatives();
                        return 0;
                    }
                }
                break;
            case 'm':
                if (optarg) {
                    msg = std::string(optarg);
                }
                break;
            case 'l':
                if (optarg) {
                    std::string value(optarg);
                    try {
                        sevLevel = std::stoi(value);
                    } catch (std::invalid_argument &e) {
                        std::cout << "Unable to convert level to int." << std::endl;
                        PrintAlternatives();
                        return 0;
                    }
                    if (sevLevel < 0 or sevLevel > 7) {
                        std::cout << "Level is not a value between 0 and 7." << std::endl;
                        PrintAlternatives();
                        return 0;
                    }
                }
        }
    }
    if (0 == msg.size()) {
        PrintAlternatives();
        return 0;
    }
    Log::SetMinimumSeverity(Severity::Debug);
    Log::RemoveAllHandlers();
    Log::AddLogHandler(LogHandler_P(new ConsoleInterface()));
    
    if (fileName.size() > 0) {
        Log::AddLogHandler(LogHandler_P(new FileInterface(fileName)));
    }
    
    if (address.size() > 0) {
        Log::AddLogHandler(LogHandler_P(new GraylogInterface(address, port)));
    }
    
    Log::Msg(Severity(sevLevel), msg);
    
    std::vector<LogHandler_P> allInterfaces = Log::GetHandlers();
    std::vector<LogHandler_P> graylogInt;
    
    for(auto ptr : allInterfaces) {
        if (dynamic_cast<GraylogInterface*>(ptr.get()) != nullptr) {
            graylogInt.push_back(ptr);
        }
    }
    if (graylogInt.size() > 0) {
        int milisSleep = 20;
        int waitLoops = int((timeout * 1000) / milisSleep + 0.5);
        auto sleepTime = std::chrono::milliseconds(milisSleep);
        bool continueLoop = true;
        for (int i = 0; i < waitLoops and continueLoop; i++) {
            std::this_thread::sleep_for(sleepTime);
            continueLoop = false;
            for(auto ptr : graylogInt){
                if (dynamic_cast<GraylogInterface*>(ptr.get())->MessagesQueued()) {
                    continueLoop = true;
                }
            }
        }
    }
    
    return 0;
}


void PrintAlternatives() {
    std::cout << "\nusage: logMsg [-h] [-f<file_name>] [-a<address>] [-p<port>]\n";
    std::cout << "              [-t<timeout in s>] [-l <level>] [-m<message>]\n\n";
    std::cout << "This application will write the log message to file and socket by default.\n";
    std::cout << "To prevent the application from doing this, use the -f and -a flags but do not\n";
    std::cout << "provide a file name or address. The level paramater is a value between 0  and 7\n";
    std::cout << "with 0 being \"Emergency\" and 7 indicating a debug message. The default level";
    std::cout << "is 7 (debug). The default file name is \"messages.log\". The default address\n";
    std::cout << "is \"localhost\" and the default port is 12201.\n\n";
    std::cout << "Example: ./logMsg -t2.0 -m\"This is a log message.\"" << std::endl;
}
