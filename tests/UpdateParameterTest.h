#include <iostream>
class UpdateParameterTest {
    public:
    UpdateParameterTest();
    
    int currentStep() const { return _currentStep; }
    void setCurrentStep(int currentStep) { _currentStep = currentStep; }

    std::string file() const { return _file; }
    void setFile(const std::string &file) { _file = file; }

    std::string parameterName() const { return _parameterName; }
    void setParameterName(const std::string &parameterName) { _parameterName = parameterName; }

    std::string value() const { return _value; }
    void setValue(std::string &value) { _value = value; }

    private:

    int _currentStep;
    std::string _file;
    std::string _parameterName;
    std::string _value;


 
};