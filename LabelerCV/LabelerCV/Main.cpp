#include "VideoPlayer.h"
#include <Windows.h>
#include <opencv2\highgui.hpp>

LRESULT CALLBACK LLKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Winhandle(int nCode, WPARAM wparam, LPARAM lp);

const char * window_name = "PoV - Dataset Slicer";

Labeler::VideoPlayer* video;
MSG msg;
HHOOK hKeyboard = 0, hWin = 0;

int main(int argc, char** argv)
{
	FreeConsole();
	
	if (argc != 2)
		return -1;

	video = new Labeler::VideoPlayer(argv[1], window_name);

	hKeyboard = SetWindowsHookEx(WH_KEYBOARD_LL, LLKeyboardProc, 0, 0);
	hWin = SetWindowsHookEx(WH_CALLWNDPROC, Winhandle, 0, GetCurrentThreadId());

	while (GetMessage(&msg, 0, 0, 0))
	{
		std::cout << cv::getWindowProperty(window_name, 0) << std::endl;

		for (int i = 0; video->isPlaying && video->readImage(); i++)
			video->showImage();
		if (video->isPlaying)
			cv::waitKey(2);
		else
			cv::waitKey();
	}

	cv::destroyAllWindows();
	UnhookWindowsHookEx(hKeyboard);
	UnhookWindowsHookEx(hWin);

	return 0;
}

LRESULT CALLBACK LLKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if ((nCode == HC_ACTION) && ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)))
	{
		KBDLLHOOKSTRUCT* ptrKbdll = (KBDLLHOOKSTRUCT*)lParam;

		if (ptrKbdll->vkCode == VK_SPACE)
		{
			video->CutImages();
			video->isPlaying = !video->isPlaying;
		}
		else if (ptrKbdll->vkCode == '1' || ptrKbdll->vkCode == 'T')
		{
			video->setLabel(Labeler::LabelType::Human);
		}
		else if (ptrKbdll->vkCode == '2' || ptrKbdll->vkCode == 'R')
		{
			video->setLabel(Labeler::LabelType::Car);
		}
		else if (ptrKbdll->vkCode == '3' || ptrKbdll->vkCode == 'C')
		{
			video->setLabel(Labeler::LabelType::Animal);
		}
		else if (ptrKbdll->vkCode == VK_BACK)
		{
			video->getbackMat();
		}
		else if (ptrKbdll->vkCode == VK_ESCAPE)
		{
			cv::destroyAllWindows();
			exit(0);
		}
	}

	return (CallNextHookEx(hKeyboard, nCode, wParam, lParam));
}
LRESULT CALLBACK Winhandle(int nCode, WPARAM wparam, LPARAM lp)
{
	CWPSTRUCT cpstruct = *(CWPSTRUCT*)lp;

	if (cpstruct.message == WM_CLOSE)
	{
		cv::destroyAllWindows();
		exit(0);
	}

	return (CallNextHookEx(hWin, nCode, wparam, lp));
}