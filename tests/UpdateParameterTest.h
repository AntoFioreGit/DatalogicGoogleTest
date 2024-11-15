#include <iostream>
#ifndef UpdateParameterTest_h
#define UpdateParameterTest_h
class UpdateParameterTest {
    public:
    UpdateParameterTest();
    UpdateParameterTest(int currentStep,std::string file,std::string parameterName,std::string value,std::string typeFile);
    int currentStep() const { return _currentStep; }
    void setCurrentStep(int currentStep) { _currentStep = currentStep; }

    std::string file() const { return _file; }
    void setFile(const std::string &file) { _file = file; }

    std::string parameterName() const { return _parameterName; }
    void setParameterName(const std::string &parameterName) { _parameterName = parameterName; }

    std::string value() const { return _value; }
    void setValue(std::string &value) { _value = value; }

    std::string typeFile() const { return _typeFile; }
    void setTypeFile(const std::string &typeFile) { _typeFile = typeFile; }

    private:

    int _currentStep;
    std::string _file;
    std::string _typeFile;
    std::string _parameterName;
    std::string _value;


 
};
#endif