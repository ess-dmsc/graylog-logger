//
//  LogMessageTest.cpp
//  dm-graylog-logger
//
//  Created by Jonas Nilsson on 2017-01-20.
//  Copyright © 2017 European Spallation Source. All rights reserved.
//

#include "graylog_logger/LogUtil.hpp"
#include <ciso646>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace Log;

class LogMessageTesting : public ::testing::Test {
public:
  static void SetUpTestCase() {};

  static void TearDownTestCase() {};

  virtual void SetUp() {};

  virtual void TearDown() {};
};

TEST_F(LogMessageTesting, AddDoubleExtraField) {
  LogMessage testMsg;
  std::string someKey = "my_key";
  double someValue = 3.43234;
  testMsg.addField(someKey, someValue);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).first, someKey);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).second.dblVal, someValue);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).second.FieldType,
            AdditionalField::Type::typeDbl);
  ASSERT_EQ(testMsg.AdditionalFields.size(), 1);
}

TEST_F(LogMessageTesting, AddStringExtraField) {
  LogMessage testMsg;
  std::string someKey = "my_key";
  std::string someValue = "some_random_value_string";
  testMsg.addField(someKey, someValue);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).first, someKey);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).second.strVal, someValue);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).second.FieldType,
            AdditionalField::Type::typeStr);
  ASSERT_EQ(testMsg.AdditionalFields.size(), 1);
}

TEST_F(LogMessageTesting, AddIntExtraField) {
  LogMessage testMsg;
  std::string someKey = "my_key";
  std::int64_t someValue = 9124432895;
  testMsg.addField(someKey, someValue);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).first, someKey);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).second.intVal, someValue);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).second.FieldType,
            AdditionalField::Type::typeInt);
  ASSERT_EQ(testMsg.AdditionalFields.size(), 1);
}

TEST_F(LogMessageTesting, AddTwoKeys1) {
  LogMessage testMsg;
  std::string someKey = "my_key";
  std::int64_t someValue = 9124432895;
  testMsg.addField(someKey, someValue);
  testMsg.addField(someKey, someValue);
  ASSERT_EQ(testMsg.AdditionalFields.size(), 1);
}

TEST_F(LogMessageTesting, AddTwoKeys2) {
  LogMessage testMsg;
  std::string someKey = "my_key";
  std::int64_t someValue1 = 9124432895;
  std::string someValue2 = "912";
  testMsg.addField(someKey, someValue1);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).second.FieldType,
            AdditionalField::Type::typeInt);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).second.intVal, someValue1);
  testMsg.addField(someKey, someValue2);
  ASSERT_EQ(testMsg.AdditionalFields.size(), 1);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).second.FieldType,
            AdditionalField::Type::typeStr);
  ASSERT_EQ(testMsg.AdditionalFields.at(0).second.strVal, someValue2);
}
