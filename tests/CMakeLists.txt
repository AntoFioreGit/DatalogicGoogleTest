enable_testing()

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
                    FrameProcessorTest.cpp
                    File_JsonTest.cpp
                    Calib_Io_Test.cpp
                    InitializeEnvironment.cpp)
                    

# Crea un collegamento tra testTechie e la libreria Techie (usa il nome dell'eseguibile) e le librerie di google
target_link_libraries(testAppRsaAlgorithms RSAAlgorithmsLib GTest::gtest GTest::gtest_main glog::glog) # ${LRS_TARGET}

# Registra i test, fa il modo che i test siano eseguiti con il comando CTest
#add_test(NAME testAppRsaAlgorithms COMMAND testAppRsaAlgorithms hOperation)

gtest_discover_tests(testAppRsaAlgorithms)