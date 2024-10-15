#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "CommonTest.h"
#include "camera_parameters.h"
class CameraParameterTest : public ::testing::Test
{
protected:
     void SetUp() override {}
     void TearDown() override {}
     rs::calib::CameraParameters cp;
};

TEST_F(CameraParameterTest, checkIntrinsicsOk)
{

     LOG(INFO) << "CameraParameterTest test checkIntrinsicsOk  begin";
     bool expected = true;
     bool result = true;
     int width = 512;
     int height = 512;
     float fx = 10;
     float fy = 20;
     float ppx = 0;
     float ppy = 1;
     cp.setIntrinsics(width, height, fx, fy, ppx, ppy);
     result = cp.checkIntrinsics();
     EXPECT_EQ(result, expected);

     LOG(INFO) << "Calib_Io_Test test checkIntrinsicsOk  end";
}
TEST_F(CameraParameterTest, checkIntrinsicsKO)
{

     LOG(INFO) << "CameraParameterTest test checkIntrinsicsKO  begin";
     bool expected = false;
     bool result = true;
     LOG(INFO) << "CameraParameterTest <fx=0> <fy=0>";
     int width = 512;
     int height = 512;
     float fx = 0;
     float fy = 0;
     float ppx = 0;
     float ppy = 1;
     cp.setIntrinsics(width, height, fx, fy, ppx, ppy);
     result = cp.checkIntrinsics();
     EXPECT_EQ(result, expected);
     fx = 10;
     fy = 20;
     width = 0;
     LOG(INFO) << "CameraParameterTest <width=0>";
     cp.setIntrinsics(width, height, fx, fy, ppx, ppy);
     result = cp.checkIntrinsics();
     expected = false;
     EXPECT_EQ(result, expected);
     LOG(INFO) << "CameraParameterTest <width=-10>";
     width = -10;
     cp.setIntrinsics(width, height, fx, fy, ppx, ppy);
     result = cp.checkIntrinsics();
     expected = false;
     EXPECT_EQ(result, expected);
     LOG(INFO) << "CameraParameterTest <height=-1>";
     width = 10;
     height = -1;
     cp.setIntrinsics(width, height, fx, fy, ppx, ppy);
     result = cp.checkIntrinsics();
     expected = false;
     EXPECT_EQ(result, expected);
     LOG(INFO) << "CameraParameterTest <fx=-1>";
     width = 10;
     height = 11;
     fx=-1;
     cp.setIntrinsics(width, height, fx, fy, ppx, ppy);
     result = cp.checkIntrinsics();
     expected = false;
     EXPECT_EQ(result, expected);
     LOG(INFO) << "CameraParameterTest <fy=-1>";
     width = 10;
     height = 11;
     fx=1;
     fy=-1;
     cp.setIntrinsics(width, height, fx, fy, ppx, ppy);
     result = cp.checkIntrinsics();
     expected = false;
     EXPECT_EQ(result, expected);
     LOG(INFO) << "Calib_Io_Test test checkIntrinsicsKO  end";
}
