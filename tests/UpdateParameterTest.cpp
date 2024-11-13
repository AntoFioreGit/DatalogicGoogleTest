#include "UpdateParameterTest.h"

  UpdateParameterTest::UpdateParameterTest():_currentStep(-1){
    
  }

   UpdateParameterTest::UpdateParameterTest(int currentStep,std::string file,std::string parameterName,std::string value):
   _currentStep(currentStep),_file(file),_parameterName(parameterName),_value(value)
   {};