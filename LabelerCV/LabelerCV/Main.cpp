#include "General.h"
#include "VideoPlayer.h"

#include <Windows.h>
#include <opencv2\highgui.hpp>

LRESULT CALLBACK LLKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

Labeler::VideoPlayer* video;
MSG msg;
HHOOK hKeyboard = 0;
bool play_flag = true;
const char * window_name = "POV - Dataset Slicer";
int main()
{
	video = new Labeler::VideoPlayer("I:\\RonCohen\\Desktop\\testvid.mp4", window_name);
	SetWindowsHookEx(WH_KEYBOARD_LL, LLKeyboardProc, 0, 0);

	while (GetMessage(&msg,0,0,0))
	{
		// If video done restart it.
		if (video->isVideoEnded())
			video->changeTime();

		for (int i = 0; play_flag && video->readImage() ; i++)
			video->showImage();

		cv::waitKey();
	}
}

LRESULT CALLBACK LLKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if ((nCode == HC_ACTION) && ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)))
	{
		KBDLLHOOKSTRUCT* ptrKbdll = (KBDLLHOOKSTRUCT*)lParam;

		if (ptrKbdll->vkCode == VK_SPACE)
		{
			play_flag = !play_flag;
		//	cv::waitKey();
		}
		else if (ptrKbdll->vkCode == '1')
		{
			video->setLabel(Labeler::LabelType::Human);
		}
		else if (ptrKbdll->vkCode == '2')
		{
			video->setLabel(Labeler::LabelType::Car);
		}
		else if (ptrKbdll->vkCode == '3')
		{
			video->setLabel(Labeler::LabelType::Animal);
		}
		else if (ptrKbdll->vkCode == 'Z')
		{
			video->getbackMat();
		}
	/*	else if (ptrKbdll->vkCode == VK_ESCAPE)
		{
			if (!cv::getWindowProperty(window_name, CV_WND_PROP_FULLSCREEN))
				cv::setWindowProperty(window_name, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
			else
			{
				cv::setWindowProperty(window_name, CV_WND_PROP_FULLSCREEN, 0);
				cv::setWindowProperty(window_name, CV_WINDOW_KEEPRATIO, 1);
			}
		}*/
	}

	return (CallNextHookEx(hKeyboard, nCode, wParam, lParam));
}