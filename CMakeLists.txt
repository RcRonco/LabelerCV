cmake_minimum_required(VERSION 3.5)
project(LabelerCV)

LINK_DIRECTORIES(/usr/local/lib)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)

add_subdirectory(VideoPlayer)
add_subdirectory(ImgResize)
add_subdirectory(ImgCut)
add_subdirectory(TrainValReader)
