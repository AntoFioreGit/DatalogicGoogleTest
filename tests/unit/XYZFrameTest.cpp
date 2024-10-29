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

using namespace rs;

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

std::string generateTempConf(std::string& confFile) {
     std::string newFileGenerate;

      nlohmann::json json_value;
     bool result = rs::io::readJsonFile(confFile, json_value);
     if (! result ) {
           LOG(ERROR) << "Error on read file : " << confFile;
           return newFileGenerate;
     }

     std::string absolutePath = getDirectoryPath(confFile);
     std::string nameTmpConf = std::string("Tmp_")+nameAlgoConf;
     newFileGenerate = absolutePath +"/"+ nameTmpConf;
     json_value["algo"][0]["config_file"]=absolutePath+"/"+nameprofile3DCalcConf;
     std::ofstream o(newFileGenerate);
      o << std::setw(4) << json_value << std::endl;
     o.close();
     return newFileGenerate;
}
TEST_F(XYZFrameTest, getProfileEmpty)
{

     LOG(INFO) << "XYZFrameTest test getProfileEmpty  start";
     std::string pathAlgoFile = XYZFrameTest::getAlgoFile();
     std::string pathCalibXYZFile = XYZFrameTest::getFileCalibXYZFrame();
     std::string pathIntCalXYZFile = XYZFrameTest::getFileIntCalibXYZFrame();
     bool expected = true;
     bool result = true;

     std::string tmpConfFile = generateTempConf(pathAlgoFile);
     if (! tmpConfFile.size())
          return ;

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

     // Active Brightness
     ImageFrame ab_frame;
     std::vector<char> ab_buffer;
     std::string ab_dataFile = findFileRecursively(homeDir, ab_DataMap[ONE_PROF]);

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

     std::string xyz_dataFile = findFileRecursively(homeDir, xyz_DataMap[ONE_PROF]);

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
     std::remove(tmpConfFile.c_str());

     LOG(INFO) << "XYZFrameTest test getProfileEmpty  end";
}
