#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "calib_io.h"
#include "CommonTest.h"
#include "parameters.h"
#include "Util.h"
class Calib_Io_Test : public ::testing::Test
{
public:
     static std::string getconfigFile() { return _fullNameConfig; }
      static std::string getIntrinsicsCalibFile() { return _intrinsicsCalibConfig; }

protected:
     void SetUp() override
     {
          std::string homeDir=std::getenv("WORKSPACE")!=nullptr?std::getenv("WORKSPACE"):std::getenv("HOME");           
          if (!_fullNameConfig.size())
          {
               _fullNameConfig = findFileRecursively(homeDir, nameCalibConf);
          }
           if (!_intrinsicsCalibConfig.size())
          {
               _intrinsicsCalibConfig = findFileRecursively(homeDir, nameIntrinsicsCalibrConf);
          }


          return;
     }
     void TearDown() override {}

private:
     static std::string _fullNameConfig;
      static std::string _intrinsicsCalibConfig;
};
std::string Calib_Io_Test::_fullNameConfig = "";
std::string Calib_Io_Test::_intrinsicsCalibConfig = "";

TEST_F(Calib_Io_Test, readCalibration)
{

     LOG(INFO) << "Calib_Io_Test test readCalibration  begin";
     bool expected = false;
     std::string pathFile = "";
     nlohmann::json json_value;
     rs::ConveyorCalibrationParameters params;

     bool result = rs::io::readConveyorCalibration(pathFile, params);
     EXPECT_EQ(result, expected);

     //pathFile = calibConfig;
     pathFile = Calib_Io_Test::getconfigFile();
     result = rs::io::readConveyorCalibration(pathFile, params);
     expected = true;
     EXPECT_EQ(result, expected);

     int roi_x = params.getParameter("roi_x").query();
     EXPECT_EQ(roi_x, roi_xConfigValue);

     int roi_y = params.getParameter("roi_y").query();
     EXPECT_EQ(roi_y, roi_yConfigValue);

     int roi_width = params.getParameter("roi_width").query();
     EXPECT_EQ(roi_width, roi_widthConfigValue);

     int roi_height = params.getParameter("roi_height").query();
     EXPECT_EQ(roi_height, roi_heightConfigValue);

     float range_x_min = params.getParameter("range_x_min").query();
     EXPECT_FLOAT_EQ(range_x_min, range_x_minConfigValue);

     float range_x_max = params.getParameter("range_x_max").query();
     EXPECT_FLOAT_EQ(range_x_max, range_x_maxConfigValue);

     float range_y_min = params.getParameter("range_y_min").query();
     EXPECT_FLOAT_EQ(range_y_min, range_y_minConfigValue);

     float range_y_max = params.getParameter("range_y_max").query();
     EXPECT_FLOAT_EQ(range_y_max, range_y_maxConfigValue);

     float range_z_min = params.getParameter("range_z_min").query();
     EXPECT_FLOAT_EQ(range_z_min, range_z_minConfigValue);

     float range_z_max = params.getParameter("range_z_max").query();
     EXPECT_FLOAT_EQ(range_z_max, range_z_maxConfigValue);

     float camera_position_x = params.getParameter("camera_position_x").query();
     EXPECT_FLOAT_EQ(camera_position_x, camera_position_xConfigValue);

     float camera_position_y = params.getParameter("camera_position_y").query();
     EXPECT_FLOAT_EQ(camera_position_y, camera_position_yConfigValue);

     int camera_position_z = params.getParameter("camera_position_z").query();
     EXPECT_EQ(camera_position_z, camera_position_zConfigValue);

     float camera_orientation_x = params.getParameter("camera_orientation_x").query();
     EXPECT_FLOAT_EQ(camera_orientation_x, camera_orientation_xConfigValue);

     float camera_orientation_y = params.getParameter("camera_orientation_y").query();
     EXPECT_FLOAT_EQ(camera_orientation_y, camera_orientation_yConfigValue);

     float camera_orientation_z = params.getParameter("camera_orientation_z").query();
     EXPECT_FLOAT_EQ(camera_orientation_z, camera_orientation_zConfigValue);

     result = typeid(rs::InvalidParameter) == typeid(params.getParameter(""));
     expected = true;
     EXPECT_EQ(result, expected);
     LOG(INFO) << "Calib_Io_Test test readCalibration  end";
}

TEST_F(Calib_Io_Test, readCameraIntrinsics)
{
     LOG(INFO) << "Calib_Io_Test test readCameraIntrinsics  begin";
     bool expected = false;

     std::string pathFile = "";
     rs::Intrinsics params;
     bool result = rs::io::readCameraIntrinsics(pathFile, 3, params);
     EXPECT_EQ(result, expected);

    // pathFile = ConveyorCalibrationConfig;
   pathFile=Calib_Io_Test::getIntrinsicsCalibFile();
     expected = true;
     result = rs::io::readCameraIntrinsics(pathFile, 3, params);
     EXPECT_EQ(result, expected);

     EXPECT_EQ(params.image_size[0], IntrinsicsConigValue.image_size[0]);
     EXPECT_EQ(params.image_size[1], IntrinsicsConigValue.image_size[1]);
     EXPECT_DOUBLE_EQ(params.principal_point[0], IntrinsicsConigValue.principal_point[0]);
     EXPECT_DOUBLE_EQ(params.principal_point[1], IntrinsicsConigValue.principal_point[1]);
     EXPECT_DOUBLE_EQ(params.focal_length[0], IntrinsicsConigValue.focal_length[0]);
     EXPECT_DOUBLE_EQ(params.focal_length[1], IntrinsicsConigValue.focal_length[1]);
     EXPECT_EQ(params.model, IntrinsicsConigValue.model);

     EXPECT_DOUBLE_EQ(params.distortion_coeffs[0], IntrinsicsConigValue.distortion_coeffs[0]);
     EXPECT_DOUBLE_EQ(params.distortion_coeffs[1], IntrinsicsConigValue.distortion_coeffs[1]);
     EXPECT_DOUBLE_EQ(params.distortion_coeffs[2], IntrinsicsConigValue.distortion_coeffs[2]);
     EXPECT_DOUBLE_EQ(params.distortion_coeffs[3], IntrinsicsConigValue.distortion_coeffs[3]);
     EXPECT_DOUBLE_EQ(params.distortion_coeffs[4], IntrinsicsConigValue.distortion_coeffs[4]);
     EXPECT_DOUBLE_EQ(params.distortion_coeffs[5], IntrinsicsConigValue.distortion_coeffs[5]);
     EXPECT_DOUBLE_EQ(params.distortion_coeffs[6], IntrinsicsConigValue.distortion_coeffs[6]);
     EXPECT_DOUBLE_EQ(params.distortion_coeffs[7], IntrinsicsConigValue.distortion_coeffs[7]);

     LOG(INFO) << "Calib_Io_Test test readCameraIntrinsics  end";
}
