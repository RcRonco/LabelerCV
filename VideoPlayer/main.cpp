#include <memory>
#include <iostream>
#include <boost/program_options.hpp>

#include "VideoPlayer.h"

using namespace RCO;
namespace po = boost::program_options;

void runVideoPlayer(std::string path, std::string outputPath);

void printCVVersion() {
    std::cout << "OpenCV version : " << CV_VERSION << std::endl;
    std::cout << "Major version : " << CV_MAJOR_VERSION << std::endl;
    std::cout << "Minor version : " << CV_MINOR_VERSION << std::endl;
    std::cout << "Subminor version : " << CV_SUBMINOR_VERSION << std::endl;
}

int main(int argc, const char** argv)
{
    po::variables_map vm;

    printCVVersion();

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,?", "Show help message.")
                ("video,v", po::value<std::string>(), Labeler::szVideoDescription)
                ("output,o", po::value<std::string>(), "Output file for the train data.");

        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);

        if (!vm["help"].empty())
        {
            std::cout << desc;
        }
        else if (!vm["video"].empty())
        {
            if (!vm["output"].empty())
                runVideoPlayer(vm["video"].as<std::string>(), vm["output"].as<std::string>());
            else
                runVideoPlayer(vm["video"].as<std::string>(), "");
        }

        return 0;
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;

        return -1;
    }
}

void runVideoPlayer(std::string path, std::string outputPath)
{
    const char * window_name = "Dataset Slicer";
    std::unique_ptr<Labeler::VideoPlayer> video;
    if (outputPath == "")
        video = std::unique_ptr<Labeler::VideoPlayer>(new Labeler::VideoPlayer(path, window_name));
    else
        video = std::unique_ptr<Labeler::VideoPlayer>(new Labeler::VideoPlayer(path, window_name, outputPath));
    video->run();
}