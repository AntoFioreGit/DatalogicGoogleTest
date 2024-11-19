#include "ExpectedResultsTest.h"

int _expectedNuberProfile;
  ExpectedResults::ExpectedResults(){
    
  }
 

 ExpectedResults::ExpectedResults(int currentStep,std::string expectedParamName,std::string expectedParamValue):
 _currentStep(currentStep),_expectedParamName(expectedParamName),_expectedParamValue(expectedParamValue)
 {};