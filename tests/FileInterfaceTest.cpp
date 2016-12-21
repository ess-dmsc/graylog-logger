//
//  FileInterfaceTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-21.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <gtest/gtest.h>
#include <boost/filesystem.hpp>
#include "FileInterface.hpp"

const std::string usedFileName("testFileName.log");

class FileInterfaceStandIn : public FileInterface {
public:
    FileInterfaceStandIn(std::string fileName) : FileInterface(fileName){};
    ~FileInterfaceStandIn() {};
    using FileInterface::fileName;
    using FileInterface::msgQueue;
    using FileInterface::fileThread;
};

class FileInterfaceTest : public ::testing::Test {
public:
    static void SetUpTestCase() {
        
    };
    
    static void TearDownTestCase() {
        
    };
    
    virtual void SetUp() {
        if (boost::filesystem::exists(usedFileName)) {
            boost::filesystem::remove(usedFileName);
        }
    };
    
    virtual void TearDown() {
        
    };
};

TEST_F(FileInterfaceTest, LogFileCreationTest) {
    {
        FileInterfaceStandIn flInt(usedFileName);
    }
    ASSERT_TRUE(boost::filesystem::exists(usedFileName));
}
