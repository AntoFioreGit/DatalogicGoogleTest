#include <iostream>

class ExpectedResults
{

public:
    ExpectedResults();

    int expectedNuberProfile() const { return _expectedNuberProfile; }
    void setExpectedNuberProfile(int expectedNuberProfile) { _expectedNuberProfile = expectedNuberProfile; }

    int currentStep() const { return _currentStep; }
    void setCurrentStep(int currentStep) { _currentStep = currentStep; }

    int expectedNumberScanLine() const { return _expectedNumberScanLine; }
    void setExpectedNumberScanLine(int expectedNumberScanLine) { _expectedNumberScanLine = expectedNumberScanLine; }

   
private:

int _currentStep;
int _expectedNuberProfile;
int _expectedNumberScanLine;


};