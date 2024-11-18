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

#define PARAM_CHECK_PROFILE_NAME "NumberProfiles"


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

bool getCalibParameter(std::string homeDir, std::string calibXYZFile, rs::ConveyorCalibrationParameters &calib_params)
{
     bool ret = true;
     ret = rs::io::readConveyorCalibration(calibXYZFile, calib_params);
     if (!ret)
     {
          LOG(ERROR) << "Error on load  target calibration from file  " << calibXYZFile;
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

    if (  !  ConfigManagerTest::getInstance().loadListTests() ) {
                LOG(ERROR) << "Fail Load Configuration ";
                return ;
    }

     for (auto test : ConfigManagerTest::getInstance().getTestsMap())
     {
          if (test.second.type() == TYPE_PROFILE)
          {
               for (int currentStep = 0; currentStep < test.second.numberExcution(); currentStep++)
               {
                    std::set<Keyonfig> keys;
                    findUpdRsAlgo(test.second.testId(), currentStep + 1, keys);

                    bool isRemoveCalibTmpFile = false;
                    auto tmpCalibFile = verifyAndUpdCalibrationFile(test.second.testId(), currentStep + 1, homeDir, isRemoveCalibTmpFile);
                    bool expected = true;
                    bool result = true;

                    auto ret = buildTmpAlgoFile(homeDir, test.second.rsAlgoConfFile(),&keys);
                    std::string tmpConfFile = ret.first;
                    std::string outDir = ret.second;
                    if (!tmpConfFile.size())
                         continue;

                    std::string pathIntCalXYZFile = findFileRecursively(homeDir, test.second.intrinsicFile());
                    rs::Intrinsics cam_intrinsics;
                    result = rs::io::readCameraIntrinsics(pathIntCalXYZFile, 3, cam_intrinsics); // 3 = ADTF "lr-qnative"
                    EXPECT_EQ(result, expected);
                    if (!result)
                    {
                         LOG(ERROR) << "Error on load camera intrinsics from file " << pathIntCalXYZFile;
                         return;
                    }
                    rs::ConveyorCalibrationParameters calib_params;
                    if (!getCalibParameter(homeDir, tmpCalibFile, calib_params))
                    {
                         continue;
                    }

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
                    //Chek the result begin
                    auto eRList =  getExpectedResultsList(test.second.testId(), currentStep + 1);
                    int expectedNuberProfile=0;
                    bool found = false;
                    for (auto itemRes :eRList) {
                         if (itemRes != nullptr) {
                              if (itemRes->expectedParamName() == PARAM_CHECK_PROFILE_NAME){
                                  expectedNuberProfile=std::stoi(itemRes->expectedParamValue()) ;
                                  found=true;
                                  break;
                              }
                          }
                    }
                    if (!found ) {
                         LOG(ERROR) << "Expected  Result not Configurated for step Number" << currentStep + 1;
                         continue;                    
                    }
                    
                     EXPECT_EQ(num_profiles, expectedNuberProfile);
                      std::vector<std::vector<float>> xyz_lines;
                    xyz_lines = fp.getProfiles();
                    int countPointValidResult = 0;
                    for (auto xyz_line : xyz_lines)
                    {
                         countPointValidResult += std::count_if(xyz_line.begin(), xyz_line.end(), [](int x)
                                                                { return x != 0; });
                    }
                    int expectedXYZPoints =expectedNuberProfile * NUMBER_POINT_4_PROFILE;
                    EXPECT_EQ(countPointValidResult, expectedXYZPoints);





                    // auto expectedProfileResult = getExpectedResults(test.second.testId(), currentStep + 1);
                    // if (expectedProfileResult == nullptr)
                    // {

                    //      LOG(ERROR) << "Expected  Result not Configurated for step Number" << currentStep + 1;
                    //      continue;
                    // }
                    // EXPECT_EQ(num_profiles, expectedProfileResult->expectedNuberProfile());

                    // std::vector<std::vector<float>> xyz_lines;
                    // xyz_lines = fp.getProfiles();
                    // int countPointValidResult = 0;
                    // for (auto xyz_line : xyz_lines)
                    // {
                    //      countPointValidResult += std::count_if(xyz_line.begin(), xyz_line.end(), [](int x)
                    //                                             { return x != 0; });
                    // }
                    // int expectedXYZPoints = expectedProfileResult->expectedNuberProfile() * NUMBER_POINT_4_PROFILE;
                    // EXPECT_EQ(countPointValidResult, expectedXYZPoints);

                    //Chek the result end


                    std::remove(tmpConfFile.c_str());
                    removeDirectory(outDir);
                    if (isRemoveCalibTmpFile)
                    {
                         std::remove(tmpCalibFile.c_str());
                    }
               }
          }
     }
}
