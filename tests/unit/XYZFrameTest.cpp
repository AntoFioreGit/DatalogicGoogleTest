#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include "CommonTest.h"
#include "Util.h"
#include "frame.h"
#include <fstream>
#include <vector>
#include "types.h"
#include "geometry.h"
#include "frame_processor.h"
#include "calib_io.h"
#include "filesytem_helpers.h"
#include "json.h"
#include "file_json.h"

#include "profile3d_calculator.h"
using namespace rs;
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
class XYZFrameTest : public ::testing::Test
{
public:
     static std::string getFileIntCalibXYZFrame() { return _intrCalXYZFrameConf; }
     static std::string getFileCalibXYZFrame() { return _calibXYZFrameConf; }
     static std::string getAlgoFile() { return _algoConf; }

     

protected:
     void SetUp() override
     {
          std::string homeDir = std::getenv("WORKSPACE") != nullptr ? std::getenv("WORKSPACE") : std::getenv("HOME");
          if (!_intrCalXYZFrameConf.size())
          {
               _intrCalXYZFrameConf = findFileRecursively(homeDir, nameIntrCalXYZFrameConf);
          }
          if (!_calibXYZFrameConf.size())
          {
               _calibXYZFrameConf = findFileRecursively(homeDir, nameCalibXYZFrameConf);
          }
          if (!_algoConf.size())
          {
               _algoConf = findFileRecursively(homeDir, nameAlgoConf);
          }
       
          return;
     }
     void TearDown() override {}

protected:
     static std::string _intrCalXYZFrameConf;
     static std::string _calibXYZFrameConf;
     static std::string _algoConf;
};

std::string XYZFrameTest::_intrCalXYZFrameConf = "";
std::string XYZFrameTest::_calibXYZFrameConf = "";
std::string XYZFrameTest::_algoConf = "";

TEST_F(XYZFrameTest, checkProfiles)
{

     LOG(INFO) << "XYZFrameTest test checkProfiles  start";
     std::string pathAlgoFile = XYZFrameTest::getAlgoFile();
     std::string pathCalibXYZFile = XYZFrameTest::getFileCalibXYZFrame();
     std::string pathIntCalXYZFile = XYZFrameTest::getFileIntCalibXYZFrame();
     bool expected = true;
     bool result = true;

     std::set<Keyonfig> keys{ALGO, OUTDIR};
     std::string tmpConfFile = generateTempConf(pathAlgoFile, keys);
     if (!tmpConfFile.size())
          return;

     nlohmann::json json_value;
     result = rs::io::readJsonFile(tmpConfFile, json_value);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration file " << tmpConfFile;
          return;
     }
     std::string outDir = json_value["save"]["out_dir"];
     rs::utils::filesystem::MakeDirectory(outDir);

     rs::Intrinsics cam_intrinsics;
     result = rs::io::readCameraIntrinsics(pathIntCalXYZFile, 3, cam_intrinsics); // 3 = ADTF "lr-qnative"
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load camera intrinsics from file " << pathIntCalXYZFile;
          return;
     }
     rs::ConveyorCalibrationParameters calib_params;
     result = rs::io::readConveyorCalibration(pathCalibXYZFile, calib_params);
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load  target calibration from file  " << pathCalibXYZFile;
          return;
     }

     FrameProcessor fp;
     result = fp.initialize(tmpConfFile);
     expected = true;
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration  file  " << pathAlgoFile;
          return;
     }
     fp.configure(calib_params, cam_intrinsics);
     std::string homeDir = std::getenv("WORKSPACE") != nullptr ? std::getenv("WORKSPACE") : std::getenv("HOME");
     std::vector<char> frame_buffer;
     int frame_width = cam_intrinsics.image_size[0];
     int frame_height = cam_intrinsics.image_size[1];
     int NUMBER_POINT_4_PROFILE = (3 * 2);
     for (int idx = ZERO_PROF; idx <= THREE_PROF; idx++)
     {

          LOG(INFO) << "Test Profiles Begin. Numerber profiles expected:  " << idx;
          // Active Brightness
          ImageFrame ab_frame;
          std::vector<char> ab_buffer;
          std::string ab_dataFile = findFileRecursively(homeDir, ab_DataMap[static_cast<Profile4File>(idx)]);

          result = rs::utils::filesystem::ReadFileToBuffer(ab_dataFile, frame_buffer);
          expected = true;
          EXPECT_EQ(result, expected);
          if (!result)
          {

               LOG(ERROR) << "Error on load AB  file  " << ab_dataFile;
               return;
          }
          ab_frame.prepare(frame_height, frame_width);
          ab_frame.copyPixels(reinterpret_cast<uint16_t *>(frame_buffer.data()), frame_height * frame_width);

          frame_buffer.clear();
          XyzFrame xyz_frame;

          std::string xyz_dataFile = findFileRecursively(homeDir, xyz_DataMap[static_cast<Profile4File>(idx)]);

          result = rs::utils::filesystem::ReadFileToBuffer(xyz_dataFile, frame_buffer);
          expected = true;
          EXPECT_EQ(result, expected);
          if (!result)
          {
               LOG(ERROR) << "Error on load XYZ data file  " << xyz_dataFile;
               return;
          }
          xyz_frame.prepare(frame_height, frame_width);
          xyz_frame.copyPoints(reinterpret_cast<int16_t *>(frame_buffer.data()), frame_height * frame_width);

          size_t num_profiles = fp.invoke(xyz_frame.getPoints(), ab_frame.getPixels());

          EXPECT_EQ(num_profiles, idx);

          std::vector<std::vector<float>> xyz_lines;
          xyz_lines = fp.getProfiles();
          int expectedSize = 3;
          EXPECT_EQ(xyz_lines.size(), expectedSize);

          int countPointValidResult = 0;
          for (auto xyz_line : xyz_lines)
          {
               countPointValidResult += std::count_if(xyz_line.begin(), xyz_line.end(), [](int x)
                                                      { return x != 0; });
          }

          int expectedXYZPoints = idx * NUMBER_POINT_4_PROFILE;
          EXPECT_EQ(countPointValidResult, expectedXYZPoints);
          LOG(INFO) << "Test Profiles End. Numerber profiles expected:  " << idx;
     }
     std::remove(tmpConfFile.c_str());
     removeDirectory(outDir);

     LOG(INFO) << "XYZFrameTest test checkProfiles  end";
}
TEST_F(XYZFrameTest, checkNoGenerateProfiles_0)
{

     LOG(INFO) << "XYZFrameTest test checkNoGenerateProfiles_0  start";
     std::string pathAlgoFile = XYZFrameTest::getAlgoFile();
     std::string pathCalibXYZFile = XYZFrameTest::getFileCalibXYZFrame();
     std::string pathIntCalXYZFile = XYZFrameTest::getFileIntCalibXYZFrame();
     bool expected = true;
     bool result = true;

     std::set<Keyonfig> keys{ALGO, OUTDIR};
     std::string tmpConfFile = generateTempConf(pathAlgoFile, keys);
     if (!tmpConfFile.size())
          return;

     nlohmann::json json_value;
     result = rs::io::readJsonFile(tmpConfFile, json_value);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration file " << tmpConfFile;
          return;
     }
     std::string outDir = json_value["save"]["out_dir"];
     rs::utils::filesystem::MakeDirectory(outDir);

     rs::Intrinsics cam_intrinsics;
     result = rs::io::readCameraIntrinsics(pathIntCalXYZFile, 3, cam_intrinsics); // 3 = ADTF "lr-qnative"
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load camera intrinsics from file " << pathIntCalXYZFile;
          return;
     }
     std::map<KeyCalibPar, float> keyVal{
         {RANGE_Z_MAX, -20.0},
         {RANGE_Y_MIN, -100.0},
         {RANGE_Y_MAX, -10.0},
         {RANGE_X_MIN, -100.0},
         {RANGE_X_MAX, -30.0},

     };
     std::string tmpCalibFile = generateTempCalibParam(pathCalibXYZFile, keyVal);
     if (!tmpCalibFile.size())
          return;

     rs::ConveyorCalibrationParameters calib_params;
     result = rs::io::readConveyorCalibration(tmpCalibFile, calib_params);
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load  target calibration from file  " << tmpCalibFile;
          return;
     }

     FrameProcessor fp;
     result = fp.initialize(tmpConfFile);
     expected = true;
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration  file  " << pathAlgoFile;
          return;
     }
     fp.configure(calib_params, cam_intrinsics);
     std::string homeDir = std::getenv("WORKSPACE") != nullptr ? std::getenv("WORKSPACE") : std::getenv("HOME");
     std::vector<char> frame_buffer;
     int frame_width = cam_intrinsics.image_size[0];
     int frame_height = cam_intrinsics.image_size[1];
     int expectedProfiles = 0;
     for (int idx = ZERO_PROF; idx <= THREE_PROF; idx++)
     {

          LOG(INFO) << "Test Profiles Begin. Numerber profiles expected:  " << idx;
          ImageFrame ab_frame;
          std::vector<char> ab_buffer;
          std::string ab_dataFile = findFileRecursively(homeDir, ab_DataMap[static_cast<Profile4File>(idx)]);
          result = rs::utils::filesystem::ReadFileToBuffer(ab_dataFile, frame_buffer);
          expected = true;
          EXPECT_EQ(result, expected);
          if (!result)
          {

               LOG(ERROR) << "Error on load AB  file  " << ab_dataFile;
               return;
          }
          ab_frame.prepare(frame_height, frame_width);
          ab_frame.copyPixels(reinterpret_cast<uint16_t *>(frame_buffer.data()), frame_height * frame_width);

          frame_buffer.clear();
          XyzFrame xyz_frame;
          std::string xyz_dataFile = findFileRecursively(homeDir, xyz_DataMap[static_cast<Profile4File>(idx)]);
          result = rs::utils::filesystem::ReadFileToBuffer(xyz_dataFile, frame_buffer);
          expected = true;
          EXPECT_EQ(result, expected);
          if (!result)
          {
               LOG(ERROR) << "Error on load XYZ data file  " << xyz_dataFile;
               return;
          }
          xyz_frame.prepare(frame_height, frame_width);
          xyz_frame.copyPoints(reinterpret_cast<int16_t *>(frame_buffer.data()), frame_height * frame_width);

          size_t num_profiles = fp.invoke(xyz_frame.getPoints(), ab_frame.getPixels());
          EXPECT_EQ(num_profiles, expectedProfiles);
          std::vector<std::vector<float>> xyz_lines;
          xyz_lines = fp.getProfiles();
          int expectedSize = 0;
          EXPECT_EQ(xyz_lines.size(), expectedSize);

          LOG(INFO) << "Test Profiles End. Numerber profiles expected:  " << idx;
     }
     std::remove(tmpConfFile.c_str());
     std::remove(tmpCalibFile.c_str());
     removeDirectory(outDir);

     LOG(INFO) << "XYZFrameTest test checkNoGenerateProfiles_0  end";
}
TEST_F(XYZFrameTest, checkNoGenerateProfiles_1)
{

     LOG(INFO) << "XYZFrameTest test checkNoGenerateProfiles_1  start";
     std::string pathAlgoFile = XYZFrameTest::getAlgoFile();
     std::string pathCalibXYZFile = XYZFrameTest::getFileCalibXYZFrame();
     std::string pathIntCalXYZFile = XYZFrameTest::getFileIntCalibXYZFrame();
     bool expected = true;
     bool result = true;

     std::set<Keyonfig> keys{ALGO, OUTDIR};
     std::string tmpConfFile = generateTempConf(pathAlgoFile, keys);
     if (!tmpConfFile.size())
          return;

     nlohmann::json json_value;
     result = rs::io::readJsonFile(tmpConfFile, json_value);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration file " << tmpConfFile;
          return;
     }
     std::string outDir = json_value["save"]["out_dir"];
     rs::utils::filesystem::MakeDirectory(outDir);

     rs::Intrinsics cam_intrinsics;
     result = rs::io::readCameraIntrinsics(pathIntCalXYZFile, 3, cam_intrinsics); // 3 = ADTF "lr-qnative"
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load camera intrinsics from file " << pathIntCalXYZFile;
          return;
     }
     std::map<KeyCalibPar, float> keyVal{
         {RANGE_Z_MAX, 0.0},
         {RANGE_Z_MIN, 0.0}

     };
     std::string tmpCalibFile = generateTempCalibParam(pathCalibXYZFile, keyVal);
     if (!tmpCalibFile.size())
          return;

     rs::ConveyorCalibrationParameters calib_params;
     result = rs::io::readConveyorCalibration(tmpCalibFile, calib_params);
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load  target calibration from file  " << tmpCalibFile;
          return;
     }

     FrameProcessor fp;
     result = fp.initialize(tmpConfFile);
     expected = true;
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration  file  " << pathAlgoFile;
          return;
     }
     fp.configure(calib_params, cam_intrinsics);
     std::string homeDir = std::getenv("WORKSPACE") != nullptr ? std::getenv("WORKSPACE") : std::getenv("HOME");
     std::vector<char> frame_buffer;
     int frame_width = cam_intrinsics.image_size[0];
     int frame_height = cam_intrinsics.image_size[1];
     int expectedProfiles = 0;
     for (int idx = ZERO_PROF; idx <= THREE_PROF; idx++)
     {

          LOG(INFO) << "Test Profiles Begin. Numerber profiles expected:  " << idx;
          ImageFrame ab_frame;
          std::vector<char> ab_buffer;
          std::string ab_dataFile = findFileRecursively(homeDir, ab_DataMap[static_cast<Profile4File>(idx)]);
          result = rs::utils::filesystem::ReadFileToBuffer(ab_dataFile, frame_buffer);
          expected = true;
          EXPECT_EQ(result, expected);
          if (!result)
          {

               LOG(ERROR) << "Error on load AB  file  " << ab_dataFile;
               return;
          }
          ab_frame.prepare(frame_height, frame_width);
          ab_frame.copyPixels(reinterpret_cast<uint16_t *>(frame_buffer.data()), frame_height * frame_width);

          frame_buffer.clear();
          XyzFrame xyz_frame;
          std::string xyz_dataFile = findFileRecursively(homeDir, xyz_DataMap[static_cast<Profile4File>(idx)]);
          result = rs::utils::filesystem::ReadFileToBuffer(xyz_dataFile, frame_buffer);
          expected = true;
          EXPECT_EQ(result, expected);
          if (!result)
          {
               LOG(ERROR) << "Error on load XYZ data file  " << xyz_dataFile;
               return;
          }
          xyz_frame.prepare(frame_height, frame_width);
          xyz_frame.copyPoints(reinterpret_cast<int16_t *>(frame_buffer.data()), frame_height * frame_width);

          size_t num_profiles = fp.invoke(xyz_frame.getPoints(), ab_frame.getPixels());
          EXPECT_EQ(num_profiles, expectedProfiles);
          std::vector<std::vector<float>> xyz_lines;
          xyz_lines = fp.getProfiles();
          int expectedSize = 0;
          EXPECT_EQ(xyz_lines.size(), expectedSize);

          LOG(INFO) << "Test Profiles End. Numerber profiles expected:  " << idx;
     }
     std::remove(tmpConfFile.c_str());
     std::remove(tmpCalibFile.c_str());
     removeDirectory(outDir);

     LOG(INFO) << "XYZFrameTest test checkNoGenerateProfiles_1  end";
}
TEST_F(XYZFrameTest, checkNoGenerateProfiles_2)
{

     LOG(INFO) << "XYZFrameTest test checkNoGenerateProfiles_2  start";
     std::string pathAlgoFile = XYZFrameTest::getAlgoFile();
     std::string pathCalibXYZFile = XYZFrameTest::getFileCalibXYZFrame();
     std::string pathIntCalXYZFile = XYZFrameTest::getFileIntCalibXYZFrame();
     bool expected = true;
     bool result = true;

     std::set<Keyonfig> keys{ALGO, OUTDIR};
     std::string tmpConfFile = generateTempConf(pathAlgoFile, keys);
     if (!tmpConfFile.size())
          return;

     nlohmann::json json_value;
     result = rs::io::readJsonFile(tmpConfFile, json_value);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration file " << tmpConfFile;
          return;
     }
     std::string outDir = json_value["save"]["out_dir"];
     rs::utils::filesystem::MakeDirectory(outDir);

     rs::Intrinsics cam_intrinsics;
     result = rs::io::readCameraIntrinsics(pathIntCalXYZFile, 3, cam_intrinsics); // 3 = ADTF "lr-qnative"
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load camera intrinsics from file " << pathIntCalXYZFile;
          return;
     }
     std::map<KeyCalibPar, float> keyVal{
         {ROI_X, 4095.0},
         {ROI_Y, 4095.0},
         {ROI_WIDTH, 5.0},
         {ROI_HEIGHT, 2.0}

     };
     std::string tmpCalibFile = generateTempCalibParam(pathCalibXYZFile, keyVal);
     if (!tmpCalibFile.size())
          return;

     rs::ConveyorCalibrationParameters calib_params;
     result = rs::io::readConveyorCalibration(tmpCalibFile, calib_params);
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load  target calibration from file  " << tmpCalibFile;
          return;
     }

     FrameProcessor fp;
     result = fp.initialize(tmpConfFile);
     expected = true;
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration  file  " << pathAlgoFile;
          return;
     }
     result = fp.configure(calib_params, cam_intrinsics);
     expected = false;
     EXPECT_EQ(result, expected);

     std::remove(tmpConfFile.c_str());
     std::remove(tmpCalibFile.c_str());

     LOG(INFO) << "XYZFrameTest test checkNoGenerateProfiles_2  end";
}
TEST_F(XYZFrameTest, checkNoGenerateProfiles_3)
{

     LOG(INFO) << "XYZFrameTest test checkNoGenerateProfiles_3  start";
     std::string pathAlgoFile = XYZFrameTest::getAlgoFile();
     std::string pathCalibXYZFile = XYZFrameTest::getFileCalibXYZFrame();
     std::string pathIntCalXYZFile = XYZFrameTest::getFileIntCalibXYZFrame();
     bool expected = true;
     bool result = true;

     std::set<Keyonfig> keys{ALGO, OUTDIR};
     std::string tmpConfFile = generateTempConf(pathAlgoFile, keys);
     if (!tmpConfFile.size())
          return;

     nlohmann::json json_value;
     result = rs::io::readJsonFile(tmpConfFile, json_value);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration file " << tmpConfFile;
          return;
     }
     std::string outDir = json_value["save"]["out_dir"];
     rs::utils::filesystem::MakeDirectory(outDir);

     rs::Intrinsics cam_intrinsics;
     result = rs::io::readCameraIntrinsics(pathIntCalXYZFile, 3, cam_intrinsics); // 3 = ADTF "lr-qnative"
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load camera intrinsics from file " << pathIntCalXYZFile;
          return;
     }
     std::map<KeyCalibPar, float> keyVal{
         {CAMERA_ORIENTATION_X, 90.0},
         {CAMERA_ORIENTATION_Z, 90.0},
         {CAMERA_ORIENTATION_Y, 90.0},

     };
     std::string tmpCalibFile = generateTempCalibParam(pathCalibXYZFile, keyVal);
     if (!tmpCalibFile.size())
          return;

     rs::ConveyorCalibrationParameters calib_params;
     result = rs::io::readConveyorCalibration(tmpCalibFile, calib_params);
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load  target calibration from file  " << tmpCalibFile;
          return;
     }

     FrameProcessor fp;
     result = fp.initialize(tmpConfFile);
     expected = true;
     EXPECT_EQ(result, expected);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration  file  " << pathAlgoFile;
          return;
     }
     result = fp.configure(calib_params, cam_intrinsics);
     expected = false;
     EXPECT_EQ(result, expected);
     std::remove(tmpConfFile.c_str());
     std::remove(tmpCalibFile.c_str());
     LOG(INFO) << "XYZFrameTest test checkNoGenerateProfiles_3  end";
}
