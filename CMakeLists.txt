cmake_minimum_required(VERSION 3.5)
project(LabelerCV)

LINK_DIRECTORIES(/usr/local/lib)
add_subdirectory(VideoPlayer)
add_subdirectory(ImgResize)
add_subdirectory(ImgCut)
add_subdirectory(TrainValReader)
