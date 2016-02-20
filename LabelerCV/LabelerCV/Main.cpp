#include "VideoPlayer.h"
#include <memory>
#include <opencv2\highgui.hpp>

using namespace Labeler;
const char * window_name = "PoV - Dataset Slicer";

int main(int argc, char** argv)
{
	if (argc != 2)
		return -1;

	int key = 0;
	auto video = std::make_unique<VideoPlayer>(argv[1], window_name);
	
	while ((cv::getWindowProperty(window_name, CV_WND_PROP_FULLSCREEN) != -1) && !video->Shutdown)
	{
		for (int i = 0; video->isPlaying && video->readImage(); i++)
			video->showImage();

		if (video->isPlaying)
			key = cv::waitKey(2);
		else
			key = cv::waitKey();

		video->keyAction((char)key);
	}

	cv::destroyAllWindows();

	return 0;
}