//
//  main.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2016-12-15.
//  Copyright © 2016 European Spallation Source. All rights reserved.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv) {
    //::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
