#ifndef _RC_VIDEO_PLAYER_H
#define _RC_VIDEO_PLAYER_H
#include "General.h"

#include <stdint.h>
#include <opencv.hpp>
#include <opencv2\highgui.hpp>
#include <stack>

namespace Labeler
{
	enum class LabelType { Human, Car, Animal};
	class VideoPlayer
	{
		private:
			const char* _TIMEBAR_NAME = "Timeline";
			const char* _WIN_NAME;
			std::string _path;
			std::stack<cv::Mat> _historyMat;
			//cv::Mat _foregroundImg;
			cv::Mat _frameBuffer;
			cv::VideoCapture _capture;
			double fps, frame_count, vidlength, show_interval;
			uint64_t _frameCounter = 0, cropIndex = 0;
			cv::Point point1, point2;
			bool drag = false;
			LabelType label = LabelType::Human;

		public:
			VideoPlayer() = default;
			VideoPlayer(std::string videoPath, const char* winname);

			bool isMouseDragging() { return drag; }
			void setMouseDragging(bool state) { drag = state; }
			bool isVideoEnded();
			void changeTime(int seconds = 0);
			bool readImage();
			void showImage();

			cv::Mat getFrame() { return _frameBuffer; }
			cv::Mat getForegroundImage() { return _historyMat.top(); }
			void setPoint1(cv::Point point) { point1 = point; }
			void setPoint2(cv::Point point) { point2 = point; }
			cv::Point getPoint1() { return point1; }
			cv::Point getPoint2() { return point2; }
			uint64_t  getCropCounter() { return cropIndex; }
			uint64_t  increaseCropCounter() { cropIndex++;  return cropIndex; }
			const char* getWindowName() const { return _WIN_NAME; }
			void setLabel(LabelType lbtype) { label = lbtype; }
			LabelType getLabel() { return label; }
			void pushMat(cv::Mat& mt) { _historyMat.push(mt); }
			void getbackMat() 
			{ 
 				if (_historyMat.size() > 1)
				{
					_historyMat.pop(); 
					cv::imshow(_WIN_NAME, _historyMat.top());
				}
			}
		private:
			bool loadVideo(std::string VideoPath);
	};
};

#endif // !_RC_VIDEO_PLAYER_H

