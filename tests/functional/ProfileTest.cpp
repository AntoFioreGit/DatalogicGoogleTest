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
TEST_F(ProfileTest, demoProfileTest)
{
     std::string homeDir = std::getenv("WORKSPACE") != nullptr ? std::getenv("WORKSPACE") : std::getenv("HOME");
     if (homeDir.size() == 0)
     {
          LOG(ERROR) << "Directory non valid ";
          return;
     }

     // Load List test.
     if (!ConfigManagerTest::getInstance().loadListTests())
     {
          LOG(ERROR) << "Fail Load Configuration ";
          return;
     }

     // for each configurated test, check all tests of type Profile
     for (auto test : ConfigManagerTest::getInstance().getTestsMap())
     {
          if (test.second.type() == TYPE_PROFILE)
          {
               // if the current test is Profile test than run it
               for (int currentStep = 0; currentStep < test.second.numberExcution(); currentStep++)
               {
                    // Verify for the current step if there are parameters must be updated into the file specified into the section Rs_AlgoConf_File.
                    // Build a list of parameters
                    std::set<Keyonfig> keys;
                    findUpdRsAlgo(test.second.testId(), currentStep + 1, keys);

                    // Verify for the current step if there are parameters must be updated into the file specified into the section Calib_File. if there are
                    // parameters must be updated, than a new temporary calibration file is created, else read the file specified in the section
                    bool isRemoveCalibTmpFile = false;
                    auto tmpCalibFile = verifyAndUpdCalibrationFile(test.second.testId(), currentStep + 1, homeDir, isRemoveCalibTmpFile);
                    bool expected = true;
                    bool result = true;

                    // Create a temporary rs-algorithms configuration file. This file is a copy of file specified in the section Rs_AlgoConf_File.
                    // The file will eventually also contain the parameters that need to be updated for the current step.
                    // This file is the configuration file for the library
                    auto ret = buildTmpAlgoFile(homeDir, test.second.rsAlgoConfFile(), &keys);
                    std::string tmpConfFile = ret.first;
                    std::string outDir = ret.second;
                    if (!tmpConfFile.size())
                         continue;

                    // Get the absolute path for the file specified in the section Calib_File. This file contains the intrinsic paramewters of device.
                    // This file is used by the library for the internal settings parameters
                    std::string pathIntCalXYZFile = findFileRecursively(homeDir, test.second.intrinsicFile());
                    rs::Intrinsics cam_intrinsics;
                    result = rs::io::readCameraIntrinsics(pathIntCalXYZFile, 3, cam_intrinsics); // 3 = ADTF "lr-qnative"
                    EXPECT_EQ(result, expected);
                    if (!result)
                    {
                         LOG(ERROR) << "Error on load camera intrinsics from file " << pathIntCalXYZFile;
                         return;
                    }
                    // Read the calibration file. This file, if there are not updated  paramaters (for current step) in the relative section, is the file
                    // specified int the section Calib_File, otherwise is a temporary file

                    rs::ConveyorCalibrationParameters calib_params;
                    if (!getCalibParameter(homeDir, tmpCalibFile, calib_params))
                    {
                         continue;
                    }
                    // Instantiate the frame processor and intitialze it with the configuration file
                    FrameProcessor fp;
                    result = fp.initialize(tmpConfFile);
                    expected = true;
                    EXPECT_EQ(result, expected);
                    if (!result)
                    {
                         LOG(ERROR) << "Error on load configuration  file  " << tmpConfFile;
                         continue;
                    }
                    // Configure the frame processor whit the calibibration parameters and the intrinsic cam parameters
                    fp.configure(calib_params, cam_intrinsics);
                    std::vector<char> frame_buffer;
                    int frame_width = cam_intrinsics.image_size[0];
                    int frame_height = cam_intrinsics.image_size[1];
                    //Number of element  for  each profile. Each profile has a first and  last point. Each point has three coordinate (x,y,z)
                    int NUMBER_POINT_4_PROFILE = (3 * 2);
                    XyzFrame xyz_frame;

                    // Get the absolute path for the file specified in the section XYZ_File. This file represents the points cloud  of the range sensor acquisition
                    std::string xyz_dataFile = findFileRecursively(homeDir, test.second.xyzFile());
                    // Read the file
                    result = rs::utils::filesystem::ReadFileToBuffer(xyz_dataFile, frame_buffer);
                    expected = true;
                    EXPECT_EQ(result, expected);
                    if (!result)
                    {
                         LOG(ERROR) << "Error on load XYZ data file  " << xyz_dataFile;
                         return;
                    }

                    // Call the some method (istance XyzFrame ) to prepare the result
                    xyz_frame.prepare(frame_height, frame_width);
                    xyz_frame.copyPoints(reinterpret_cast<int16_t *>(frame_buffer.data()), frame_height * frame_width);

                    // Call the invoke method (istance Frame Processor) and get the number profiles
                    size_t num_profiles = fp.invoke(xyz_frame.getPoints(), nullptr);

                    // ****** Chek the results test   -- begin section ******

                    // Get a list with all paramaters and values expected for this test and current step for the validation(PAASED -FAIL) test
                    // In this implementation there is one parameter (NumberProfiles)
                    auto eRList = getExpectedResultsList(test.second.testId(), currentStep + 1);/**/
                    int expectedNuberProfile = 0;/**/
                    bool found = false;
                    for (auto itemRes : eRList)
                    {
                         if (itemRes != nullptr)
                         {
                              if (itemRes->expectedParamName() == PARAM_CHECK_PROFILE_NAME)
                              {
                                   expectedNuberProfile = std::stoi(itemRes->expectedParamValue());
                                   found = true;
                                   break;
                              }
                         }
                    }
                    if (!found)
                    {
                         LOG(ERROR) << "Expected  Result not Configurated for step Number" << currentStep + 1;
                         continue;
                    }
                    // Check the number of profilis
                    EXPECT_EQ(num_profiles, expectedNuberProfile);
                    std::vector<std::vector<float>> xyz_lines;
                    // Get the list of points (first and last point. Point in coodinate x,y, z ) for each profile
                    xyz_lines = fp.getProfiles();
                    int countPointValidResult = 0;
                    // count for each profile (if found ) the number of coordinate (x,y,z) not equal to zero
                    for (auto xyz_line : xyz_lines)
                    {
                         countPointValidResult += std::count_if(xyz_line.begin(), xyz_line.end(), [](int x)
                                                                { return x != 0; });
                    }
                    // Check the number of points for all profilis
                    int expectedXYZPoints = expectedNuberProfile * NUMBER_POINT_4_PROFILE;
                    EXPECT_EQ(countPointValidResult, expectedXYZPoints);

                    // ****** Chek the results test   -- end section ******

                    // ***** remove temporary directory and files -- begin section ******
                    std::remove(tmpConfFile.c_str());
                    removeDirectory(outDir);
                    if (isRemoveCalibTmpFile)
                    {
                         std::remove(tmpCalibFile.c_str());
                    }
                    // ***** remove temporary directory and files -- end  section ******
               }
          }
     }
}
