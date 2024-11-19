#include <iostream>
#ifndef ExpectedResults_h
#define ExpectedResults_h
class ExpectedResults
{

public:
    ExpectedResults();

      ExpectedResults(int currentStep,std::string _expectedParamName,std::string _expectedParamValue);

    int currentStep() const { return _currentStep; }
    void setCurrentStep(int currentStep) { _currentStep = currentStep; }

   
    std::string expectedParamName() const { return _expectedParamName; }
    void setExpectedParamName(const std::string &expectedParamName) { _expectedParamName = expectedParamName; }

    std::string expectedParamValue() const { return _expectedParamValue; }
    void setExpectedParamValue(const std::string &expectedParamValue) { _expectedParamValue = expectedParamValue; }

   
private:

int _currentStep;
std::string _expectedParamName;
std::string _expectedParamValue;


};
#endif