cmake_minimum_required(VERSION 3.10)
set(TEST_TARGET MyFirstPorject)
project(MyFirstPorject)


project(${TEST_TARGET}
	LANGUAGES CXX C
	DESCRIPTION "Lib Range Sensor to extract parcel features from 3D images")
message(STATUS "PROJECT_NAME: ${TEST_TARGET}")

# options
# set(CMAKE_BUILD_TYPE Debug)

######################################################################################################
option(BUILD_SHARED_LIB "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" ON)
option(BUILD_TEST_EXECUTABLE "Build the test executable" ON)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/3rdparty/glog-0.7.0/lib/cmake/glog)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/3rdparty/opencv-4.5.5)
find_package(glog REQUIRED)
find_package(OpenCV REQUIRED)

# Genera  la libreria RSAAlgorithmsLib
add_library(RSAAlgorithmsLib src/MathOperation.cpp 
						src/io/Prova.cpp
						src/io/file_json.cpp
						src/common/frame_processor.cpp)
target_link_libraries(RSAAlgorithmsLib ${OpenCV_LIBS})

# Specifica la cartella degli "include" relativi agli header file delle classi da testare
target_include_directories(RSAAlgorithmsLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src 
											${CMAKE_CURRENT_SOURCE_DIR}/src/io)

target_include_directories(RSAAlgorithmsLib PRIVATE ${OpenCV_INCLUDE_DIRS})
##########################################################################################################																					

# option(ENABLE_TESTS "Enable unit tests")
# if (ENABLE_TESTS)
# enable_testing()
#     include(GoogleTest)
# endif()
# Abilita i test
enable_testing()

# Aggiungere la directory dei test
add_subdirectory(tests)
