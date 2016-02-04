#include "General.h"
#include "VideoPlayer.h"
#include <ctime>
#include <sstream>
#include <Windows.h>

using namespace Labeler;

void timeTrackbarHandler(int pos, void* userdata);
void mouseHandler(int event, int x, int y, int flags, void* param);

bool dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

Labeler::VideoPlayer::VideoPlayer(std::string videoPath, const char * winname) : _path(videoPath), _WIN_NAME(winname)
{
	if (!loadVideo(videoPath))
		throw std::exception("Can't load the video");

	cv::namedWindow(_WIN_NAME, cv::WINDOW_KEEPRATIO);
	//cv::setWindowProperty(_WIN_NAME, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	cv::createTrackbar(_TIMEBAR_NAME, _WIN_NAME, 0, vidlength, timeTrackbarHandler, reinterpret_cast<void*>(&_capture));
	cv::setMouseCallback(_WIN_NAME, mouseHandler, this);
	

	if (!dirExists("Humans"))
		CreateDirectory(L"Humans", NULL);
	if (!dirExists("Cars"))
		CreateDirectory(L"Cars", NULL);
	if (!dirExists("Animals"))
		CreateDirectory(L"Animals", NULL);
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
	int a = 0;
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
			a++;
			strm << std::to_string(time(0)) << "_" << std::to_string(a) << ".png";
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
	VideoPlayer* player = (VideoPlayer*)param;
	if (event == CV_EVENT_LBUTTONDOWN && !player->isMouseDragging())
	{
		/* left button clicked. ROI selection begins */
		player->setPoint1(cv::Point(x, y));
		player->setMouseDragging(true);
	}

	if (event == CV_EVENT_MOUSEMOVE && player->isMouseDragging())
	{
		/* mouse dragged. ROI being selected */
		cv::Mat img1 = player->getForegroundImage().clone();
		player->setPoint2(cv::Point(x, y));
		cv::Scalar color;
		switch (player->getLabel())
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

		cv::rectangle(img1, player->getPoint1(), player->getPoint2(), color, 1, 8, 0);
		cv::imshow(player->getWindowName(), img1);
	}

	if (event == CV_EVENT_LBUTTONUP && player->isMouseDragging())
	{
		player->setPoint2(cv::Point(x, y));
		player->setMouseDragging(false);

		cv::Scalar color;
		switch (player->getLabel())
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

		cv::Mat frm = player->getForegroundImage().clone();
		player->pushMat(frm);
		cv::Rect rect(player->getPoint1(), player->getPoint2());
		player->pushRect(RectType(rect, player->getLabel()));
		cv::rectangle(player->getForegroundImage(), rect, color, 1);
		cv::imshow(player->getWindowName(), player->getForegroundImage());

		/*cv::Mat img = player->getFrame()(cv::Rect(player->getPoint1(), player->getPoint2()));
		
		strm << std::to_string(time(0)) << ".png";
		cout << strm.str().c_str() << endl;

		if (!cv::imwrite(strm.str(), img))
			throw exception("Unable to save the picture");*/
	}
}