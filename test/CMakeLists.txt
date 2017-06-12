cmake_minimum_required(VERSION 2.6)
 
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../)
# Link runTests with what we want to test and the GTest and pthread library
set(SOURCE_FILES test.cpp ../headers.cpp ../writing.cpp ../config.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -std=c++14 -lgtest_main -lgtest -lpthread")
add_executable(test ${SOURCE_FILES})
target_link_libraries(test ${GTEST_LIBRARIES} pthread)