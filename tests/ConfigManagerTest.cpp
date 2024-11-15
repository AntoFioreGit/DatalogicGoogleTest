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

                    Test t{json_item["Test_Id"], json_item["Type"], json_item["Input"]["XYZ_File"],
                           json_item["Input"]["Rs_AlgoConf_File"], json_item["Input"]["Calib_File"],
                           json_item["Input"]["IntrisicParam_File"], json_item["NumberExectution"]};
                    addTest(json_item["Test_Id"], t);

                    for (auto &json_item_expected : json_item["Expected_Results"])
                    {
                        ExpectedResults er{json_item_expected["Current_Step"],
                                           json_item_expected["NumberProfiles"],
                                           json_item_expected["NumebeScanLine"]};

                        addExpectedRes(json_item["Test_Id"], er);
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
            }
            else
            {
                // TODO LOG Errorr
            }
        }
    }
    dumpConfiguration();

    return ret;
}
void ConfigManagerTest::dumpConfiguration()
{

    std::string homeDir = std::getenv("WORKSPACE") != nullptr ? std::getenv("WORKSPACE") : std::getenv("HOME");
    if (homeDir.size())
    {
        auto fullPath = findFileRecursively(homeDir, NAME_FILE_CONF);
        nlohmann::json json_value;
        bool ret = rs::io::readJsonFile(fullPath, json_value);
        if (ret)
        {
            ;
            std::cout << "Name Configuration:" << json_value["Name_Configurarion"] << std::endl;

            for (auto &json_item : json_value["Tests"])
            {

                std::cout << '\t' << "Test id:" << json_item["Test_Id"] << std::endl;
                std::cout << '\t' << "Type:" << json_item["Type"] << std::endl;
                std::cout << '\t' << "NumberExectution:" << json_item["NumberExectution"] << std::endl;
                std::cout << '\t' << "*** Input Section ***:" << std::endl;

                std::cout << "\t\t" << "XYZ_File:" << json_item["Input"]["XYZ_File"] << std::endl;
                std::cout << "\t\t" << "Rs_AlgoConf_File:" << json_item["Input"]["Rs_AlgoConf_File"] << std::endl;
                std::cout << "\t\t" << "IntrisicParam_File:" << json_item["Input"]["IntrisicParam_File"] << std::endl;
                std::cout << '\t' << "*** Expected Results Section ***:" << std::endl;

                for (auto &json_item_expected : json_item["Expected_Results"])
                {

                    std::cout << "\t\t" <<"Current_step:" << json_item_expected["Current_Step"] << std::endl;
                    std::cout << "\t\t" <<"NumberProfiles:" << json_item_expected["NumberProfiles"] << std::endl;
                    std::cout << "\t\t" <<"NumebeScanLine:" << json_item_expected["NumebeScanLine"] << std::endl;
                }
                 std::cout << '\t' << "***Update Parameter Section ***:" << std::endl;


                for (auto &json_item_update : json_item["Update_Parameter"])
                {
                    for (auto &json_item_param : json_item_update["Parameters"])
                    {

                        std::cout <<  "\t\t"<<"File:" << json_item_update["File"] << std::endl;
                         std::cout <<  "\t\t"<<"Type_File:" << json_item_update["Type_File"] << std::endl;
                        std::cout << "\t\t"<< "Step:" << json_item_update["Step"] << std::endl;
                        std::cout << "\t\t\t"<<"Parameter:" << json_item_param["Parameter"] << std::endl;
                        std::cout << "\t\t\t"<<"Value:" << json_item_param["Value"] << std::endl;
                        std::cout <<std::endl;
                        
                    }
                }
            }
        }
        else
        {
            std::cout << "*** Dump Configuration failed ***" << std::endl;
        }
    }

    std::cout << "*** Dump Configuration ***" << std::endl;
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