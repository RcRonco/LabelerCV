cmake_minimum_required(VERSION 3.5)
project(TrainValReader)

LINK_DIRECTORIES(/usr/local/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES TrainValReader.cpp)

add_executable(TrainValReader ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(TrainValReader)