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
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace rs;
#include "ConfigManagerTest.h"

class ImageFrameTest : public ::testing::Test
{
public:
     static std::string getFileTestRaw() { return _fileTestRaw; }

protected:
     void SetUp() override
     {
         std::string homeDir=std::getenv("WORKSPACE")!=nullptr?std::getenv("WORKSPACE"):std::getenv("HOME");
             
          if (!_fileTestRaw.size())
          {               
               _fileTestRaw = findFileRecursively(homeDir, nameFileTestRaw);
          }

          return;
     }
     void TearDown() override {}

protected:
     static std::string _fileTestRaw;
     rs::ImageFrame _if;
};

std::string ImageFrameTest::_fileTestRaw = "";

TEST_F(ImageFrameTest, debugReadCfg)
{
     ConfigManagerTest::getInstance().loadConfiguration();
}
TEST_F(ImageFrameTest, frameAll)
{

     LOG(INFO) << "ImageFrameTest test ReadCCB  start";
     std::string pathFile = ImageFrameTest::getFileTestRaw();

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
     bool result = _if.exportToPng(nameExportPng, roi, scan_lines);
     bool excepted = true;
     EXPECT_EQ(result, excepted);
      std::remove(nameExportPng.c_str());
     LOG(INFO) << "ImageFrameTest test ReadCCB  end";

 }
