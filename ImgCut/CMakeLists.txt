cmake_minimum_required(VERSION 3.5)
project(ImgCut)

LINK_DIRECTORIES(/usr/local/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES main.cpp ImageCut.h ../ImgResize/main.cpp ../ImgResize/ImageResize.h ../ImgResize/ImageResize.cpp)

add_executable(ImgCut ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(ImgCut opencv_core opencv_imgproc opencv_videoio opencv_imgcodecs opencv_video opencv_highgui boost_system boost_program_options boost_filesystem)