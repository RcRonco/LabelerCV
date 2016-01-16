#ifndef RC_GENERAL_H
#define RC_GENERAL_H

#include <iostream>
#include <string>
#include <opencv.hpp>

using namespace std;

namespace Labeler
{
	#ifdef _UNICODE
		typedef wchar_t rcchar;
		typedef std::wstring rcstring;
	#else
		typedef char rcchar;
		typedef std::string rcstring;
	#endif

	//const char* OPCV_WIN_NAME = "POV - Dataset Slicer";
	//const char* OPCV_TB_TIME = "POV - Timebar";

	const cv::Scalar BLUE(255, 0, 0);
	const cv::Scalar GREEN(0, 255, 0);
	const cv::Scalar RED(0, 0, 255);
};


#endif // !RC_GENERAL_H

