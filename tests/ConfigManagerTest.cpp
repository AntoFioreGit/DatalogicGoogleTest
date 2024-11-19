#include "ConfigManagerTest.h"
#include "Util.h"

#include "file_json.h"
#include <glog/logging.h>
ConfigManagerTest::ConfigManagerTest() : _isFirstTime(true), _enableDumpConfig(false)
{
}

bool ConfigManagerTest::loadListTests()
{
    bool ret = true;

    if (_isFirstTime)
    {
        std::string homeDir = std::getenv("WORKSPACE") != nullptr ? std::getenv("WORKSPACE") : std::getenv("HOME");
        if (homeDir.size())
        {
            auto fullPath = findFileRecursively(homeDir, LIST_TEST_FILE);
            if (!fullPath.size())
            {
                LOG(ERROR) << "Error on find the file " << fullPath;
                ret = false;
            }
            else
            {
                nlohmann::json json_value;
                ret = rs::io::readJsonFile(fullPath, json_value);
                if (ret)
                {
                    LOG(INFO) << "Read  the file " << fullPath;
                    for (auto &json_item : json_value["Tests"])
                    {
                        _allNameFileTest.push_back(json_item["File_Name_Test"]);
                        LOG(INFO) << "Add Test  " << json_item["File_Name_Test"];
                    }

                    if (loadConfigurationTests(homeDir))
                    {
                        _isFirstTime = false;
                    }
                }
                else
                {
                    LOG(ERROR) << "Error on read  the file " << fullPath;
                    ret = false;
                }
            }
        }
    }

    return ret;
}
bool ConfigManagerTest::loadConfigurationTests(std::string homeDir)
{
    bool ret = true;
    if (homeDir.size())
    {
        for (auto &nameFile : _allNameFileTest)
        {
            nlohmann::json json_value;
            auto fullPath = findFileRecursively(homeDir, nameFile);
            if (fullPath.size())
            {
                ret = rs::io::readJsonFile(fullPath, json_value);
                if (ret)
                {

                    std::cout << "Test Name:" << json_value["Name_Test"];

                    for (auto &json_item : json_value["Tests"])
                    {

                        Test t{json_item["Test_Id"], json_item["Type"], json_item["Input"]["XYZ_File"],
                               json_item["Input"]["Rs_AlgoConf_File"], json_item["Input"]["Calib_File"],
                               json_item["Input"]["IntrisicParam_File"], json_item["NumberExectution"]};
                        addTest(json_item["Test_Id"], t);

                        for (auto &json_item_expected : json_item["Expected_Results"])
                        {

                            for (auto &json_item_eParam : json_item_expected["Expected_Params"])
                            {
                                ExpectedResults er{json_item_expected["Current_Step"],
                                                   json_item_eParam["Expected_Parameter"],
                                                   json_item_eParam["Expected_Value"]};
                                addExpectedRes(json_item["Test_Id"], er);
                            }
                        }

                        for (auto &json_item_update : json_item["Update_Parameter"])
                        {
                            for (auto &json_item_param : json_item_update["Parameters"])
                            {
                                UpdateParameterTest up{json_item_update["Step"],
                                                       json_item_update["File"],
                                                       json_item_param["Parameter"],
                                                       json_item_param["Value"],
                                                       json_item_update["Type_File"]};

                                addUpdPar(json_item["Test_Id"], up);
                            }
                        }
                    }
                    if (_enableDumpConfig)
                        dumpConfiguration(json_value);
                }
                else
                {
                    LOG(ERROR) << "Error on read  the file " << fullPath;
                }
            } else {
                 LOG(ERROR) << "File: " << nameFile << " not found";
            }
        }
    }
    return ret;
}

void ConfigManagerTest::dumpConfiguration(nlohmann::json &json_value)
{
    LOG(INFO) << "Test Name:" << json_value["Name_Test"];
    for (auto &json_item : json_value["Tests"])
    {
        LOG(INFO) << '\t' << "Test id:" << json_item["Test_Id"];
        LOG(INFO) << '\t' << "Type:" << json_item["Type"];
        LOG(INFO) << '\t' << "NumberExectution:" << json_item["NumberExectution"];
        LOG(INFO) << '\t' << "*** Input Section ***:";

        LOG(INFO) << "\t\t" << "XYZ_File:" << json_item["Input"]["XYZ_File"];
        LOG(INFO) << "\t\t" << "Rs_AlgoConf_File:" << json_item["Input"]["Rs_AlgoConf_File"];
        LOG(INFO) << "\t\t" << "IntrisicParam_File:" << json_item["Input"]["IntrisicParam_File"];
        LOG(INFO) << '\t' << "*** Expected Results Section ***:";

        for (auto &json_item_expected : json_item["Expected_Results"])
        {
            for (auto &json_item_eParam : json_item_expected["Expected_Params"])
            {
                LOG(INFO) << "\t\t" << "Current_step:" << json_item_expected["Current_Step"];
                LOG(INFO) << "\t\t" << "Name Parameter:" << json_item_eParam["Expected_Parameter"];
                LOG(INFO) << "\t\t" << "Value Parameter:" << json_item_eParam["Expected_Value"];
            }
        }
        LOG(INFO) << '\t' << "***Update Parameter Section ***:";
        for (auto &json_item_update : json_item["Update_Parameter"])
        {
            for (auto &json_item_param : json_item_update["Parameters"])
            {

                LOG(INFO) << "\t\t" << "File:" << json_item_update["File"];
                LOG(INFO) << "\t\t" << "Type_File:" << json_item_update["Type_File"];
                LOG(INFO) << "\t\t" << "Step:" << json_item_update["Step"];
                LOG(INFO) << "\t\t\t" << "Parameter:" << json_item_param["Parameter"];
                LOG(INFO) << "\t\t\t" << "Value:" << json_item_param["Value"];
                LOG(INFO) << std::endl;
            }
        }
    }
}
void ConfigManagerTest::addTest(std::string key, Test t)
{
    _testMap[key] = t;
}
void ConfigManagerTest::addExpectedRes(std::string key, ExpectedResults er)
{

    if (_expecteResultrMap.find(key) != _expecteResultrMap.end())
    {

        _expecteResultrMap[key].push_back(er);
    }
    else
    {
        std::list<ExpectedResults> erList{er};
        _expecteResultrMap[key] = erList;
    }
}
void ConfigManagerTest::addUpdPar(std::string key, UpdateParameterTest up)
{

    if (_updParMap.find(key) != _updParMap.end())
    {
        _updParMap[key].push_back(up);
    }
    else
    {
        std::list<UpdateParameterTest> upList{up};
        _updParMap[key] = upList;
    }
}
std::list<UpdateParameterTest> ConfigManagerTest::getUpParameter(std::string key)
{

    if (_updParMap.find(key) != _updParMap.end())
    {
        return _updParMap[key];
    }
    return std::list<UpdateParameterTest>{};
}
std::list<ExpectedResults> ConfigManagerTest::getExpectedRes(std::string key)
{

    if (_expecteResultrMap.find(key) != _expecteResultrMap.end())
    {
        return _expecteResultrMap[key];
    }
    return std::list<ExpectedResults>{};
}
std::map<std::string, Test> &ConfigManagerTest::getTestsMap()
{
    return _testMap;
}