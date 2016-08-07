//
// Created by Ron Cohen on 07/08/2016.
//

#include <memory>
#include <iostream>
#include <boost/program_options.hpp>

#include "ImageResize.h"

namespace po = boost::program_options;

int main(int argc, const char** argv)
{
    po::variables_map vm;

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,?", "Show help message.")
                ("resize,r", po::value<std::string>(), "Resize images, need height and width.")
                ("dest,d", po::value<std::string>(), "The destination to the resized images, if not exists the source images will be overrided.")
                ("height,h", po::value<uint32_t>(), "New Images height.")
                ("width,w", po::value<uint32_t>(), "New Images width.");

        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);

        if (!vm["help"].empty())
        {
            std::cout << desc;
        }
        else if (!vm["resize"].empty())
        {
            if (vm["height"].empty() || vm["width"].empty())
            {
                std::cout << "For resize must be width and height" << std::endl;
                return -1;
            }
            else if (!vm["dest"].empty())
            {
                Labeler::ImageRefactor::ResizeImages(vm["resize"].as<std::string>(), vm["dest"].as<std::string>(), vm["height"].as<uint32_t>(), vm["width"].as<uint32_t>());
            }
            else
            {
                Labeler::ImageRefactor::ResizeImages(vm["resize"].as<std::string>(), vm["height"].as<uint32_t>(), vm["width"].as<uint32_t>());
            }
        }

        return 0;
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;

        return -1;
    }
}