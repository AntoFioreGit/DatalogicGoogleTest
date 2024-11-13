#include "ExpectedResultsTest.h"

int _expectedNuberProfile;
  ExpectedResults::ExpectedResults():_currentStep(-1), _expectedNuberProfile(-1),_expectedNumberScanLine(-1){
    
  }
 ExpectedResults::ExpectedResults(int currentStep,int expectedNuberProfile,int expectedNumberScanLine ):
 _currentStep(currentStep),_expectedNuberProfile(expectedNuberProfile),_expectedNumberScanLine(expectedNumberScanLine)
 {};