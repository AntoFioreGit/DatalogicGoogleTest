#include "ExpectedResultsTest.h"

int _expectedNuberProfile;
  ExpectedResults::ExpectedResults():_currentStep(-1), _expectedNuberProfile(-1),_expectedNumberScanLine(-1){
    
  }
 ExpectedResults::ExpectedResults(int currentStep,int expectedNuberProfile,int expectedNumberScanLine,std::string expectedParamName,std::string expectedParamValue):
 _currentStep(currentStep),_expectedNuberProfile(expectedNuberProfile),_expectedNumberScanLine(expectedNumberScanLine),_expectedParamName(expectedParamName),_expectedParamValue(expectedParamValue)
 {};