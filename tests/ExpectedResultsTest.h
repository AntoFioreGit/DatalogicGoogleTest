#include <iostream>
#ifndef ExpectedResults_h
#define ExpectedResults_h
class ExpectedResults
{

public:
    ExpectedResults();

     ExpectedResults(int currentStep,int _expectedNuberProfile,int expectedNumberScanLine, std::string _expectedParamName,std::string _expectedParamValue);


    int expectedNuberProfile() const { return _expectedNuberProfile; }
    void setExpectedNuberProfile(int expectedNuberProfile) { _expectedNuberProfile = expectedNuberProfile; }

    int currentStep() const { return _currentStep; }
    void setCurrentStep(int currentStep) { _currentStep = currentStep; }

    int expectedNumberScanLine() const { return _expectedNumberScanLine; }
    void setExpectedNumberScanLine(int expectedNumberScanLine) { _expectedNumberScanLine = expectedNumberScanLine; }

    std::string expectedParamName() const { return _expectedParamName; }
    void setExpectedParamName(const std::string &expectedParamName) { _expectedParamName = expectedParamName; }

    std::string expectedParamValue() const { return _expectedParamValue; }
    void setExpectedParamValue(const std::string &expectedParamValue) { _expectedParamValue = expectedParamValue; }

   
private:

int _currentStep;
int _expectedNuberProfile;
int _expectedNumberScanLine;
std::string _expectedParamName;
std::string _expectedParamValue;


};
#endif