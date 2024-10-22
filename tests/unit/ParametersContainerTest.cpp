#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "CommonTest.h"
#include "camera_parameters.h"
#include "parameters.h"

class ParametersContainerTest : public ::testing::Test
{
protected:
     void SetUp() override { _pcPtr = std::make_shared<rs::ParametersContainer>(); }
     void TearDown() override { _pcPtr.reset(); }

     std::shared_ptr<rs::ParametersContainer> _pcPtr;
};

TEST_F(ParametersContainerTest, getParameter)
{

     LOG(INFO) << "CameraParameterTest test getParameter  begin";
     std::string id1 = "id_1";
     int value1 = 8;
     int min1 = 0;
     int max1 = 10;
     int step1 = 1;
     int defaulVal1 = 5;
     std::string desc1 = "desc1";
     std::shared_ptr<rs::Parameter> param1 = nullptr;
     param1 = std::make_shared<rs::PtrParameter<int>>(min1, max1, step1, defaulVal1, &value1, desc1);
     _pcPtr->addParameter(id1, param1);

     std::string id2 = "id_2";
     int value2 = 110;
     int min2 = 10;
     int max2 = 20;
     int step2 = 2;
     int defaulVal2 = 15;
     std::string desc2 = "desc2";
     std::shared_ptr<rs::Parameter> param2 = nullptr;
     param2 = std::make_shared<rs::PtrParameter<int>>(min2, max2, step2, defaulVal2, &value2, desc2);
     _pcPtr->addParameter(id2, param2);
     bool expected = true;
     bool result = true;
     result = typeid(rs::InvalidParameter) == typeid(_pcPtr->getParameter("notexist"));
     EXPECT_EQ(result, expected);
     expected = true;
     result = typeid(rs::PtrParameter<int>) == typeid(_pcPtr->getParameter(id2));
     EXPECT_EQ(result, expected);

     EXPECT_FLOAT_EQ(_pcPtr->getParameter(id1).query(), value1);
     EXPECT_EQ(_pcPtr->getParameter(id1).getDescription(), desc1);
     EXPECT_EQ(_pcPtr->getParameter(id1).getDefault(), defaulVal1);

     EXPECT_FLOAT_EQ(_pcPtr->getParameter(id2).query(), value2);
     EXPECT_EQ(_pcPtr->getParameter(id2).getDescription(), desc2);
     EXPECT_EQ(_pcPtr->getParameter(id2).getDefault(), defaulVal2);

     LOG(INFO) << "ParametersContainerTest test getParameter  end";
}

TEST_F(ParametersContainerTest, getParameterHandler)
{

     LOG(INFO) << "ParametersContainerTest test getParameter  begin";
     std::string id1 = "id_1";
     int value1 = 8;
     int min1 = 0;
     int max1 = 10;
     int step1 = 1;
     int defaulVal1 = 5;
     std::string desc1 = "desc1";
     std::shared_ptr<rs::Parameter> param1 = nullptr;
     param1 = std::make_shared<rs::PtrParameter<int>>(min1, max1, step1, defaulVal1, &value1, desc1);
     _pcPtr->addParameter(id1, param1);

     std::string id2 = "id_2";
     int value2 = 110;
     int min2 = 10;
     int max2 = 20;
     int step2 = 2;
     int defaulVal2 = 15;
     std::string desc2 = "desc2";
     std::shared_ptr<rs::Parameter> param2 = nullptr;
     param2 = std::make_shared<rs::PtrParameter<int>>(min2, max2, step2, defaulVal2, &value2, desc2);
     _pcPtr->addParameter(id2, param2);

     EXPECT_EQ(_pcPtr->getParameterHandler("notexist").get(), nullptr);
     EXPECT_EQ(_pcPtr->getParameterHandler(id1).get()->query(), value1);
     EXPECT_EQ(_pcPtr->getParameterHandler(id2).get()->query(), value2);

     LOG(INFO) << "ParametersContainerTest test getParameterHandler  end";
}

TEST_F(ParametersContainerTest, convertFromJson)
{
     LOG(INFO) << "ParametersContainerTest test convertFromJson  begin";
     std::string id = "id";
     int value = 110;
     int min = 10;
     int max = 20;
     int step = 2;
     int defaulVal = 15;
     std::string desc = "desc";
     std::shared_ptr<rs::Parameter> param = nullptr;
     param = std::make_shared<rs::PtrParameter<int>>(min, max, step, defaulVal, &value, desc);
     _pcPtr->addParameter(id, param);

     nlohmann::json json_value;
     int validNewValue = min + max / 2;
     json_value[id] = validNewValue;

     bool result = _pcPtr->convertFromJson(json_value);
     bool expected = true;
     EXPECT_EQ(result, expected);
     EXPECT_FLOAT_EQ(_pcPtr->getParameterHandler(id).get()->query(), validNewValue);

     json_value[id] = validNewValue + max;

     result = _pcPtr->convertFromJson(json_value);
     expected = false;
     EXPECT_EQ(result, expected);

     LOG(INFO) << "ParametersContainerTest test convertFromJson  end";
}
TEST_F(ParametersContainerTest, convertToJson)
{
     LOG(INFO) << "ParametersContainerTest test convertToJson  begin";
     // wait the implemantation method

     nlohmann::json json_value;
     bool result = _pcPtr->convertToJson(json_value);
     bool expected = false;
     EXPECT_EQ(result, expected);

     LOG(INFO) << "ParametersContainerTest test convertToJson  begin";
}
TEST_F(ParametersContainerTest, getSupportedParameters)
{

     LOG(INFO) << "ParametersContainerTest test getSupportedParameters  begin";
     std::string id1 = "id_1";
     int value1 = 8;
     int min1 = 0;
     int max1 = 10;
     int step1 = 1;
     int defaulVal1 = 5;
     std::string desc1 = "desc1";
     std::shared_ptr<rs::Parameter> param1 = nullptr;
     param1 = std::make_shared<rs::PtrParameter<int>>(min1, max1, step1, defaulVal1, &value1, desc1);
     _pcPtr->addParameter(id1, param1);

     std::string id2 = "id_2";
     int value2 = 110;
     int min2 = 10;
     int max2 = 20;
     int step2 = 2;
     int defaulVal2 = 15;
     std::string desc2 = "desc2";
     std::shared_ptr<rs::Parameter> param2 = nullptr;
     param2 = std::make_shared<rs::PtrParameter<int>>(min2, max2, step2, defaulVal2, &value2, desc2);
     _pcPtr->addParameter(id2, param2);

     std::string id3 = "id_3";
     int value3 = 110;
     int min3 = 10;
     int max3 = 20;
     int step3 = 2;
     int defaulVal3 = 15;
     std::string desc3 = "desc3";
     std::shared_ptr<rs::Parameter> param3 = nullptr;
     param3 = std::make_shared<rs::PtrParameter<int>>(min3, max3, step3, defaulVal3, &value3, desc3);
     _pcPtr->addParameter(id3, param3);

     auto parameters = _pcPtr->getSupportedParameters();
     auto it = std::find(parameters.begin(), parameters.end(), id1);
     bool result = it != parameters.end();
     bool expected = true;
     EXPECT_EQ(result, expected);

     it = std::find(parameters.begin(), parameters.end(), id2);
     result = it != parameters.end();
     expected = true;
     EXPECT_EQ(result, expected);

     it = std::find(parameters.begin(), parameters.end(), id3);
     result = it != parameters.end();
     expected = true;
     EXPECT_EQ(result, expected);

     it = std::find(parameters.begin(), parameters.end(), "paramNotExist");
     result = it == parameters.end();
     expected = true;
     EXPECT_EQ(result, expected);

     LOG(INFO) << "ParametersContainerTest test getSupportedParameters  end";
}
TEST_F(ParametersContainerTest, setDefaults)
{

     LOG(INFO) << "CameraParameterTest test setDefaults  begin";
     std::string id1 = "id_1";
     int value1 = 8;
     int min1 = 0;
     int max1 = 10;
     int step1 = 1;
     int defaulVal1 = 5;
     std::string desc1 = "desc1";
     std::shared_ptr<rs::Parameter> param1 = nullptr;
     param1 = std::make_shared<rs::PtrParameter<int>>(min1, max1, step1, defaulVal1, &value1, desc1);
     _pcPtr->addParameter(id1, param1);

     std::string id2 = "id_2";
     int value2 = 110;
     int min2 = 10;
     int max2 = 20;
     int step2 = 2;
     int defaulVal2 = 15;
     std::string desc2 = "desc2";
     std::shared_ptr<rs::Parameter> param2 = nullptr;
     param2 = std::make_shared<rs::PtrParameter<int>>(min2, max2, step2, defaulVal2, &value2, desc2);
     _pcPtr->addParameter(id2, param2);

     std::string id3 = "id_3";
     int value3 = 12;
     int min3 = 10;
     int max3 = 220;
     int step3 = 3;
     int defaulVal3 = 35;
     std::string desc3 = "desc3";
     std::shared_ptr<rs::Parameter> param3 = nullptr;
     param3 = std::make_shared<rs::PtrParameter<int>>(min3, max3, step3, defaulVal3, &value3, desc3);
     _pcPtr->addParameter(id3, param3);
     _pcPtr->setDefaults();

     EXPECT_FLOAT_EQ(_pcPtr->getParameter(id1).query(), defaulVal1);
     EXPECT_FLOAT_EQ(_pcPtr->getParameter(id2).query(), defaulVal2);
     EXPECT_FLOAT_EQ(_pcPtr->getParameter(id3).query(), defaulVal3);

     LOG(INFO) << "ParametersContainerTest test setDefaults  end";
}
TEST_F(ParametersContainerTest, supportParameter)
{

     LOG(INFO) << "ParametersContainerTest test supportParameter  begin";
     std::string id1 = "id_1";
     int value1 = 8;
     int min1 = 0;
     int max1 = 10;
     int step1 = 1;
     int defaulVal1 = 5;
     std::string desc1 = "desc1";
     std::shared_ptr<rs::Parameter> param1 = nullptr;
     param1 = std::make_shared<rs::PtrParameter<int>>(min1, max1, step1, defaulVal1, &value1, desc1);
     _pcPtr->addParameter(id1, param1);

     std::string id2 = "id_2";
     int value2 = 110;
     int min2 = 10;
     int max2 = 20;
     int step2 = 2;
     int defaulVal2 = 15;
     std::string desc2 = "desc2";
     std::shared_ptr<rs::Parameter> param2 = nullptr;
     param2 = std::make_shared<rs::PtrParameter<int>>(min2, max2, step2, defaulVal2, &value2, desc2);
     _pcPtr->addParameter(id2, param2);
     std::string id3 = "id_3";
     int value3 = 110;
     int min3 = 10;
     int max3 = 20;
     int step3 = 2;
     int defaulVal3 = 15;
     std::string desc3 = "desc3";
     std::shared_ptr<rs::Parameter> param3 = nullptr;
     param3 = std::make_shared<rs::PtrParameter<int>>(min3, max3, step3, defaulVal3, &value3, desc3);
     _pcPtr->addParameter(id3, param3);
     bool result = true;
     bool expected = true;
     result = _pcPtr->supportParameter(id1);
     EXPECT_EQ(result, expected);
     expected = true;
     result = _pcPtr->supportParameter(id2);
     EXPECT_EQ(result, expected);
     expected = true;
     result = _pcPtr->supportParameter(id3);
     EXPECT_EQ(result, expected);
     expected = false;
     result = _pcPtr->supportParameter("notSupported");
     EXPECT_EQ(result, expected);

     LOG(INFO) << "ParametersContainerTest test supportParameter  end";
}
TEST_F(ParametersContainerTest, setAll)
{
     LOG(INFO) << "ParametersContainerTest test setAll  begin";
     std::string id = "id";
     int basseValue = 5;
     std::unique_ptr<int> values[5];
     int min = 0;
     int max = 10;
     int step = 1;
     int defaulVal = 5;
     std::string desc = "desc";
    // std::shared_ptr<rs::Parameter> param = nullptr;
     for (int idx = 1; idx <= 5; idx++)
     {
          values[idx-1]= std::make_unique<int>(basseValue * idx);        
          std::string descIter = desc + std::to_string(idx); 
         std::shared_ptr<rs::Parameter> param  = std::make_shared<rs::PtrParameter<int>>(min * idx, max * idx, step, defaulVal * idx,  values[idx-1].get(),  descIter);
          _pcPtr->addParameter(std::string(id + std::to_string(idx)), param);
         
     }
     for (int idx = 1; idx <= 5; idx++)
     {
          std::string descIter = desc + std::to_string(idx);
          auto idString = std::string(id + std::to_string(idx));
          EXPECT_FLOAT_EQ(_pcPtr->getParameter(std::string(id + std::to_string(idx))).query(), basseValue * idx);
          EXPECT_EQ(_pcPtr->getParameter(std::string(id + std::to_string(idx))).getDescription(), descIter);
          EXPECT_FLOAT_EQ(_pcPtr->getParameter(std::string(id + std::to_string(idx))).getDefault(), defaulVal * idx);
     }

     rs::ParametersContainer pCNewValue;
     for (int idx = 1; idx <= 5; idx++)
     {
          *values[idx-1]=(basseValue * (idx * 2));        
          std::string descIter = desc + std::to_string(idx); 
         std::shared_ptr<rs::Parameter> param  = std::make_shared<rs::PtrParameter<int>>(min * (idx*2), max * (idx*2), step, defaulVal * (idx*2),  values[idx-1].get(),  descIter);
          pCNewValue.addParameter(std::string(id + std::to_string(idx)), param);
         
     }

     _pcPtr->setAll(pCNewValue);
     for (int idx = 1; idx <= 5; idx++)
     {
          std::string descIter = desc + std::to_string(idx);
          auto idString = std::string(id + std::to_string(idx));
          EXPECT_FLOAT_EQ(_pcPtr->getParameter(std::string(id + std::to_string(idx))).query(), basseValue * idx*2);
          EXPECT_EQ(_pcPtr->getParameter(std::string(id + std::to_string(idx))).getDescription(), descIter);
          EXPECT_FLOAT_EQ(_pcPtr->getParameter(std::string(id + std::to_string(idx))).getDefault(), defaulVal * idx*2);

          EXPECT_FLOAT_EQ(_pcPtr->getParameter(std::string(id + std::to_string(idx))).getRange().min,min*idx*2);
          EXPECT_FLOAT_EQ(_pcPtr->getParameter(std::string(id + std::to_string(idx))).getRange().max,max*idx*2);
     
     }
 
     LOG(INFO) << "ParametersContainerTest test setAll  begin";
}