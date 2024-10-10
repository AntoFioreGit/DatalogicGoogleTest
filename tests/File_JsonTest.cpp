#include <iostream>
#include "MathOperation.hpp"
#include <gtest/gtest.h>
#include <glog/logging.h>

#include "io/file_json.h"

class File_JsonTest : public ::testing::Test
{
protected:
     void SetUp() override {}
     void TearDown() override {}
};

TEST_F(File_JsonTest, readFile)
{

     LOG(INFO) << "File_JsonTest test readFile  begin";
     bool excepted = false;
     std::string pathFile = "";
     nlohmann::json json_value;
     bool result = rs::io::readJsonFile(pathFile, json_value);
     EXPECT_EQ(result, excepted);
     excepted = true;
     pathFile="../../data/config/rs_algo_config.json";
     result = rs::io::readJsonFile(pathFile, json_value);
     EXPECT_EQ(result, excepted);
     LOG(INFO) << "File_JsonTest test readFile  end";
}

TEST_F(File_JsonTest, checkParameters)
{

     bool excepted = false;
     const std::string pathFile = "";
     nlohmann::json json_value;
     excepted = rs::io::readJsonFile("", json_value);
     EXPECT_EQ(excepted, false);
     // TODO Check the parameters
}
