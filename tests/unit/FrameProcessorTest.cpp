#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "frame_processor.h"
#include "io/file_json.h"
#include "utils/filesytem_helpers.h"
#include "algorithms/profile3d_calculator.h"
#include "calib_io.h"
#include "parameters.h"
#include <array>
#include "CommonTest.h"
#include "Util.h"
using namespace rs;
class FrameProcessorTest : public ::testing::Test
{
public:
     static std::string getconfigFile() { return _fullNameConfig; }
     static std::string getfile3DCalcConfig() { return _fullnameprofile3DCalcConfig; }

protected:
     FrameProcessor _fp;

     void SetUp() override
     {
           std::string homeDir = std::getenv("HOME");;
          if (!_fullNameConfig.size())
          {
              // fs::path homeDir = std::getenv("HOME");
               _fullNameConfig = findFileRecursively(homeDir, nameCalibConf);
          }
           if (!_fullnameprofile3DCalcConfig.size())
          {
              // fs::path homeDir = std::getenv("HOME");
               _fullnameprofile3DCalcConfig = findFileRecursively(homeDir, nameprofile3DCalcConf);
          }

          return;
     }
     void TearDown() override {}

private:
     static std::string _fullNameConfig;
     static std::string _fullnameprofile3DCalcConfig;
};
std::string FrameProcessorTest::_fullNameConfig = "";
std::string FrameProcessorTest::_fullnameprofile3DCalcConfig = "";

TEST_F(FrameProcessorTest, loadConfig)
{

     LOG(INFO) << "FrameProcessor test Load config  begin";
     std::string config_file = "not_valid_config_file";
     bool result = _fp.loadConfig(config_file);
     bool excpcted = false;
     EXPECT_EQ(result, excpcted);

    // config_file = fullNameConfig;
    config_file = FrameProcessorTest::getconfigFile();
     result = _fp.loadConfig(config_file);
     excpcted = true;
     EXPECT_EQ(result, excpcted);
     LOG(INFO) << "FrameProcessor test Load config  end";
}
TEST_F(FrameProcessorTest, loadAlgoParameters)
{

     LOG(INFO) << "FrameProcessor test Load Algorithms parameter  begin";
     std::string config_file = "not_valid_config_file";
     std::shared_ptr<rs::ParametersContainer> params(new Profile3dCalculatorParameters());
     bool result = _fp.loadAlgoParameters(config_file, params);
     bool excpcted = false;
     EXPECT_EQ(result, excpcted);
   //  config_file = profile3DCalcConfig;
     config_file= FrameProcessorTest::getfile3DCalcConfig();
     result = _fp.loadAlgoParameters(config_file, params);
     excpcted = true;
     EXPECT_EQ(result, excpcted);
     excpcted = true;
     result = static_cast<bool>(params->getParameter("static_roi").query());
     excpcted = static_roiConfig;
     EXPECT_EQ(result, excpcted);
     auto scanline_spacingValue = static_cast<int>(params->getParameter("scanline_spacing").query());
     excpcted = true;
     EXPECT_EQ(scanline_spacingValue == scanLine_spacingConfig, excpcted);
     auto num_scanlinesValue = static_cast<int>(params->getParameter("num_scanlines").query());
     excpcted = true;
     EXPECT_EQ(num_scanlinesValue == num_scalelineConfig, excpcted);
     result = typeid(InvalidParameter) == typeid(params->getParameter(""));
     excpcted = true;
     EXPECT_EQ(result, excpcted);
     LOG(INFO) << "FrameProcessor test Load Algorithms parameter  end";
}

TEST_F(FrameProcessorTest, initialize)
{

     // EXPECT_EQ(operation.somma(1,2), 3);
     bool value = true;
     EXPECT_EQ(true, true);
}
TEST_F(FrameProcessorTest, configure)
{

     LOG(INFO) << "FrameProcessor test configure  begin";
    // std::string conv_calib_file = calibConfig;
      std::string conv_calib_file= FrameProcessorTest::getconfigFile();
     bool excpcted = true;
     ConveyorCalibrationParameters calib_params;
     bool result = rs::io::readConveyorCalibration(conv_calib_file, calib_params);
     EXPECT_EQ(result, excpcted);

     FrameProcessor processor;
     rs::Intrinsics cam_intrinsics;

     // result=processor.configure(calib_params, cam_intrinsics);

     LOG(INFO) << "FrameProcessor test configure  begin";
}
TEST_F(FrameProcessorTest, invoke)
{

     LOG(INFO) << "FrameProcessor test invoke  begin";
     bool excpcted = true;
     bool result = true;
     auto vaule = _fp.invoke(nullptr, nullptr);
     EXPECT_EQ(vaule == 0, excpcted);
     LOG(INFO) << "FrameProcessor test invoke  end";
     // TODO

     // try {

     //       std::array<short int, 4> container{1, 2, 3, 4};
     //        std::array<unsigned short int, 10> container1{1, 2, 3, 4,5,6,7,8,9,10};
     //       vaule= _fp.invoke(container.data(), container1.data());

     // } catch (...) {
     //      int x;
     // }
}

TEST_F(FrameProcessorTest, getProfiles)
{

     // EXPECT_EQ(operation.somma(1,2), 3);
     LOG(INFO) << "FrameProcessor test getProfiles  begin";
     bool excpcted = true;
     bool result = true;
     auto vaule = _fp.getProfiles();
     EXPECT_EQ(vaule.size() == 0, excpcted);
     LOG(INFO) << "FrameProcessor test getProfiles  end";
     // TODO other case
}
