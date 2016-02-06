#ifndef _RC_VIDEO_PLAYER_H
#define _RC_VIDEO_PLAYER_H

#include <stdint.h>
#include <opencv.hpp>
#include <vector>

namespace Labeler
{
	enum class LabelType { Human, Car, Animal };

	const cv::Scalar BLUE(255, 0, 0);
	const cv::Scalar GREEN(0, 255, 0);
	const cv::Scalar RED(0, 0, 255);

	using RectType = std::pair<cv::Rect, LabelType>;

	class VideoPlayer
	{
	private:
		const char* _TIMEBAR_NAME = "Timeline";
		const char* _WIN_NAME;
		std::string _path;
		cv::Mat _foregroundMat;
		std::vector<RectType> _historyRects;
		cv::Mat _frameBuffer;
		cv::VideoCapture _capture;
		double fps, frame_count, vidlength, show_interval;
		uint64_t _frameCounter = 0, cropIndex = 0;
		cv::Point point1, point2;
		bool drag = false;
		LabelType label = LabelType::Human;
	public:
		bool isPlaying = false;

	public:
		VideoPlayer() = default;
		VideoPlayer(std::string videoPath, const char* winname);

		bool isMouseDragging() { return drag; }
		void setMouseDragging(bool state) { drag = state; }
		bool isVideoEnded();
		void changeTime(int seconds = 0);
		bool readImage();
		void showImage();
		void CutImages();
		void keyAction(char key);

		cv::Mat getFrame() { return _frameBuffer; }
		cv::Mat getForegroundImage() { return _foregroundMat; }
		void setPoint1(cv::Point point) { point1 = point; }
		void setPoint2(cv::Point point) { point2 = point; }
		cv::Point getPoint1() { return point1; }
		cv::Point getPoint2() { return point2; }
		uint64_t  getCropCounter() { return cropIndex; }
		uint64_t  increaseCropCounter() { cropIndex++;  return cropIndex; }
		const char* getWindowName() const { return _WIN_NAME; }
		void setLabel(LabelType lbtype) { label = lbtype; }
		LabelType getLabel() { return label; }
		void pushRect(RectType& rect) { _historyRects.push_back(rect); }
		void getbackMat();

	private:
		bool loadVideo(std::string VideoPath);
	};
};

#endif // !_RC_VIDEO_PLAYER_H