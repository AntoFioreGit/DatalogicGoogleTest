#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "frame_processor.h"
#include "io/file_json.h"
#include "utils/filesytem_helpers.h"
#include "algorithms/profile3d_calculator.h"
#include "calib_io.h"
#include "parameters.h"
#include "file_json.h"
#include <array>
#include "CommonTest.h"
#include "Util.h"
using namespace rs;
class FrameProcessorTest : public ::testing::Test
{
public:
     static std::string getconfigFile() { return _fullNameConfig; }
     static std::string getfile3DCalcConfig() { return _fullnameprofile3DCalcConfig; }
     static std::string getAlgoFile() { return _algoConf; }
     static std::string getFileIntCalib() { return _intrCalConf; }
     static std::string getFileCalib() { return _caliConf; }

protected:
     // FrameProcessor _fp;

     void SetUp() override
     {
          std::string homeDir = std::getenv("HOME");
          ;
          if (!_fullNameConfig.size())
          {

               _fullNameConfig = findFileRecursively(homeDir, nameCalibConf);
          }
          if (!_fullnameprofile3DCalcConfig.size())
          {

               _fullnameprofile3DCalcConfig = findFileRecursively(homeDir, nameprofile3DCalcConf);
          }
          if (!_algoConf.size())
          {
               _algoConf = findFileRecursively(homeDir, nameAlgoConf);
          }
          if (!_intrCalConf.size())
          {
               _intrCalConf = findFileRecursively(homeDir, nameIntrCalXYZFrameConf);
          }
          if (!_caliConf.size())
          {
               _caliConf = findFileRecursively(homeDir, nameCalibXYZFrameConf);
          }

          return;
     }
     void TearDown() override {}

private:
     static std::string _fullNameConfig;
     static std::string _fullnameprofile3DCalcConfig;
     static std::string _algoConf;
     static std::string _intrCalConf;
     static std::string _caliConf;
};
std::string FrameProcessorTest::_fullNameConfig = "";
std::string FrameProcessorTest::_fullnameprofile3DCalcConfig = "";
std::string FrameProcessorTest::_algoConf = "";
std::string FrameProcessorTest::_intrCalConf = "";
std::string FrameProcessorTest::_caliConf = "";

std::string generateNotValidConfFile(std::string &confFile)
{
     std::string newFileGenerate;

     nlohmann::json json_value;
     bool result = rs::io::readJsonFile(confFile, json_value);
     if (!result)
     {
          LOG(ERROR) << "Error on read file : " << confFile;
          return newFileGenerate;
     }

     std::string absolutePath = getDirectoryPath(confFile);
     std::string nameTmpConf = std::string("Err_") + nameAlgoConf;
     newFileGenerate = absolutePath + "/" + nameTmpConf;
     json_value["algo"][0]["id"] = "ParameterNotValid";
     std::ofstream o(newFileGenerate);
     o << std::setw(4) << json_value << std::endl;
     o.close();
     return newFileGenerate;
}

TEST_F(FrameProcessorTest, loadConfig)
{

     LOG(INFO) << "FrameProcessor test Load config  begin";
     FrameProcessor fp;
     std::string config_file = "not_valid_config_file";
     bool result = fp.loadConfig(config_file);
     bool excpcted = false;
     EXPECT_EQ(result, excpcted);

     config_file = FrameProcessorTest::getAlgoFile();
     result = fp.loadConfig(config_file);
     excpcted = true;
     EXPECT_EQ(result, excpcted);
     std::string errConfFile = generateNotValidConfFile(config_file);
     if (!errConfFile.size())
          return;
     result = fp.loadConfig(errConfFile);
     excpcted = false;
     EXPECT_EQ(result, excpcted);
     std::remove(errConfFile.c_str());
     LOG(INFO) << "FrameProcessor test Load config  end";
}
TEST_F(FrameProcessorTest, loadAlgoParameters)
{

     LOG(INFO) << "FrameProcessor test Load Algorithms parameter  begin";
     FrameProcessor fp;
     std::string config_file = "not_valid_config_file";
     std::shared_ptr<rs::ParametersContainer> params(new Profile3dCalculatorParameters());
     bool result = fp.loadAlgoParameters(config_file, params);
     bool excpcted = false;
     EXPECT_EQ(result, excpcted);
     config_file = FrameProcessorTest::getfile3DCalcConfig();
     result = fp.loadAlgoParameters(config_file, params);
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
     FrameProcessor fp;
     bool excpcted = true;
     LOG(INFO) << "FrameProcessor test initialize start";
     auto config_file = FrameProcessorTest::getAlgoFile();
     bool result = fp.initialize(config_file);
     EXPECT_EQ(result, excpcted);
     std::string errConfFile = generateNotValidConfFile(config_file);
     if (!errConfFile.size())
          return;

     result = fp.initialize(errConfFile);
     excpcted = false;
     EXPECT_EQ(result, excpcted);

     std::remove(errConfFile.c_str());
     LOG(INFO) << "FrameProcessor test initialize end";
}
TEST_F(FrameProcessorTest, configure)
{

     LOG(INFO) << "FrameProcessor test configure  begin";
     FrameProcessor fp;
     std::string intCalib_file = FrameProcessorTest::getFileIntCalib();
     bool excpcted = true;

     rs::Intrinsics cam_intrinsics;
     bool result = rs::io::readCameraIntrinsics(intCalib_file, 3, cam_intrinsics); // 3 = ADTF "lr-qnative"
     EXPECT_EQ(result, excpcted);
     if (!result)
     {
          LOG(ERROR) << "Error on load camera intrinsics from file " << intCalib_file;
          return;
     }
     std::string calibFile = FrameProcessorTest::getFileCalib();
     rs::ConveyorCalibrationParameters calib_params;
     result = rs::io::readConveyorCalibration(calibFile, calib_params);
     excpcted = true;
     EXPECT_EQ(result, excpcted);
     if (!result)
     {
          LOG(ERROR) << "Error on load  target calibration from file  " << calibFile;
          return;
     }
     std::string algoFile = FrameProcessorTest::getAlgoFile();
     std::string tmpConfFile = generateTempConf1(algoFile);
     if (!tmpConfFile.size())
          return;

     result = fp.initialize(tmpConfFile);
     excpcted = true;
     EXPECT_EQ(result, excpcted);
     if (!result)
     {
          LOG(ERROR) << "Error on load configuration  file  " << algoFile;
          return;
     }
     result = fp.configure(calib_params, cam_intrinsics);
     excpcted = true;
     EXPECT_EQ(result, excpcted);

     auto fx_orig = cam_intrinsics.focal_length[0];
     auto fy_orig = cam_intrinsics.focal_length[1];

     cam_intrinsics.focal_length[0] = -1.0;
     cam_intrinsics.focal_length[1] = -1.0;

     result = fp.configure(calib_params, cam_intrinsics);
     excpcted = false;
     EXPECT_EQ(result, excpcted);

     cam_intrinsics.focal_length[0] = fx_orig;
     cam_intrinsics.focal_length[1] = fy_orig;

     auto cx_orig = cam_intrinsics.principal_point[0];
     auto cy_orig = cam_intrinsics.principal_point[1];

     cam_intrinsics.principal_point[0]=-20;
     cam_intrinsics.principal_point[1]=-60;

     result = fp.configure(calib_params, cam_intrinsics);
     excpcted = false;
     EXPECT_EQ(result, excpcted);

     LOG(INFO) << "FrameProcessor test configure  begin";
}
TEST_F(FrameProcessorTest, invoke)
{

     LOG(INFO) << "FrameProcessor test invoke  begin";
     bool excpcted = true;
     bool result = true;
     // auto vaule = _fp.invoke(nullptr, nullptr);
     // EXPECT_EQ(vaule == 0, excpcted);
     // LOG(INFO) << "FrameProcessor test invoke  end";
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
     // auto vaule = _fp.getProfiles();
     // EXPECT_EQ(vaule.size() == 0, excpcted);
     // LOG(INFO) << "FrameProcessor test getProfiles  end";
     // TODO other case
}
