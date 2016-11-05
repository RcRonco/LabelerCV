#ifndef _RC_VIDEO_PLAYER_H
#define _RC_VIDEO_PLAYER_H

#include <stdint.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>

namespace RCO {
    namespace Labeler {
        enum class LabelType {
            Human, Car, Animal
        };

        const cv::Scalar BLUE(255, 0, 0);
        const cv::Scalar GREEN(0, 255, 0);
        const cv::Scalar RED(0, 0, 255);

        using LabeledRect = std::pair<cv::Rect, LabelType>;
        static const char *szVideoDescription = "Run Dataset video player\nSPACE - Stop/Start the video.\nBACK - Remove last label. (Undo)\nESCAPE - Exit\nS\\s - Save image to train format.\n1\\T\\t - Set label to Human.\n2\\R\\r - Set label to Car.\n3\\C\\c - Set label to Animal.";

        class VideoPlayer {
        private:
            const char *_TIMEBAR_NAME = "Timeline";
            const char *_WIN_NAME;
            std::string _path;
            std::ofstream _ofsTrainVal;
            cv::Mat _foregroundMat;
            std::vector<LabeledRect> _historyRects;
            cv::Mat _frameBuffer;
            cv::VideoCapture _capture;
            uint32_t _fps, _frame_count, _vidlength, _show_interval;
            uint32_t _frameCounter = 0;//, cropIndex = 0;
            cv::Point _point1, _point2;
            bool _is_dragging = false, _playing = false, Shutdown = false;
            LabelType _current_label = LabelType::Human;

        public:
            VideoPlayer(std::string videoPath, const char *winname, std::string outputPath = "Dataset/trainval.txt");

            void run();

            bool isVideoEnded() noexcept;

            void changeTime(int seconds = 0) noexcept;

            bool readImage() noexcept;

            void showImage();

            void SaveImage();

            void getbackMat();

            void keyAction(char key);

            void pushRect(LabeledRect &rect) noexcept;

            // Access Methods
            cv::Mat &getFrame() noexcept { return _frameBuffer; }

            cv::Mat &getForegroundImage() noexcept { return _foregroundMat; }

            const bool isMouseDragging() noexcept { return _is_dragging; }

            void setMouseDragging(bool state) noexcept { _is_dragging = state; }

            const cv::Point getPoint1() noexcept { return _point1; }

            void setPoint1(cv::Point point) noexcept { _point1 = point; }

            const cv::Point getPoint2() noexcept { return _point2; }

            void setPoint2(cv::Point point) noexcept { _point2 = point; }

            void setLabel(LabelType lbtype) noexcept { _current_label = lbtype; }

            const LabelType getLabel() noexcept { return _current_label; }

            const bool isPlaying() const noexcept { return _playing; }

            const char *getWindowName() const noexcept { return _WIN_NAME; }

        private:
            bool loadVideo(std::string VideoPath = "");

        };
    };
};
#endif // !_RC_VIDEO_PLAYER_H