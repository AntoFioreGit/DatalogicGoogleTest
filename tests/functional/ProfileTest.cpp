#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <iostream>
// #include <chrono>
// #include <ctime>
#include <sstream>
#include "CommonTest.h"
#include "Util.h"
#include "frame.h"
#include <fstream>
#include <vector>
#include "types.h"
// #include "geometry.h"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace rs;

#include "ConfigManagerTest.h"
#include "json.h"
#include "file_json.h"
#include "filesytem_helpers.h"
#include "calib_io.h"
#include "frame_processor.h"
class ProfileTest : public ::testing::Test
{
public:
protected:
     void SetUp() override
     {
     }
     void TearDown() override {}

protected:
};

ExpectedResults *getExpectedResults(std::string idTest, int step)
{
     ExpectedResults *result = nullptr;

     auto listResult = ConfigManagerTest::getInstance().getExpectedRes(idTest);
     for (ExpectedResults &itemResult : listResult)
     {
          if (itemResult.currentStep() == step)
          {
               result = &itemResult;
               break;
          }
     }
     return result;
}

std::string verifyAndUpdCalibrationFile(std::string idTest, int currentStep, std::string homeDir,bool &isRemoveFile)
{
     isRemoveFile=false;
     std::string fullNameCalib = "";
     std::string tmpNameFile;
     std::map<KeyCalibPar, float> keyVal;
     auto listUpd = ConfigManagerTest::getInstance().getUpParameter(idTest);
     if (listUpd.size() != 0)
     {
          if (listUpd.front().typeFile() == TYPE_FILE_CALIB_PR)
          {
               tmpNameFile = listUpd.front().file();
               for (auto &itemUpdCalib : listUpd)
               {
                    if (itemUpdCalib.currentStep() == currentStep)
                    {
                         getUpdCalibParameter(itemUpdCalib.parameterName(), itemUpdCalib.value(), keyVal);
                    }
               }
          }
     }
     if (keyVal.size() == 0)
     {
          auto testMap = ConfigManagerTest::getInstance().getTestsMap();
          if (testMap.find(idTest) == testMap.end())
          {
               std::cout << "Error test not found...." << std::endl;
          }
          else
          {

               fullNameCalib = findFileRecursively(homeDir, testMap[idTest].calibFile());
          }
     }
     else
     {
          tmpNameFile = findFileRecursively(homeDir, tmpNameFile);
          fullNameCalib = generateTempCalibParam(tmpNameFile, keyVal);
          isRemoveFile=true;
     }

     return fullNameCalib;
}

std::pair<std::string, std::string> buildTmpAlgoFile(std::string homeDir, std::string nameAlgoFile)
{
     std::pair<std::string, std::string> retValue{"", ""};
     std::string pathAlgoFile = findFileRecursively(homeDir, nameAlgoFile);

     std::set<Keyonfig> keys{ALGO, OUTDIR};
     std::string tmpConfFile = generateTempConf(pathAlgoFile, keys);
     if (!tmpConfFile.size())
          return retValue;
     nlohmann::json json_value;
     bool result = rs::io::readJsonFile(tmpConfFile, json_value);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration file " << tmpConfFile;
          return retValue;
     }
     std::string outDir = json_value["save"]["out_dir"];
     rs::utils::filesystem::MakeDirectory(outDir);
     retValue.first = tmpConfFile;
     retValue.second = outDir;

     return retValue;
}
bool getCalibParameter(std::string homeDir, std::string calibXYZFile, rs::ConveyorCalibrationParameters &calib_params)
{
     bool ret = true;
     std::string pathCalibXYZFile = findFileRecursively(homeDir, calibXYZFile);
     ret = rs::io::readConveyorCalibration(pathCalibXYZFile, calib_params);
     if (!ret)
     {
          LOG(ERROR) << "Error on load  target calibration from file  " << pathCalibXYZFile;
          return false;
     }
     return true;
}
TEST_F(ProfileTest, debugReadCfg)
{
     std::string homeDir = std::getenv("WORKSPACE") != nullptr ? std::getenv("WORKSPACE") : std::getenv("HOME");
     if (homeDir.size() == 0)
     {
          LOG(ERROR) << "Directory non valid ";
          return;
     }
     ConfigManagerTest::getInstance().loadConfiguration();

     for (auto test : ConfigManagerTest::getInstance().getTestsMap())
     {
          if (test.second.type() == TYPE_PROFILE)
          {
               for (int currentStep = 0; currentStep < test.second.numberExcution(); currentStep++)
               {
                    
                    bool isRemoveCalibTmpFile=false;
                    auto tmpCalibFile = verifyAndUpdCalibrationFile(test.second.testId(), currentStep + 1, homeDir,isRemoveCalibTmpFile);
                    bool expected = true;
                    bool result = true;

                    auto ret = buildTmpAlgoFile(homeDir, test.second.rsAlgoConfFile());
                    std::string tmpConfFile = ret.first;
                    std::string outDir = ret.second;
                    if (!tmpConfFile.size())
                         continue;
                    // Get Algo File
                    /*  std::string pathAlgoFile = findFileRecursively(homeDir, test.second.rsAlgoConfFile());
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
                      */

                    std::string pathIntCalXYZFile = findFileRecursively(homeDir, test.second.intrinsicFile());
                    rs::Intrinsics cam_intrinsics;
                    result = rs::io::readCameraIntrinsics(pathIntCalXYZFile, 3, cam_intrinsics); // 3 = ADTF "lr-qnative"
                    EXPECT_EQ(result, expected);
                    if (!result)
                    {
                         LOG(ERROR) << "Error on load camera intrinsics from file " << pathIntCalXYZFile;
                         return;
                    }
                    /*
                    std::string pathCalibXYZFile = findFileRecursively(homeDir, test.second.calibFile());
                    rs::ConveyorCalibrationParameters calib_params;
                    result = rs::io::readConveyorCalibration(pathCalibXYZFile, calib_params);
                    EXPECT_EQ(result, expected);
                    if (!result)
                    {
                         LOG(ERROR) << "Error on load  target calibration from file  " << pathCalibXYZFile;
                         return;
                    }*/
                    rs::ConveyorCalibrationParameters calib_params;
                    getCalibParameter(homeDir, test.second.calibFile(), calib_params);

                    FrameProcessor fp;
                    result = fp.initialize(tmpConfFile);
                    expected = true;
                    EXPECT_EQ(result, expected);
                    if (!result)
                    {
                         LOG(ERROR) << "Error on load configuration  file  " << tmpConfFile;
                         continue;
                    }
                    fp.configure(calib_params, cam_intrinsics);
                    std::vector<char> frame_buffer;
                    int frame_width = cam_intrinsics.image_size[0];
                    int frame_height = cam_intrinsics.image_size[1];
                    int NUMBER_POINT_4_PROFILE = (3 * 2);
                    XyzFrame xyz_frame;
                    std::string xyz_dataFile = findFileRecursively(homeDir, test.second.xyzFile());
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

                    size_t num_profiles = fp.invoke(xyz_frame.getPoints(), nullptr);

                    auto expectedProfileResult = getExpectedResults(test.second.testId(), currentStep + 1);
                    if (expectedProfileResult == nullptr)
                    {

                         LOG(ERROR) << "Expected  Result not Configurated for step Number" << currentStep + 1;
                         continue;
                    }
                    EXPECT_EQ(num_profiles, expectedProfileResult->expectedNuberProfile());

                    std::vector<std::vector<float>> xyz_lines;
                    xyz_lines = fp.getProfiles();
                    int countPointValidResult = 0;
                    for (auto xyz_line : xyz_lines)
                    {
                         countPointValidResult += std::count_if(xyz_line.begin(), xyz_line.end(), [](int x)
                                                                { return x != 0; });
                    }
                    int expectedXYZPoints = expectedProfileResult->expectedNuberProfile() * NUMBER_POINT_4_PROFILE;
                    EXPECT_EQ(countPointValidResult, expectedXYZPoints);
                    std::remove(tmpConfFile.c_str());
                    removeDirectory(outDir);
                    if (isRemoveCalibTmpFile) {
                         std::remove(tmpCalibFile.c_str());
                         
                    }
               }
          }
     }
}
