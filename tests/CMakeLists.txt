# Include il modulo FetchContent per scaricare GoogleTest
include(FetchContent)

# Scaricare e integrare GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# Crea l'eseguibile, chiamato testTechie, del sorgente di test
add_executable(testMathOperation testMathOperation.cpp testMathOperation2.cpp)

# Crea un collegamento tra testTechie e la libreria Techie (usa il nome dell'eseguibile) e le librerie di google
target_link_libraries(testMathOperation RSAAlgorithmsLib GTest::gtest  glog::glog)

# Registra i test, fa il modo che i test siano eseguiti con il comando CTest
add_test(NAME testMathOperation COMMAND testMathOperation hOperation)