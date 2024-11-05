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
class CheckGenerateOutputTest : public ::testing::Test
{
public:
     static std::string getFileIntCalibXYZFrame() { return _intrCalXYZFrameConf; }
     static std::string getFileCalibXYZFrame() { return _calibXYZFrameConf; }
     static std::string getAlgoFile() { return _algoConf; }

protected:
     void SetUp() override
     {
          std::string homeDir = std::getenv("WORKSPACE")!=nullptr?std::getenv("WORKSPACE"):std::getenv("HOME");             

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

std::string CheckGenerateOutputTest::_intrCalXYZFrameConf = "";
std::string CheckGenerateOutputTest::_calibXYZFrameConf = "";
std::string CheckGenerateOutputTest::_algoConf = "";

TEST_F(CheckGenerateOutputTest, checkGenerateOutput)
{
    

     LOG(INFO) << "CheckGenerateOutputTest test checkGenerateOutput  start";
     std::string pathAlgoFile = CheckGenerateOutputTest::getAlgoFile();
     std::string pathCalibXYZFile = CheckGenerateOutputTest::getFileCalibXYZFrame();
     std::string pathIntCalXYZFile = CheckGenerateOutputTest::getFileIntCalibXYZFrame();
     bool expected = true;
     bool result = true;

     std::set<Keyonfig> keys{ALGO, OUTDIR,ENABLE_SAVE_AB,ENABLE_SAVE_XYZ};
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
     std::string homeDir = std::getenv("WORKSPACE")!=nullptr?std::getenv("WORKSPACE"):std::getenv("HOME");
     std::vector<char> frame_buffer;
     int frame_width = cam_intrinsics.image_size[0];
     int frame_height = cam_intrinsics.image_size[1];
     for (int idx = ZERO_PROF; idx <= THREE_PROF; idx++)
     {
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

          fp.invoke(xyz_frame.getPoints(), ab_frame.getPixels());
     }
     bool full_path = false;
     std::vector<std::string> filenames;
     rs::utils::filesystem::ListFilesInDirectoryWithExtension(outDir,
                                                              "png",
                                                              filenames,
                                                              full_path);
     int expectedNumberFiles = 4;
     int actualNumerFiles = filenames.size();
     EXPECT_EQ(actualNumerFiles, expectedNumberFiles);
     filenames.clear();
     rs::utils::filesystem::ListFilesInDirectoryWithExtension(outDir,
                                                              "ply",
                                                              filenames,
                                                              full_path);
     actualNumerFiles = filenames.size();
     EXPECT_EQ(actualNumerFiles, expectedNumberFiles);

     std::remove(tmpConfFile.c_str());
     removeDirectory(outDir);
     LOG(INFO) << "CheckGenerateOutputTest test checkGenerateOutput  end";
}
TEST_F(CheckGenerateOutputTest, checkGenerateOutputOnlyXYZ)
{
    

     LOG(INFO) << "CheckGenerateOutputTest test checkGenerateOutputOnlyXYZ  start";
     std::string pathAlgoFile = CheckGenerateOutputTest::getAlgoFile();
     std::string pathCalibXYZFile = CheckGenerateOutputTest::getFileCalibXYZFrame();
     std::string pathIntCalXYZFile = CheckGenerateOutputTest::getFileIntCalibXYZFrame();
     bool expected = true;
     bool result = true;

     std::set<Keyonfig> keys{ALGO, OUTDIR,DISABLE_SAVE_AB,ENABLE_SAVE_XYZ};
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
     std::string homeDir = std::getenv("WORKSPACE")!=nullptr?std::getenv("WORKSPACE"):std::getenv("HOME");
     std::vector<char> frame_buffer;
     int frame_width = cam_intrinsics.image_size[0];
     int frame_height = cam_intrinsics.image_size[1];
     for (int idx = ZERO_PROF; idx <= THREE_PROF; idx++)
     {
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

          fp.invoke(xyz_frame.getPoints(), ab_frame.getPixels());
     }
     bool full_path = false;
     std::vector<std::string> filenames;
     rs::utils::filesystem::ListFilesInDirectoryWithExtension(outDir,
                                                              "png",
                                                              filenames,
                                                              full_path);
     int expectedNumberFiles = 0;
     int actualNumerFiles = filenames.size();
     EXPECT_EQ(actualNumerFiles, expectedNumberFiles);
     filenames.clear();
     rs::utils::filesystem::ListFilesInDirectoryWithExtension(outDir,
                                                              "ply",
                                                              filenames,
                                                              full_path);
     actualNumerFiles = filenames.size();
     expectedNumberFiles=4;
     EXPECT_EQ(actualNumerFiles, expectedNumberFiles);

     std::remove(tmpConfFile.c_str());
     removeDirectory(outDir);
     LOG(INFO) << "CheckGenerateOutputTest test checkGenerateOutputOnlyXYZ  end";
}
TEST_F(CheckGenerateOutputTest, checkGenerateOutputOnlyPng)
{

     LOG(INFO) << "CheckGenerateOutputTest test checkGenerateOutputOnlyPng  start";
     std::string pathAlgoFile = CheckGenerateOutputTest::getAlgoFile();
     std::string pathCalibXYZFile = CheckGenerateOutputTest::getFileCalibXYZFrame();
     std::string pathIntCalXYZFile = CheckGenerateOutputTest::getFileIntCalibXYZFrame();
     bool expected = true;
     bool result = true;

     std::set<Keyonfig> keys{ALGO, OUTDIR,ENABLE_SAVE_AB,DISABLE_SAVE_XYZ};
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
     std::string homeDir = std::getenv("WORKSPACE")!=nullptr?std::getenv("WORKSPACE"):std::getenv("HOME");
     std::vector<char> frame_buffer;
     int frame_width = cam_intrinsics.image_size[0];
     int frame_height = cam_intrinsics.image_size[1];
    
     for (int idx = ZERO_PROF; idx <= THREE_PROF; idx++)
     {
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

          fp.invoke(xyz_frame.getPoints(), ab_frame.getPixels());
     }
     bool full_path = true;
     std::vector<std::string> filenames;
     rs::utils::filesystem::ListFilesInDirectoryWithExtension(outDir,
                                                              "png",
                                                              filenames,
                                                              full_path);
     int expectedNumberFiles = 4;
     int actualNumerFiles = filenames.size();
     EXPECT_EQ(actualNumerFiles, expectedNumberFiles);
     filenames.clear();
     rs::utils::filesystem::ListFilesInDirectoryWithExtension(outDir,
                                                              "ply",
                                                              filenames,
                                                              full_path);
     actualNumerFiles = filenames.size();
     expectedNumberFiles=0;
     EXPECT_EQ(actualNumerFiles, expectedNumberFiles);

     std::remove(tmpConfFile.c_str());
     removeDirectory(outDir);
     LOG(INFO) << "CheckGenerateOutputTest test checkGenerateOutputOnlyPng  end";
}
TEST_F(CheckGenerateOutputTest, checkGenerateOutputEmpty)
{


     LOG(INFO) << "CheckGenerateOutputTest test checkGenerateOutputEmpty  start";
     std::string pathAlgoFile = CheckGenerateOutputTest::getAlgoFile();
     std::string pathCalibXYZFile = CheckGenerateOutputTest::getFileCalibXYZFrame();
     std::string pathIntCalXYZFile = CheckGenerateOutputTest::getFileIntCalibXYZFrame();
     bool expected = true;
     bool result = true;

     std::set<Keyonfig> keys{ALGO, DISABLE_SAVE_AB,DISABLE_SAVE_XYZ};
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
     std::string homeDir = std::getenv("WORKSPACE")!=nullptr?std::getenv("WORKSPACE"):std::getenv("HOME");
     std::vector<char> frame_buffer;
     int frame_width = cam_intrinsics.image_size[0];
     int frame_height = cam_intrinsics.image_size[1];
    
     for (int idx = ZERO_PROF; idx <= THREE_PROF; idx++)
     {
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

          fp.invoke(xyz_frame.getPoints(), ab_frame.getPixels());
     }
     bool full_path = false;
     std::vector<std::string> filenames;
     rs::utils::filesystem::ListFilesInDirectoryWithExtension(outDir,
                                                              "png",
                                                              filenames,
                                                              full_path);
     int expectedNumberFiles = 0;
     int actualNumerFiles = filenames.size();
     EXPECT_EQ(actualNumerFiles, expectedNumberFiles);
     filenames.clear();
     rs::utils::filesystem::ListFilesInDirectoryWithExtension(outDir,
                                                              "ply",
                                                              filenames,
                                                              full_path);
     actualNumerFiles = filenames.size();
     expectedNumberFiles=0;
     EXPECT_EQ(actualNumerFiles, expectedNumberFiles);

     std::remove(tmpConfFile.c_str());
     removeDirectory(outDir);
     LOG(INFO) << "CheckGenerateOutputTest test checkGenerateOutputOnlyPng  end";
}
