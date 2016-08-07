//
// Created by ronco on 7/1/16.
//
#include <stdint.h>
#include <iostream>
#include <boost/program_options.hpp>

#include "ImageCut.h"

namespace po = boost::program_options;
void runVideoPlayer(std::string path, std::string outputPath);

int main(int argc, const char** argv) {
    po::variables_map vm;

    try {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,?", "Show help message.")
                ("img,i", po::value<void>(), "Cut image.")
                ("video,v", po::value<std::string>(), "Cut image from specific time in video., Video Path")
                ("width,w", po::value<uint32_t>(), "Width of the object in the image.")
                ("height,h", po::value<uint32_t>(), "Height of the object in the image.")
                ("x", po::value<uint32_t>(), "X coordinate of the object in the image.")
                ("y", po::value<uint32_t>(), "Y coordinate of the object in the image.")
                ("time, t", po::value<double>(), "Y coordinate of the object in the image.")
                ("output,o", po::value<std::string>(), "Output file for the train data.");

        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        if (!vm["help"].empty()) {
            std::cout << desc;
        } else if (vm["y"].empty() && vm["height"].empty() &&
                   vm["x"].empty() && vm["width"].empty() &&
                   vm["output"].empty() && (vm["video"].empty() || vm["img"].empty())) {
            std::cout << "Please enter all the needed parameters" << std::endl;
        } else {
            if (!vm["img"].empty()) {
                CutImage(vm["img"].as<std::string>(), vm["x"].as<uint32_t>(),
                         vm["y"].as<uint32_t>(), vm["width"].as<uint32_t>(),
                         vm["height"].as<uint32_t>(), vm["output"].as<std::string>());
            } else if (!vm["video"].empty() && !vm["time"].empty()) {
                CutVideo(vm["video"].as<std::string>(), vm["x"].as<uint32_t>(),
                         vm["y"].as<uint32_t>(), vm["width"].as<uint32_t>(),
                         vm["height"].as<uint32_t>(), vm["time"].as<double>(),
                         vm["output"].as<std::string>());
            }
        }
    } catch (std::exception& e) {
        std::cout << "An error occurred: " << e.what() << std::endl;
    }
}

