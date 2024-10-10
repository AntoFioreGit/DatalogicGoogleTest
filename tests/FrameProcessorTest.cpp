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
using namespace rs;
class FrameProcessorTest : public ::testing::Test
{
protected:
     FrameProcessor _fp;

     void SetUp() override {}
     void TearDown() override {}
};
TEST_F(FrameProcessorTest, loadConfig)
{

     LOG(INFO) << "FrameProcessor test Load config  begin";
     std::string config_file = "not_valid_config_file";
     bool result = _fp.loadConfig(config_file);
     bool excpcted = false;
     EXPECT_EQ(result, excpcted);

     config_file = "../../data/config/rs_algo_config.json";
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
     config_file = "../../data/config//profile3d_calculator.json";
     result = _fp.loadAlgoParameters(config_file, params);
     excpcted = true;
     EXPECT_EQ(result, excpcted);
     excpcted = true;
     result = static_cast<bool>(params->getParameter("static_roi").query());
     EXPECT_EQ(result, excpcted);
     auto scanline_spacingValue = static_cast<int>(params->getParameter("scanline_spacing").query());
     excpcted = true;
     EXPECT_EQ(scanline_spacingValue == 70, excpcted);
     auto num_scanlinesValue = static_cast<int>(params->getParameter("num_scanlines").query());
     excpcted = true;
     EXPECT_EQ(num_scanlinesValue == 3, excpcted);
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
     std::string conv_calib_file = "../../data/config/calib_data.json";
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
     //TODO

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
     //TODO other case
}

