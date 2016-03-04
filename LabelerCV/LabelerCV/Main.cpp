#include <memory>
#include <opencv2\highgui.hpp>
#include <boost\program_options.hpp>

#include "ImageRefactor.h"
#include "VideoPlayer.h"

using namespace Labeler;
namespace po = boost::program_options;
void runVideoPlayer(std::string path);

int main(int argc, char** argv)
{
	po::variables_map vm;

	try
	{

		po::options_description desc("Allowed options");
		desc.add_options()
			("help,?", "produce help message")
			("video,c", po::value<std::string>(), "Run Dataset video player")
			("resize,r", po::value<std::string>(), "Resize images, need height and width")
			("dest,d", po::value<std::string>(), "The destination to the resized images, if not exists the source images will be overrided.")
			("height,h", po::value<uint32_t>(), "New Images height")
			("width,w", po::value<uint32_t>(), "New Images width");

		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);

		if (!vm["help"].empty())
		{
			std::cout << desc;
		}
		else if (!vm["video"].empty())
		{
			runVideoPlayer(vm["video"].as<std::string>());
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

void runVideoPlayer(std::string path)
{
	const char * window_name = "PoV - Dataset Slicer";
	auto video = std::make_unique<VideoPlayer>(path, window_name);
	video->run();
}