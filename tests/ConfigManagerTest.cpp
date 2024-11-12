#include "ConfigManagerTest.h"
#include "Util.h"
#include "json.h"
#include "file_json.h"
ConfigManagerTest::ConfigManagerTest() : _isFirstTime(true)
{
}
bool ConfigManagerTest::loadConfiguration()
{
    bool ret = true;

    if (_isFirstTime)
    {
        std::string homeDir = std::getenv("WORKSPACE") != nullptr ? std::getenv("WORKSPACE") : std::getenv("HOME");
        if (homeDir.size())
        {
            auto fullPath = findFileRecursively(homeDir, NAME_FILE_CONF);
            nlohmann::json json_value;
            ret = rs::io::readJsonFile(fullPath, json_value);
            if (ret)
            {
                _isFirstTime = false;
                std::cout << "Name Configuration:" << json_value["Name_Configurarion"] << std::endl;

                for (auto &json_item : json_value["Tests"])
                {
                    Test t;
                    t.setNumberExwcution(json_item["NumberExectution"]);
                    
                    std::cout << "Test id:" << json_item["Test_Id"] << std::endl;
                    std::cout << "XYZ_File:" << json_item["XYZ_File"] << std::endl;
                    std::cout << "Profile3d_File:" << json_item["Profile3d_File"] << std::endl;
                    std::cout << "IntrisicParam_File:" << json_item["IntrisicParam_File"] << std::endl;
                    std::cout << "NumberExectution:" << json_item["NumberExectution"] << std::endl;
                    std::cout << " *** Expected Result ***" << std::endl;
                    for (auto &json_item_expected : json_item["Expected_Results"])
                    {
                        std::cout << "Current_step:" << json_item_expected["Current_Step"] << std::endl;
                        std::cout << "NumberProfiles:" << json_item_expected["NumberProfiles"] << std::endl;
                        std::cout << "NumebeScanLine:" << json_item_expected["NumebeScanLine"] << std::endl;
                    }
                    for (auto &json_item_update : json_item["Update_Parameter"])
                    {
                        std::cout << "File:" << json_item_update["File"] << std::endl;
                        std::cout << "Step:" << json_item_update["Step"] << std::endl;
                        std::cout << "Parameter:" << json_item_update["Parameter"] << std::endl;
                        std::cout << "Value:" << json_item_update["Value"] << std::endl;
                    }
                }
            }
            else
            {
                // TODO LOG Errorr
            }
        }
    }

    return ret;
}
void ConfigManagerTest::dumpConfiguration()
{

    std::cout << "*** Dump Configuration ***" << std::endl;
}
  void ConfigManagerTest::addTest(std::string key, Test t){
    _testMap[key]=t;
  }
 void ConfigManagerTest::addExpectedRes(std::string key,ExpectedResults er){

    if  ( _expecteResultrMap.find(key) != _expecteResultrMap.end() ){

        _expecteResultrMap[key].push_back(er);

    } else {
        std::list <ExpectedResults> erList {er};
        _expecteResultrMap[key]=erList;

    }

 }
 void ConfigManagerTest::addUpdPar(std::string key,UpdateParameterTest up){

     if  ( _updParMap.find(key) != _updParMap.end() ){
        _updParMap[key].push_back(up);
    } else {
        std::list <UpdateParameterTest> upList {up};
        _updParMap[key]=upList;
    }

 }
 std::list <UpdateParameterTest> ConfigManagerTest::getUpParameter(std::string key){

     if  ( _updParMap.find(key) != _updParMap.end() ){
       return _updParMap[key];
    }
    return std::list<UpdateParameterTest>{};

 }
 std::list <ExpectedResults> ConfigManagerTest::getExpectedRes(std::string key){

     if  ( _expecteResultrMap.find(key) != _expecteResultrMap.end() ){
       return _expecteResultrMap[key];
    }
    return std::list<ExpectedResults>{};
 }
  std::map<std::string,Test>& ConfigManagerTest::getTestsMap() {
    return _testMap;
  }