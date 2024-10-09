#include <iostream>
#include "MathOperation.hpp"
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "calib_io.h"

class Calib_Io_Test : public ::testing::Test
{
protected:
     void SetUp() override {}
     void TearDown() override {}
};

TEST_F(Calib_Io_Test, readCalibration)
{

     bool excepted = false;
     const std::string pathFile = "";
     nlohmann::json json_value;
     rs::ConveyorCalibrationParameters params;
    excepted = rs::io::readConveyorCalibration(pathFile, params);
     EXPECT_EQ(excepted, false);

   
     //read the calibration OK
      //check the parameter after read

}

TEST_F(Calib_Io_Test, checkParameters)
{

    
}
