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
	int opt;

	po::variables_map vm;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,H,h,?", "produce help message")
		("video,C,c", po::value<std::string>(), "Run video player")
		("resize,R,r", po::value<std::vector<std::string>>(), "ResizeImages")
		("dest,D,d", po::value<std::vector<std::string>>(), "Dest")
		("height,H,h", po::value<uint32_t>(), "New Images height")
		("width,W,w", po::value<uint32_t>(), "New Images width");

	po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
	
	if (vm.size() < 2 || vm["help,H,h,?"].empty())
	{
		
	}
	else if (!vm["video,C,c"].empty())
	{
		runVideoPlayer(vm["video,C,c"].as<std::string>());
	}
	else if (!vm["resize,R,r"].empty())
	{
		if (!vm["height,H,h"].empty() || !vm["width,W,w"].empty())
		{
			std::cout << "For resize must be width and height" << std::endl;
			return -1;
		}
		else if (!vm["dest,D,d"].empty())
		{
			Labeler::ImageRefactor::ResizeImages(vm["resize,R,r"].as<std::string>(), vm["dest,D,d"].as<std::string>(), vm["height,H,h"].as<uint32_t>(), vm["width,W,w"].as<uint32_t>());
		}
		else
		{
			Labeler::ImageRefactor::ResizeImages(vm["resize,R,r"].as<std::string>(), vm["height,H,h"].as<uint32_t>(), vm["width,W,w"].as<uint32_t>());
		}
	}
	
	return 0;
}

void runVideoPlayer(std::string path)
{
	const char * window_name = "PoV - Dataset Slicer";
	auto video = std::make_unique<VideoPlayer>(path, window_name);
	video->run();
}