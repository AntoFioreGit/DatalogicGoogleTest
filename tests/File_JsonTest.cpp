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



TEST_F(File_JsonTest, readConfigFile)
{
    
     bool excepted = false;
     const std::string pathFile = "";
      nlohmann::json json_value;
      //inserire test positivo
     excepted = rs::io::readJsonFile("",json_value);
     EXPECT_EQ(excepted ,false);
}

TEST_F(File_JsonTest, checkParameters)
{
    
     bool excepted = false;
     const std::string pathFile = "";
      nlohmann::json json_value;
     excepted = rs::io::readJsonFile("",json_value);
     EXPECT_EQ(excepted ,false);
     //TODO Check the parameters
}
 










