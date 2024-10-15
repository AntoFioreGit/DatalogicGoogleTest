#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "CommonTest.h"
#include "parameters.h"
class PtrParameterTest : public ::testing::Test
{
protected:
     void SetUp() override {}
     void TearDown() override {}
};

TEST_F(PtrParameterTest, SetCheckPtrParameters)
{

     LOG(INFO) << "PtrParameterTest test SetCheckPtrParameters  begin";
     int value = 8;
     int min = 0;
     int max = 10;
     int step = 1;
     int defaulVal = 5;
     std::string desc = "desc";
     auto pt = std::make_unique<rs::PtrParameter<int>>(min, max, step, defaulVal, &value, desc);
     EXPECT_EQ(defaulVal, pt->getDefault());
     EXPECT_EQ(pt->getDescription(), desc);
     float valueKO = value + max;
     bool result = pt->isValid(valueKO);
     bool expected = false;
     EXPECT_EQ(result, expected);
     result = pt->set(valueKO);
     expected = false;
     EXPECT_EQ(result, expected);
     result = pt->set(value);
     expected = true;
     EXPECT_EQ(result, expected);
     auto rangeValueobject = pt->getRange();
     EXPECT_EQ(min, pt->getRange().min);
     EXPECT_EQ(max, pt->getRange().max);
     LOG(INFO) << "PtrParameterTest test SetCheckPtrParameters  end";
}
