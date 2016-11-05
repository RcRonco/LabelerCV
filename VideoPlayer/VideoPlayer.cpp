#include "VideoPlayer.h"

#include <ctime>
#include <sstream>
#include <boost/filesystem.hpp>

using namespace RCO;
using namespace RCO::Labeler;
namespace fs = boost::filesystem;

void timeTrackbarHandler(int pos, void* userdata) noexcept;
void mouseHandler(int event, int x, int y, int flags, void* param);

#define LABLER_SPACE  0x20
#define LABLER_BACK   0x08
#define LABLER_ESCAPE 0x1B

Labeler::VideoPlayer::VideoPlayer(std::string videoPath, const char * winname, std::string outputPath) : _path(videoPath), _WIN_NAME(winname)
{
	if (!loadVideo(videoPath))
		throw std::runtime_error("Can't load the video");

	cv::namedWindow(_WIN_NAME, CV_WINDOW_KEEPRATIO);
	cv::createTrackbar(_TIMEBAR_NAME, _WIN_NAME, 0, _vidlength, timeTrackbarHandler, reinterpret_cast<void*>(&_capture));
	cv::setMouseCallback(_WIN_NAME, mouseHandler, this);

	readImage();
	showImage();

	if (!fs::exists(fs::path(L"Dataset")))
		fs::create_directory(fs::path(L"Dataset"));

	_ofsTrainVal.open(outputPath, std::ios::app);
	if (!_ofsTrainVal.good()) {
		throw std::runtime_error("Unable to open trainval output file.");
	}
}

void Labeler::VideoPlayer::run()
{
	int key = 0;

	while ((cv::getWindowProperty(this->_WIN_NAME, CV_WND_PROP_FULLSCREEN) != -1) && !Shutdown)
	{
		for (int i = 0; !this->isVideoEnded() && this->isPlaying() && this->readImage(); i++)
			this->showImage();

		if (this->isPlaying())
			key = cv::waitKey();
		else
			key = cv::waitKey();

		this->keyAction((char)key);
	}

	cv::destroyAllWindows();
}

bool Labeler::VideoPlayer::isVideoEnded() noexcept
{
	return cv::getTrackbarPos(_TIMEBAR_NAME, _WIN_NAME) == _vidlength;
}

void  Labeler::VideoPlayer::keyAction(char key)
{
	if (key == '1' || key == 'T' || key == 't')
	{
		setLabel(Labeler::LabelType::Human);
	}
	else if (key == '2' || key == 'R' || key == 'r')
	{
		setLabel(Labeler::LabelType::Car);
	}
	else if (key == '3' || key == 'C' || key == 'c')
	{
		setLabel(Labeler::LabelType::Animal);
	}
	else if (key == LABLER_BACK)
	{
		getbackMat();
	}
	else if (key == LABLER_ESCAPE)
	{
		cv::destroyAllWindows();
		_exit(0);
	}
	else if (key == LABLER_SPACE || key == 'S' || key == 's')
	{

		if (this->isPlaying())
			this->_playing = false;
		else
		{
			SaveImage();
			this->_playing = true;
		}
	}
}

void Labeler::VideoPlayer::pushRect(LabeledRect & rect) noexcept
{
	_historyRects.push_back(rect);
}

void Labeler::VideoPlayer::changeTime(int seconds) noexcept
{
	if (seconds > 0 && seconds <= this->_vidlength)
		cv::setTrackbarPos(_TIMEBAR_NAME, _WIN_NAME, seconds);
}

bool Labeler::VideoPlayer::readImage() noexcept
{
	if (!_capture.read(_frameBuffer))
	{
		_playing = false;
		return false;
	}

	_foregroundMat = _frameBuffer.clone();
	_frameCounter++;

	return true;
}

void Labeler::VideoPlayer::showImage()
{
	if (cv::getWindowProperty(_WIN_NAME, CV_WND_PROP_FULLSCREEN) == -1)
	{
		Shutdown = true;
		return;
	}

	cv::imshow(_WIN_NAME, _frameBuffer);
	keyAction((char) cv::waitKey(_show_interval));

	if (_frameCounter % (int)_fps == 0) {

	    int time = cv::getTrackbarPos(_TIMEBAR_NAME, _WIN_NAME) + 1;
	     cv::setTrackbarPos(_TIMEBAR_NAME, _WIN_NAME, time );
	}
}

void Labeler::VideoPlayer::getbackMat()
{
	if (_historyRects.size() > 0)
	{
		_historyRects.pop_back();
		_foregroundMat = _frameBuffer.clone();

		for (LabeledRect rect : _historyRects)
		{
			cv::Scalar color;
			switch (rect.second)
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
			cv::rectangle(_foregroundMat, rect.first, color, 1);
		}

		cv::imshow(_WIN_NAME, _foregroundMat);
	}
}

void Labeler::VideoPlayer::SaveImage()
{
	std::stringstream imgFilename, data;
	std::string ctime = std::to_string((long)time(0));
	imgFilename << "Dataset/" << ctime.c_str() << ".jpg";
	data << "Dataset/" << ctime.c_str() << ".jpg;";
	for (LabeledRect rect : this->_historyRects)
	{
		switch (rect.second)
		{
			case LabelType::Human:
				data << 0;
				break;
			case LabelType::Car:
				data << 1;
				break;
			case LabelType::Animal:
				data << 2;
				break;
		}
		data << ":" << rect.first.x << ":" << rect.first.y << ":" << rect.first.width << ":" << rect.first.height << ";";
	}
	data << std::endl;
	cv::imwrite(imgFilename.str(), this->getFrame());
	if (_ofsTrainVal.is_open()) {
		_ofsTrainVal << data.rdbuf();
		_ofsTrainVal.flush();
	}
}

bool Labeler::VideoPlayer::loadVideo(std::string VideoPath)
{
	if (VideoPath != "")
		_path = VideoPath;
	_capture = cv::VideoCapture(_path);
	if (_capture.isOpened())
	{
		_fps = (uint32_t) _capture.get(CV_CAP_PROP_FPS);
		_frame_count = (uint32_t) _capture.get(CV_CAP_PROP_FRAME_COUNT);
		_vidlength = _frame_count / _fps;
		_show_interval = 1000 / _fps;

		return true;
	}
	else
		return false;
}

void timeTrackbarHandler(int pos, void* userdata) noexcept
{
	cv::VideoCapture* vid = reinterpret_cast<cv::VideoCapture*>(userdata);
	vid->set(CV_CAP_PROP_POS_MSEC, pos * 1000);
}

void mouseHandler(int event, int x, int y, int flags, void* param)
{
	VideoPlayer* videoPlayer = (VideoPlayer*)param;
	if (!videoPlayer->isPlaying())
	{
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

			cv::Mat frm = videoPlayer->getForegroundImage();

			if (videoPlayer->getPoint2().x < 0)
				videoPlayer->setPoint2(cv::Point(0, videoPlayer->getPoint2().y));
			else if (videoPlayer->getPoint2().x > frm.cols)
				videoPlayer->setPoint2(cv::Point(frm.cols, videoPlayer->getPoint2().y));
			if (videoPlayer->getPoint2().y < 0)
				videoPlayer->setPoint2(cv::Point(videoPlayer->getPoint2().x, 0));
			else if (videoPlayer->getPoint2().y > frm.rows)
				videoPlayer->setPoint2(cv::Point(videoPlayer->getPoint2().x, frm.rows));


			cv::Rect rect(videoPlayer->getPoint1(), videoPlayer->getPoint2());

			if (rect.size().area() > 100)
			{
				LabeledRect lblRect(rect, videoPlayer->getLabel());
				videoPlayer->pushRect(lblRect);
				cv::rectangle(videoPlayer->getForegroundImage(), rect.tl(), rect.br(), color, 1);
				cv::imshow(videoPlayer->getWindowName(), videoPlayer->getForegroundImage());
			}
		}
	}
}
