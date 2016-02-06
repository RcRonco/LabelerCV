#include "VideoPlayer.h"
#include <Windows.h>
#include <opencv2\highgui.hpp>

LRESULT CALLBACK Winhandle(int nCode, WPARAM wparam, LPARAM lp);

const char * window_name = "PoV - Dataset Slicer";

Labeler::VideoPlayer* video;
MSG msg;
HHOOK hWin = 0;

int main(int argc, char** argv)
{
	FreeConsole();
	
	if (argc != 2)
		return -1;

	int c = 0;

	video = new Labeler::VideoPlayer(argv[1], window_name);

	hWin = SetWindowsHookEx(WH_CALLWNDPROC, Winhandle, 0, GetCurrentThreadId());

	while (GetMessage(&msg, 0, 0, 0))
	{
		std::cout << cv::getWindowProperty(window_name, 0) << std::endl;

		for (int i = 0; video->isPlaying && video->readImage(); i++)
			video->showImage();

		if (video->isPlaying)
			c = cv::waitKey(2);
		else
			c = cv::waitKey();

		video->keyAction((char)c);
	}

	cv::destroyAllWindows();
	UnhookWindowsHookEx(hWin);

	return 0;
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