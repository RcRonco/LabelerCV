#include "General.h"
#include "VideoPlayer.h"

#include <Windows.h>
#include <opencv2\highgui.hpp>

LRESULT CALLBACK LLKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(int nCode, WPARAM wParam, LPARAM lParam);

Labeler::VideoPlayer* video;
MSG msg;
HHOOK hKeyboard = 0, hWin = 0;
bool play_flag = true;
bool shutdown_flag = false;
const char * window_name = "POV - Dataset Slicer";

int main(int argc, char** argv )
{
	/*if (argc != 2)
		return -1;

	video = new Labeler::VideoPlayer(argv[1], window_name);*/
	video = new Labeler::VideoPlayer("I:\\RonCohen\\Desktop\\testvid.mp4", window_name);
	hKeyboard = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, 0, GetCurrentThreadId());

	if (hKeyboard || hWin)
	{
		std::cout << "Failed to initialize hooks" << std::endl
			      << "Error code: "<< std::to_string(GetLastError()) << std::endl;
		return -1;
	}

	while (GetMessage(&msg,0,0,0) && !shutdown_flag)
	{
		for (int i = 0; play_flag && video->readImage() ; i++)
			video->showImage();

		cv::waitKey();
	}

	UnhookWindowsHookEx(hKeyboard);
}
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (wParam == VK_SPACE)
		{
			play_flag = !play_flag;
			video->CutImages();
		}
		else if (wParam == '1')
		{
			video->setLabel(Labeler::LabelType::Human);
		}
		else if (wParam == '2')
		{
			video->setLabel(Labeler::LabelType::Car);
		}
		else if (wParam  == '3')
		{
			video->setLabel(Labeler::LabelType::Animal);
		}
		else if (wParam == 'Z' && GetAsyncKeyState(VK_CONTROL) == -32767)
		{
			video->getbackMat();
		}
		else if (wParam == VK_ESCAPE)
		{
			shutdown_flag = true;
			cv::destroyAllWindows();
		}
	}

	return (CallNextHookEx(hKeyboard, nCode, wParam, lParam));
}
LRESULT CALLBACK LLKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if ((nCode == HC_ACTION) && ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP)))
	{
		KBDLLHOOKSTRUCT* ptrKbdll = (KBDLLHOOKSTRUCT*)lParam;

		if (ptrKbdll->vkCode == VK_SPACE)
		{
			play_flag = !play_flag;
			video->CutImages();
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
		else if (ptrKbdll->vkCode == 'Z' && GetAsyncKeyState(VK_CONTROL) == -32767)
		{
			video->getbackMat();
		}
		else if (ptrKbdll->vkCode == VK_ESCAPE)
		{
			shutdown_flag = true;
			cv::destroyAllWindows();
		}
	}

	return (CallNextHookEx(hKeyboard, nCode, wParam, lParam));
}