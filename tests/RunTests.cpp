//
//  main.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

//#include <gtest/gtest.h>
//#include "GraylogInterfaceTest.cpp"

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "GraylogInterface.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
