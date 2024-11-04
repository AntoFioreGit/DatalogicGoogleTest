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
          std::string homeDir = std::getenv("HOME");

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
     std::string homeDir = std::getenv("HOME");
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

          if (0)
          {

               if (idx != THREE_PROF)
                    continue;

               float gamma = 0.5;
               std::vector<uint8_t> lut(256);
               for (int k = 0; k < 256; k++)
               {
                    lut[k] = cv::saturate_cast<uint8_t>(pow((k / 255.0f), gamma) * 255.0f);
               }
               ab_frame.prepare(1024, 512);
               cv::Mat ab;
               ab_frame.convertToMat(ab);
               rs::Rect roi{0, 0, 1024, 512};
               const std::vector<size_t> scan_lines{3};
               std::string nameExportPng = "PippoImage_" + getTimeStamp() + ".png";
               bool result = ab_frame.exportToPng(nameExportPng, roi, scan_lines);

               auto aspetc = ab.size().aspectRatio();
               auto height = ab.size().height;
               auto width = ab.size().width;

               //     cv::Mat norm_ab(ab.size(), CV_8U);
               //     cv::normalize(ab, norm_ab, 0, 255, cv::NORM_MINMAX, CV_8U);
               //     cv::LUT(norm_ab, lut, norm_ab);
               //     cv::namedWindow("AB", cv::WINDOW_AUTOSIZE);
               //     cv::imshow("AB", norm_ab);
               //     cv::waitKey(0);
               //     rs::ImageFrame _if;

               // _if.prepare(512,  512);
               //  _if.copyPixels((uint16_t *) ab_frame.getPixels(),frame_height * frame_width);

               // if (idx == THREE_PROF ){
               //       rs::ImageFrame _if;
               /*
               cv::Mat mat;
               _if.prepare(512,  512);
               _if.copyPixels((uint16_t *) xyz_frame.getPoints(),frame_height * frame_width);
                _if.convertToMat(mat);


                ;



               const std::vector<size_t> scan_lines{3};
                rs::Rect roi{0, 0, mat.size().width, mat.size().height};
                 std::string nameExportPng = "PippoImage_" + getTimeStamp() + ".png";

                bool  result = _if.exportToPng(nameExportPng, roi, scan_lines);
                */

               //  }
          }
          if (0) {
                 nlohmann::json json_value;
               bool result = rs::io::readJsonFile(tmpConfFile, json_value);
               std::string pathOut = json_value["save"]["out_dir"];
               bool full_path=false;
               std::vector<std::string> filenames;
                rs::utils::filesystem::ListFilesInDirectoryWithExtension(pathOut,
                                                   "png",
                                                   filenames,
                                                   full_path);
               int numb = filenames.size();
               std::cout <<  "numb= " << numb;
          }

          if (0)
          {
               std::set<Keyonfig> keys{ALGO, OUTDIR,DISABLE_SAVE_XYZ};
               std::string tmpConfFile = generateTempConf(pathAlgoFile, keys);
               if (!tmpConfFile.size())
                    return;

               nlohmann::json json_value;
               bool result = rs::io::readJsonFile(tmpConfFile, json_value);
               std::string pathOut = json_value["save"]["out_dir"];
               rs::utils::filesystem::MakeDirectory(pathOut);
               removeDirectory(pathOut);
          }
     }
     std::remove(tmpConfFile.c_str());
     removeDirectory(outDir);

     LOG(INFO) << "XYZFrameTest test checkProfiles  end";
}
