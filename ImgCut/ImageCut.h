//
// Created by ronco on 7/1/16.
//

#ifndef LABELERCV_IMAGECUT_H
#define LABELERCV_IMAGECUT_H
#include <stdint.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

void CutImage(std::string imagePath, uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::string output) {
    if (!fs::exists(fs::path(imagePath)))
        std::runtime_error("The image file or directory does not exists.");
    if (fs::exists(fs::path(output)))
        std::runtime_error("The output file already exists.");

    cv::Mat originalImg = cv::imread(imagePath);
    cv::Rect rect(x,y,w,h);
    cv::Mat ROI = originalImg(rect);

    cv::imwrite(output, ROI);
}
void CutVideo(std::string vidPath, uint32_t x, uint32_t y, uint32_t w, uint32_t h, double time, std::string output) {
    if (!fs::exists(fs::path(vidPath)))
        std::runtime_error("The video file or directory does not exists.");
    if (fs::exists(fs::path(output)))
        std::runtime_error("The output file already exists.");

    cv::Mat originalImage;
    cv::Rect rect(x,y,w,h);
    cv::VideoCapture vid(vidPath);
    vid.set(CV_CAP_PROP_POS_MSEC, time);
    if (vid.read(originalImage)) {
        cv::Mat ROI = originalImage(rect);

        cv::imwrite(output, ROI);
        vid.release();
    }
}
#endif //LABELERCV_IMAGECUT_H
