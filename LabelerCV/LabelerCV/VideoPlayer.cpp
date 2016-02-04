#include "General.h"
#include "VideoPlayer.h"
#include <ctime>
#include <sstream>
#include <boost\filesystem.hpp>

using namespace Labeler;

void timeTrackbarHandler(int pos, void* userdata);
void mouseHandler(int event, int x, int y, int flags, void* param);

Labeler::VideoPlayer::VideoPlayer(std::string videoPath, const char * winname) : _path(videoPath), _WIN_NAME(winname)
{
	if (!loadVideo(videoPath))
		throw std::exception("Can't load the video");

	cv::namedWindow(_WIN_NAME, cv::WINDOW_KEEPRATIO);
	//cv::setWindowProperty(_WIN_NAME, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	cv::createTrackbar(_TIMEBAR_NAME, _WIN_NAME, 0, vidlength, timeTrackbarHandler, reinterpret_cast<void*>(&_capture));
	cv::setMouseCallback(_WIN_NAME, mouseHandler, this);
	
	if (boost::filesystem::exists(boost::filesystem::path(L"Humans")))
		boost::filesystem::create_directory(boost::filesystem::path(L"Humans"));
	if (boost::filesystem::exists(boost::filesystem::path(L"Cars")))
		boost::filesystem::create_directory(boost::filesystem::path(L"Cars"));
	if (boost::filesystem::exists(boost::filesystem::path(L"Animals")))
		boost::filesystem::create_directory(boost::filesystem::path(L"Animals"));
}

bool Labeler::VideoPlayer::isVideoEnded()
{
	return cv::getTrackbarPos(_TIMEBAR_NAME, _WIN_NAME) == (int)vidlength;
}

void Labeler::VideoPlayer::changeTime(int seconds)
{
	cv::setTrackbarPos(_TIMEBAR_NAME, _WIN_NAME, seconds);
}

bool Labeler::VideoPlayer::readImage()
{
	if (!_capture.read(_frameBuffer))
		return false;

	std::stack<cv::Mat> empt;
	std::swap(_historyMat, empt);

	pushMat(_frameBuffer.clone());
	_frameCounter++;

	return true;
}

void Labeler::VideoPlayer::showImage() 
{
	cv::imshow(_WIN_NAME, _frameBuffer);
	cv::waitKey(show_interval);

	if (_frameCounter % (int)fps == 0)
		cv::setTrackbarPos(_TIMEBAR_NAME, _WIN_NAME, cv::getTrackbarPos(_TIMEBAR_NAME, _WIN_NAME) + 1);
}

void Labeler::VideoPlayer::CutImages()
{
	uint32_t imgCounter = 0;
	while (!_historyRects.empty())
	{
 		stringstream strm;

		switch (_historyRects.top().second)
		{
		case LabelType::Human:
			strm << "Humans\\";
			break;
		case LabelType::Car:
			strm << "Cars\\";
			break;
		case LabelType::Animal:
			strm << "Animals\\";
			break;
		}

		cv::Mat img = getFrame()(_historyRects.top().first);
		if (img.data)
		{
			imgCounter++;
			strm << std::to_string(time(0)) << "_" << std::to_string(imgCounter) << ".png";
			cout << strm.str().c_str() << endl;

			if (!cv::imwrite(strm.str(), img))
				throw exception("Unable to save the picture");
			_historyRects.pop();
		}
	}

	std::stack<RectType> empt;
	std::swap(_historyRects, empt);
}

bool Labeler::VideoPlayer::loadVideo(std::string VideoPath)
{
	_capture = cv::VideoCapture(_path);
	if (_capture.isOpened())
	{
		fps = _capture.get(cv::CAP_PROP_FPS);
		frame_count = _capture.get(cv::CAP_PROP_FRAME_COUNT);
		vidlength = frame_count / fps;
		show_interval = 1000 / fps;

		return true;
	}
	else
		return false;
}

void timeTrackbarHandler(int pos, void* userdata)
{
	cv::VideoCapture* vid = reinterpret_cast<cv::VideoCapture*>(userdata);
	vid->set(cv::CAP_PROP_POS_MSEC, pos * 1000);
}

void mouseHandler(int event, int x, int y, int flags, void* param)
{
	VideoPlayer* videoPlayer = (VideoPlayer*)param;

	if (event == CV_EVENT_LBUTTONDOWN && !videoPlayer->isMouseDragging())
	{
		/* left button clicked. ROI selection begins */
		videoPlayer->setPoint1(cv::Point(x, y));
		videoPlayer->setMouseDragging(true);
	}

	if (event == CV_EVENT_MOUSEMOVE && videoPlayer->isMouseDragging())
	{
		/* mouse dragged. ROI being selected */
		cv::Mat img1 = videoPlayer->getForegroundImage().clone();
		videoPlayer->setPoint2(cv::Point(x, y));
		cv::Scalar color;
		switch (videoPlayer->getLabel())
		{
			case LabelType::Human:
				color = Labeler::RED;
				break;
			case LabelType::Car:
				color = Labeler::BLUE;
				break;
			case LabelType::Animal:
				color = Labeler::GREEN;
				break;
		}

		cv::rectangle(img1, videoPlayer->getPoint1(), videoPlayer->getPoint2(), color, 1, 8, 0);
		cv::imshow(videoPlayer->getWindowName(), img1);
	}

	if (event == CV_EVENT_LBUTTONUP && videoPlayer->isMouseDragging())
	{
		videoPlayer->setPoint2(cv::Point(x, y));
		videoPlayer->setMouseDragging(false);

		cv::Scalar color;
		switch (videoPlayer->getLabel())
		{
			case LabelType::Human:
				color = Labeler::RED;
				break;
			case LabelType::Car:
				color = Labeler::BLUE;
				break;
			case LabelType::Animal:
				color = Labeler::GREEN;
				break;
		}
		
		cv::Mat frm = videoPlayer->getForegroundImage().clone();
		videoPlayer->pushMat(frm);

		if (videoPlayer->getPoint2().x < 0)
			videoPlayer->setPoint2(cv::Point(0, videoPlayer->getPoint2().y));
		else if (videoPlayer->getPoint2().x > frm.cols)
			videoPlayer->setPoint2(cv::Point(frm.cols, videoPlayer->getPoint2().y));
		if (videoPlayer->getPoint2().y < 0)
			videoPlayer->setPoint2(cv::Point(videoPlayer->getPoint2().x, 0));
		else if (videoPlayer->getPoint2().y > frm.rows)
			videoPlayer->setPoint2(cv::Point(videoPlayer->getPoint2().x, frm.rows));


		cv::Rect rect(videoPlayer->getPoint1(), videoPlayer->getPoint2());
		videoPlayer->pushRect(RectType(rect, videoPlayer->getLabel()));
		cv::rectangle(videoPlayer->getForegroundImage(), rect, color, 1);
		cv::imshow(videoPlayer->getWindowName(), videoPlayer->getForegroundImage());
	}
}