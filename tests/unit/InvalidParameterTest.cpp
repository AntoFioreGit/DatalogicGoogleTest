#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "CommonTest.h"
#include "parameters.h"
class InvalidParameterTest : public ::testing::Test
{
protected:
     void SetUp() override {}
     void TearDown() override {}

     rs::InvalidParameter _ip;
};

TEST_F(InvalidParameterTest, checkInvalidrParameters)
{

     LOG(INFO) << "InvalidParameterTest test checkIntrinsicsOk  begin";
     float val = 3;
     bool result = _ip.set(val);
     bool expected = false;
     EXPECT_EQ(result, expected);
     std::string desc = "Invalid option";
     EXPECT_EQ(_ip.getDescription(), desc);
     LOG(INFO) << "InvalidParameterTest test checkIntrinsicsOk  end";
}
