#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "io/file_json.h"
#include "CommonTest.h"
#include "Util.h"
class File_JsonTest : public ::testing::Test
{
public:
     static std::string getPathFile() { return _fullNameConfig; }

protected:
     void SetUp() override
     {
          std::string homeDir = std::getenv("HOME");
          if (_fullNameConfig.size())
               return;
          // fs::path homeDir = std::getenv("HOME");
          _fullNameConfig = findFileRecursively(homeDir, nameAlgoConf);
          return;
     }
     void TearDown() override {}

private:
     static std::string _fullNameConfig;
};
std::string File_JsonTest::_fullNameConfig = "";
TEST_F(File_JsonTest, readFile)
{

     LOG(INFO) << "File_JsonTest test readFile  begin";
     bool excepted = false;
     std::string pathFile = "";
     nlohmann::json json_value;
     bool result = rs::io::readJsonFile(pathFile, json_value);
     EXPECT_EQ(result, excepted);
     excepted = true;
     // pathFile = fullNameConfig;
     pathFile = File_JsonTest::getPathFile();

     result = rs::io::readJsonFile(pathFile, json_value);
     EXPECT_EQ(result, excepted);
     LOG(INFO) << "File_JsonTest test readFile  end";
}

TEST_F(File_JsonTest, checkParameters)
{

     LOG(INFO) << "File_JsonTest test checkParameters  begin";
     bool excepted = true;
     // const std::string pathFile = fullNameConfig;
     const std::string pathFile = File_JsonTest::getPathFile();
     nlohmann::json json_value;
     bool result = rs::io::readJsonFile(pathFile, json_value);
     EXPECT_EQ(result, excepted);

     result = json_value.contains("version");
     excepted = true;
     EXPECT_EQ(result, excepted);
     if (result)
     {
          std::string versionValue = json_value["version"];
          excepted = true;
          EXPECT_EQ(versionValue == versionConfigValue, excepted);
     }
     result = json_value.contains("vlog_level");
     excepted = true;
     EXPECT_EQ(result, excepted);
     if (result)
     {

          int logLevelValue = json_value["vlog_level"];
          excepted = true;
          EXPECT_EQ(logLevelValue == logLevelConfigValue, excepted);
     }
     result = json_value.contains("num_cpu");
     excepted = true;
     EXPECT_EQ(result, excepted);
     if (result)
     {

          int numCpuValue = json_value["num_cpu"];
          excepted = true;
          EXPECT_EQ(numCpuValue == numCpuConfigValue, excepted);
     }

     result = json_value.contains("algo");
     excepted = true;
     EXPECT_EQ(result, excepted);
     if (result)
     {
          for (auto &json_item : json_value["algo"])
          {
               excepted = true;
               result = json_item.contains("id");
               EXPECT_EQ(result, excepted);
               if (!result)
                    break;
               std::string algoValue = json_item["id"];
               excepted = true;
               EXPECT_EQ(algoValue == algoConfigValue, excepted);

               excepted = true;
               result = json_item.contains("enable");
               EXPECT_EQ(result, excepted);
               if (!result)
                    break;
               bool enableValue = json_item["enable"];
               excepted = true;
               EXPECT_EQ(enableValue == enableConfigValue, excepted);

               excepted = true;
               result = json_item.contains("config_file");
               EXPECT_EQ(result, excepted);
               if (!result)
                    break;
               std::string fileConfigValue = json_item["config_file"];
               excepted = true;
               EXPECT_EQ(fileConfigValue == fileConfig, excepted);
               break;
          }
     }
     result = json_value.contains("save");
     excepted = true;
     EXPECT_EQ(result, excepted);
     if (result)
     {
          auto json_item = json_value["save"];
          result = json_item.contains("ab");
          EXPECT_EQ(result, excepted);
          if (result)
          {
               bool abValue = json_item["ab"];
               excepted = true;
               EXPECT_EQ(abValue == abConfig, excepted);
          }
          excepted = true;
          result = json_item.contains("xyz");
          EXPECT_EQ(result, excepted);
          if (result)
          {
               bool xyzValue = json_item["xyz"];
               excepted = true;
               EXPECT_EQ(xyzValue == xyzConfig, excepted);
          }
          excepted = true;
          result = json_item.contains("out_dir");
          EXPECT_EQ(result, excepted);
          if (result)
          {
               std::string outDirValue = json_item["out_dir"];
               excepted = true;
               EXPECT_EQ(outDirValue == outDirConfig, excepted);
          }
     }
     excepted = false;
     result = json_value.contains("not_exist");
     EXPECT_EQ(result, excepted);
    EXPECT_EQ(json_value["not_exist"],nullptr);
   

 
}
