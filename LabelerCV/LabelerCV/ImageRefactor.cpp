#include "ImageRefactor.h"
#include <opencv.hpp>

void CopyDir(fs::path inpath, fs::path outpath, uint32_t depth = 0)
{
	fs::path workPath = outpath;

	if (depth != 0)
		workPath.append(inpath.filename().c_str());
	if (fs::exists(inpath))
	{
		if (fs::is_directory(inpath))
		{
			fs::directory_iterator end_iter;
			fs::create_directories(workPath);

			for (fs::directory_iterator dir_iter = fs::directory_iterator(inpath); dir_iter != end_iter; ++dir_iter)
			{
				CopyDir(dir_iter->path(), workPath, depth + 1);
				std::wclog << "Current depth: " << depth << " - " << workPath.c_str() << std::endl;
			}
		}
		else if (!fs::exists(workPath))
		{
			fs::copy_file(inpath, workPath);
		}
		else
		{
			std::wcerr << "The file " << workPath.c_str() << " already exists, override files not supported." << std::endl;
		}
	}
}

void Labeler::ImageRefactor::ResizeImages(fs::path path, uint32_t width, uint32_t height)

{
	if (fs::exists(path) && fs::is_directory(path))
	{
		fs::directory_iterator end_iter;

		for (fs::directory_iterator dir_iter = fs::directory_iterator(path); dir_iter != end_iter; ++dir_iter)
		{
			ResizeImages(dir_iter->path(), width, height);
		}
	}
	else
	{
		cv::Mat tempMat = cv::imread(path.string(), 1);
		cv::resize(tempMat.clone(), tempMat, cv::Size(width, height));
		cv::imwrite(path.string(), tempMat);
	}
}

void Labeler::ImageRefactor::ResizeImages(fs::path inpath, fs::path outpath, uint32_t width, uint32_t height)
{
	if (!fs::exists(inpath))
		return;

	if (fs::is_directory(inpath))
		CopyDir(inpath, outpath);
	else
	{
		fs::create_directory(outpath);
		outpath = outpath.append(inpath.filename().c_str()).append(inpath.extension().c_str());
		fs::copy_file(inpath, outpath);
	}

	ResizeImages(outpath, width, height);
}
