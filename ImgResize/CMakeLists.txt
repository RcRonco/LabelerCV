cmake_minimum_required(VERSION 3.5)
project(ImgResize)

LINK_DIRECTORIES(/usr/local/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES main.cpp ImageResize.cpp ImageResize.h)

add_executable(ImgResize ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(ImgResize opencv_core opencv_imgproc opencv_videoio opencv_imgcodecs opencv_video opencv_highgui boost_system boost_program_options boost_filesystem)