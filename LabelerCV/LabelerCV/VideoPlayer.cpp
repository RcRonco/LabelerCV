#include "VideoPlayer.h"

#include <ctime>
#include <sstream>
#include <boost\filesystem.hpp>

using namespace Labeler;

void timeTrackbarHandler(int pos, void* userdata) noexcept;
void mouseHandler(int event, int x, int y, int flags, void* param);

#define LABLER_SPACE  0x20
#define LABLER_BACK   0x08
#define LABLER_ESCAPE 0x1B

Labeler::VideoPlayer::VideoPlayer(std::string videoPath, const char * winname, std::string outputPath) : _path(videoPath), _WIN_NAME(winname) 
{
	if (!loadVideo(videoPath))
		throw std::exception("Can't load the video");

	cv::namedWindow(_WIN_NAME, cv::WINDOW_KEEPRATIO);
	cv::createTrackbar(_TIMEBAR_NAME, _WIN_NAME, 0, vidlength, timeTrackbarHandler, reinterpret_cast<void*>(&_capture));
	cv::setMouseCallback(_WIN_NAME, mouseHandler, this);

	readImage();
	showImage();

	if (!boost::filesystem::exists(boost::filesystem::path(L"Full")))
		boost::filesystem::create_directory(boost::filesystem::path(L"Full"));
	if (!boost::filesystem::exists(boost::filesystem::path(L"Humans")))
		boost::filesystem::create_directory(boost::filesystem::path(L"Humans"));
	if (!boost::filesystem::exists(boost::filesystem::path(L"Cars")))
		boost::filesystem::create_directory(boost::filesystem::path(L"Cars"));
	if (!boost::filesystem::exists(boost::filesystem::path(L"Animals")))
		boost::filesystem::create_directory(boost::filesystem::path(L"Animals"));

	_ofsTrainVal.open(outputPath, std::ios::app);
	if (!_ofsTrainVal.good()) {
		throw std::exception("Unable to open trainval output file.");
	}
}

void Labeler::VideoPlayer::run()
{
	int key = 0;

	while ((cv::getWindowProperty(this->_WIN_NAME, CV_WND_PROP_FULLSCREEN) != -1) && !Shutdown)
	{
		for (int i = 0; this->isPlaying && this->readImage(); i++)
			this->showImage();

		if (this->isPlaying)
			key = cv::waitKey(2);
		else
			key = cv::waitKey();

		this->keyAction((char)key);
	}

	cv::destroyAllWindows();
}

bool Labeler::VideoPlayer::isVideoEnded() noexcept
{
	return cv::getTrackbarPos(_TIMEBAR_NAME, _WIN_NAME) == vidlength;
}

void  Labeler::VideoPlayer::keyAction(char key)
{
	if (key == LABLER_SPACE)
	{
		CutImages();
		isPlaying = !isPlaying;
	}
	else if (key == '1' || key == 'T' || key == 't')
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
		exit(0);
	}
	else if (key == 'S' || key == 's')
	{
		if (!this->isPlaying)
		{
			SaveImage();
			this->isPlaying = true;
		}
	}
}

void Labeler::VideoPlayer::pushRect(LabeledRect & rect) noexcept
{
	_historyRects.push_back(rect);
}

void Labeler::VideoPlayer::changeTime(int seconds) noexcept
{
	if (seconds > 0 && seconds <= this->vidlength)
		cv::setTrackbarPos(_TIMEBAR_NAME, _WIN_NAME, seconds);
}

bool Labeler::VideoPlayer::readImage() noexcept
{
	if (!_capture.read(_frameBuffer))
	{
		isPlaying = false;
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
	keyAction(cv::waitKey(show_interval));

	if (_frameCounter % (int)fps == 0)
		cv::setTrackbarPos(_TIMEBAR_NAME, _WIN_NAME, cv::getTrackbarPos(_TIMEBAR_NAME, _WIN_NAME) + 1);
}

void Labeler::VideoPlayer::CutImages()
{
	uint32_t imgCounter = 0;

	while (!_historyRects.empty())
	{
 		std::stringstream strm;

		switch (_historyRects[_historyRects.size() - 1].second)
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

		cv::Mat img = getFrame()(_historyRects[_historyRects.size() - 1].first);
		if (img.data)
		{
			imgCounter++;
			strm << std::to_string(time(0)) << "_" << std::to_string(imgCounter) << ".png";
			std::cout << strm.str().c_str() << std::endl;

			if (!cv::imwrite(strm.str(), img))
				throw std::exception("Unable to save the picture");
			_historyRects.pop_back();
		}
	}

	std::vector<LabeledRect> empt;
	std::swap(_historyRects, empt);
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
	/*std::stringstream imgFilename, datafileName, data;
	imgFilename << "Full\\" << std::to_string(time(0)) << ".jpg";
	datafileName << "Full\\" << std::to_string(time(0)) << ".dat";
	data << "Type:x:y:width:height" << std::endl;
	for (LabeledRect rect : this->_historyRects)
	{
		switch (rect.second)
		{
		case LabelType::Human:
			data << "Humans:";
			break;
		case LabelType::Car:
			data << "Cars:";
			break;
		case LabelType::Animal:
			data << "Animals:";
			break;
		}

		data << rect.first.x << ":" << rect.first.y << ":" << rect.first.width << ":" << rect.first.height << std::endl;
	}

	cv::imwrite(imgFilename.str(), this->getFrame());
	std::ofstream ofs(datafileName.str(), std::ios::out);
	if (ofs.is_open())
		ofs << data.rdbuf();

	ofs.close();*/

	std::stringstream imgFilename, data;
	imgFilename << "Full\\" << std::to_string(time(0)) << ".jpg";
	data << "Full\\" << std::to_string(time(0)) << ".jpg;";
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

void timeTrackbarHandler(int pos, void* userdata) noexcept
{
	cv::VideoCapture* vid = reinterpret_cast<cv::VideoCapture*>(userdata);
	vid->set(cv::CAP_PROP_POS_MSEC, pos * 1000);
}

void mouseHandler(int event, int x, int y, int flags, void* param)
{
	VideoPlayer* videoPlayer = (VideoPlayer*)param;
	if (!videoPlayer->isPlaying)
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
				videoPlayer->pushRect(LabeledRect(rect, videoPlayer->getLabel()));
				cv::rectangle(videoPlayer->getForegroundImage(), rect, color, 1);
				cv::imshow(videoPlayer->getWindowName(), videoPlayer->getForegroundImage());
			}
		}
	}
}