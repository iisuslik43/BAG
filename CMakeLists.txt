project(BAG)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp headers.cpp headers.h config.cpp config.h writing.h writing.cpp)
add_executable(BAG ${SOURCE_FILES})