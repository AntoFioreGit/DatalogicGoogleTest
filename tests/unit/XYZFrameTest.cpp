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

using namespace rs;

class XYZFrameTest : public ::testing::Test
{
public:
     static std::string getFileTestRaw() { return _fileTestRaw; }
     static std::string getconfigFile() { return _fullNameConfig; }

protected:
     void SetUp() override
     {
          std::string homeDir = std::getenv("HOME");

          if (!_fileTestRaw.size())
          {              
               _fileTestRaw = findFileRecursively(homeDir, nameFileTestRaw);
          }
          if (!_fullNameConfig.size())
          {              
               _fullNameConfig = findFileRecursively(homeDir, nameCalibConf);
          }
          return;
     }
     void TearDown() override {}

protected:
     static std::string _fileTestRaw;
     static std::string _fullNameConfig;
     rs::ImageFrame _if;
};

std::string XYZFrameTest::_fileTestRaw = "";
std::string XYZFrameTest::_fullNameConfig = "";

TEST_F(XYZFrameTest, getProfileEmpty)
{

     LOG(INFO) << "XYZFrameTest test getProfileEmpty  start";
     std::string pathFile = XYZFrameTest::getFileTestRaw();

      FrameProcessor fp;
       bool result = fp.initialize(_fullNameConfig);
     bool excpcted = false;
     EXPECT_EQ(result, excpcted);
     if ( result ) {

          
     }



     //TODO

     unsigned int size = 512 * 512 * 2;

     std::vector<char> vectData(size);
     std::ifstream file(pathFile, std::ios::binary);
     if (!file.is_open())
     {
          LOG(INFO) << "Error Open file " << pathFile;
          return;
     }
     file.read(vectData.data(), size);
     LOG(INFO) << "File  = " << pathFile << " Size = " << vectData.size();
     file.close();

     _if.prepare(512, 512);
     std::vector<uint16_t> vectDataIn((uint16_t *)vectData.data(), (uint16_t *)vectData.data() + vectData.size() / 2);
     _if.copyPixels((uint16_t *)vectDataIn.data(), vectDataIn.size());
     std::vector<uint16_t> vectDataOut(_if.getPixels(), _if.getPixels() + 512 * 512);
     EXPECT_EQ(vectDataIn.size(), vectDataOut.size());
     if (vectDataIn.size() == vectDataOut.size())
     {
          for (int idx = 0; idx < vectDataOut.size(); idx++)
          {
               EXPECT_EQ(vectDataIn.at(idx), vectDataOut.at(idx));
               if (vectDataIn.at(idx) != vectDataOut.at(idx))
                    break;
          }
     }
     cv::Mat mat;
     _if.convertToMat(mat);
     auto aspetc = mat.size().aspectRatio();
     auto height = mat.size().height;
     auto width = mat.size().width;
     EXPECT_EQ(aspetc, 1);
     EXPECT_EQ(height, 512);
     EXPECT_EQ(width, 512);
     rs::Rect roi{0, 0, 512, 512};
     const std::vector<size_t> scan_lines{3};
     std::string nameExportPng = "Image_" + getTimeStamp() + ".png";
    result = _if.exportToPng(nameExportPng, roi, scan_lines);
     bool excepted = true;
     EXPECT_EQ(result, excepted);
     LOG(INFO) << "XYZFrameTest test getProfileEmpty  end";

 }
