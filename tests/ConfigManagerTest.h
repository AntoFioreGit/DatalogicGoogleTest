#include <iostream>
#define NAME_FILE_CONF "ConfigTest.json"
#include <map>
#include <list>
#include "Test.h"
#include "ExpectedResultsTest.h"
#include "UpdateParameterTest.h"
class ConfigManagerTest {
public:
    static ConfigManagerTest& getInstance() {
        static ConfigManagerTest instance;
        return instance;
    }

    bool loadConfiguration();
    void dumpConfiguration();
    std::list <UpdateParameterTest> getUpParameter(std::string key);
    std::list <ExpectedResults> getExpectedRes(std::string key);
    std::map<std::string,Test>& getTestsMap() ;

private:
    ConfigManagerTest();
    ConfigManagerTest(const ConfigManagerTest&) = delete; 
    ConfigManagerTest& operator=(const ConfigManagerTest&) = delete; 

    void addTest(std::string key, Test);
    void addExpectedRes(std::string key,ExpectedResults er);
    void addUpdPar(std::string key,UpdateParameterTest up);
    
    bool _isFirstTime;

    std::map<std::string,Test> _testMap;
    std::map <std::string, std::list <UpdateParameterTest>> _updParMap;
    std::map <std::string, std::list <ExpectedResults>> _expecteResultrMap;

};