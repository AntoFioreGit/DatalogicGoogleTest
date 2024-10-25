#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "aditof_intrinsics.h"

#include "CommonTest.h"

#include "Util.h"
#include "filesytem_helpers.h"
#include "TOF_Calibration_Types.h"
using namespace rs;

class AdTof_IntrisicsTest : public ::testing::Test
{
public:
//da sostituire con intrin.......
     static std::string getconveyorCalibFile() { return _conveyorCalibConfig; }

protected:
     void SetUp() override
     {
          std::string homeDir = std::getenv("HOME");

          if (!_conveyorCalibConfig.size())
          {
               //  fs::path homeDir = std::getenv("HOME");
               _conveyorCalibConfig = findFileRecursively(homeDir, nameConveyorCalibrConf);
          }

          return;
     }
     void TearDown() override {}

private:
     static std::string _conveyorCalibConfig;
    
};

std::string AdTof_IntrisicsTest::_conveyorCalibConfig = "";


TEST_F(AdTof_IntrisicsTest, ReadCCB)
{

     LOG(INFO) << "AdTof_IntrisicsTest test ReadCCB  begin";
     bool expected = true;
     std::string pathFile = AdTof_IntrisicsTest::getconveyorCalibFile();
     rs::TofiCCBData ccb_data;
     uint16_t mode = 0;

     bool result = rs::ReadCCB(pathFile, &ccb_data, mode);
     EXPECT_EQ(result, expected);

     EXPECT_EQ(ccb_data.n_rows, ccb_data_ConfigValue.n_rows);
     EXPECT_EQ(ccb_data.n_cols, ccb_data_ConfigValue.n_cols);
     EXPECT_EQ(ccb_data.n_freqs, ccb_data_ConfigValue.n_freqs);
     EXPECT_EQ(ccb_data.row_bin_factor, ccb_data_ConfigValue.row_bin_factor);
     EXPECT_EQ(ccb_data.col_bin_factor, ccb_data_ConfigValue.col_bin_factor);
     EXPECT_EQ(ccb_data.n_offset_rows, ccb_data_ConfigValue.n_offset_rows);
     EXPECT_EQ(ccb_data.n_offset_cols, ccb_data_ConfigValue.n_offset_cols);
     EXPECT_EQ(ccb_data.n_sensor_rows, ccb_data_ConfigValue.n_sensor_rows);
     EXPECT_EQ(ccb_data.n_sensor_cols, ccb_data_ConfigValue.n_sensor_cols);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.fx, ccb_data_ConfigValue.camera_intrinsics.fx);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.fy, ccb_data_ConfigValue.camera_intrinsics.fy);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.cx, ccb_data_ConfigValue.camera_intrinsics.cx);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.cy, ccb_data_ConfigValue.camera_intrinsics.cy);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.codx, ccb_data_ConfigValue.camera_intrinsics.codx);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.cody, ccb_data_ConfigValue.camera_intrinsics.cody);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.k1, ccb_data_ConfigValue.camera_intrinsics.k1);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.k2, ccb_data_ConfigValue.camera_intrinsics.k2);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.k3, ccb_data_ConfigValue.camera_intrinsics.k3);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.k4, ccb_data_ConfigValue.camera_intrinsics.k4);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.k5, ccb_data_ConfigValue.camera_intrinsics.k5);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.k6, ccb_data_ConfigValue.camera_intrinsics.k6);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.p1, ccb_data_ConfigValue.camera_intrinsics.p1);
     EXPECT_FLOAT_EQ(ccb_data.camera_intrinsics.p2, ccb_data_ConfigValue.camera_intrinsics.p2);

     LOG(INFO) << "AdTof_IntrisicsTest test ReadCCB  end";
}
