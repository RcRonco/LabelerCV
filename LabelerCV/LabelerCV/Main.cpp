#include "General.h"
#include "VideoPlayer.h"

#include <Windows.h>
#include <opencv2\highgui.hpp>

LRESULT CALLBACK LLKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

Labeler::VideoPlayer* video;
MSG msg;
HHOOK hKeyboard = 0;
bool play_flag = true;

int main()
{
	//cv::namedWindow("POV - Dataset Slicer", cv::WINDOW_KEEPRATIO);
	video = new Labeler::VideoPlayer("I:\\RonCohen\\Desktop\\testvid.mp4", "POV - Dataset Slicer");
	SetWindowsHookEx(WH_KEYBOARD_LL, LLKeyboardProc, 0, 0);

	while (GetMessage(&msg,0,0,0))
	{
		/*if (GetAsyncKeyState(VK_SPACE) == -32767)
		{
			play_flag = !play_flag;
			cv::waitKey();
		}*/

		// If video done restart it.
		if (video->isVideoEnded())
			video->changeTime();

		for (int i = 0; play_flag && video->readImage() ; i++)
		{
			/*if (GetAsyncKeyState(VK_SPACE) == -32767)
			{
				play_flag = !play_flag;
				cv::waitKey();
			}*/

			video->showImage();
		}
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
		/*
		*   ptrKbdll->vkCode        ||
		*   TODO: Data saving here  ||
		*						   \  /
		*                           \/
		*/
		// TODO: dsasads
	}

	return (CallNextHookEx(hKeyboard, nCode, wParam, lParam));
}