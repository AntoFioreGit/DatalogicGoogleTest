
# Include il modulo FetchContent per scaricare GoogleTest
include(FetchContent)

# Scaricare e integrare GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
#include(CTest)
include(GoogleTest)

# Crea l'eseguibile, chiamato testTechie, del sorgente di test
add_executable(testAppRsaAlgorithms                                       
#                unit/FrameProcessorTest.cpp
#                  unit/File_JsonTest.cpp
#                 unit/Calib_Io_Test.cpp
#                  unit/CameraParameterTest.cpp
#                  unit/PtrParameterTest.cpp
#                  unit/InvalidParameterTest.cpp
#                  unit/ParametersContainerTest.cpp
#                 unit/CcbTest.cpp
#                  unit/AdTof_IntrisicsTest.cpp
#                  unit/ImageFrameTest.cpp
#                 unit/XYZFrameTest.cpp                    
 #                 functional/CheckGenerateOutputTest.cpp
                 functional/ProfileTest.cpp
                  InitializeEnvironment.cpp
                  Util.cpp
                  ConfigManagerTest.cpp
                  Test.cpp
                  ExpectedResultsTest.cpp
                  UpdateParameterTest.cpp)
                    
                    

                   

# Crea un collegamento tra testTechie e la libreria Techie (usa il nome dell'eseguibile) e le librerie di google
target_link_libraries(testAppRsaAlgorithms RSAAlgorithmsLib GTest::gtest GTest::gtest_main glog::glog) # ${LRS_TARGET}
target_include_directories(testAppRsaAlgorithms PUBLIC .)
# Registra i test, fa il modo che i test siano eseguiti con il comando CTest
#add_test(NAME testAppRsaAlgorithms COMMAND testAppRsaAlgorithms hOperation)

gtest_discover_tests(testAppRsaAlgorithms)