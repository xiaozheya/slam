// slam.cpp : 定义控制台应用程序的入口点。
//

#include <opencv/cv.hpp>
#include <iostream>
#include <conio.h>
#include <Skuld/OptionsParser.h>

using namespace Skuld;

int main(int argc, char** argv)
{
	OptionsParser mDesc(U"slam");

	mDesc
		(U"help", 'h', U"")
		(U"caribrate-image-count", U"", 1)
		(U"chess-board-size", U"", 2)
		(U"use-image-file", U"");

	std::vector<String> mArgv(argc);
	for (int i = 0; i < argc; i++) mArgv[i] = argv[i];
	OptionsVariableMap mVM;
	mDesc.Parse(mArgv.data(), argc, mVM);

	if (mVM.Has(U"help"))
	{
		std::cout << mDesc << std::endl;
		return 0;
	}

	uint32_t mImageCount = 1;
	if (mVM.Has(U"caribrate-image-count")) mImageCount = std::stoul(mVM(U"caribrate-image-count").GetStr());

	std::vector<cv::Mat> mLeftImages(mImageCount), mRightImages(mImageCount);

	//初始化标定板的像素坐标数据
	int cols = 7;
	int rows = 7;

	if (mVM.Has(U"chess-board-size"))
	{
		cols = std::stoul(mVM(U"chess-board-size", 0).GetStr());
		rows = std::stoul(mVM(U"chess-board-size", 1).GetStr());
	}

	float distance = 25;

	cv::Size mPatternSize(cols, rows);
	std::vector<cv::Point3f> mWorldPoints;

	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			mWorldPoints.push_back(cv::Point3f(i*distance, j*distance, 0));
		}
	}

	cv::Size mImageSize;

	if (mVM.Has(U"use-image-file"))
	{
		//读取图片数据
		for (uint32_t i = 0; i < mImageCount; i++)
		{
			mLeftImages[i] = cv::imread("left_image" + std::to_string(i) + ".jpg", cv::IMREAD_GRAYSCALE);
			if (mLeftImages[i].data == nullptr)
			{
				std::cout << "Can't read left image " << i << "." << std::endl;
				return -1;
			}
			if (i == 0) mImageSize = cv::Size(mLeftImages[i].size[0], mLeftImages[i].size[1]);
			else if (cv::Size(mLeftImages[i].size[0], mLeftImages[i].size[1]) != mImageSize)
			{
				std::cout << "Invalid left image " << i << "size" << std::endl;
				return -1;
			}

			mRightImages[i] = cv::imread("right_image" + std::to_string(i) + ".jpg", cv::IMREAD_GRAYSCALE);
			if (mRightImages[i].data == nullptr)
			{
				std::cout << "Can't read right image " << i << "." << std::endl;
				return -1;
			}
			if (cv::Size(mRightImages[i].size[0], mRightImages[i].size[1]) != mImageSize)
			{
				std::cout << "Invalid right image " << i << "size" << std::endl;
				return -1;
			}
		}
	}
	else
	{
		cv::VideoCapture mCamera[2] = {
			cv::VideoCapture(0),
			cv::VideoCapture(1)
		};


		if (!mCamera[0].isOpened() || !mCamera[1].isOpened())
		{
			std::cout << "Failed to init camera" << std::endl;
			return -1;
		}

		std::cout << "Press any key to capture image[0] from camera" << std::endl;
		getch();

		for (uint32_t i = 0; i < mImageCount; i++)
		{
			if (!mCamera[0].read(mLeftImages[i]))
			{
				std::cout << "Can't read image from camera[0]" << std::endl;
				return -1;
			}
			if (!mCamera[1].read(mRightImages[i]))
			{
				std::cout << "Can't read image from camera[1]" << std::endl;
				return -1;
			}

			cv::imshow(std::to_string(i) + "L", mLeftImages[i]);
			cv::imshow(std::to_string(i) + "R", mRightImages[i]);

			if (i != mImageCount - 1)
			{
				std::cout << "Press any key to capture image[" << i + 1 << "] from camera" << std::endl;
			}
			else
			{
				std::cout << "Press any key to continue" << std::endl;
			}
			cv::waitKey();
		}

		std::cout << "Begin to caribrate" << std::endl;
		mImageSize = cv::Size(mLeftImages[0].size[0], mLeftImages[0].size[1]);

		mCamera[0].release();
		mCamera[1].release();
	}
	//标定

	std::vector<std::vector<cv::Point2f>> mLeftCornersVect(mImageCount), mRightCornersVect(mImageCount);
	std::vector<std::vector<cv::Point3f>> mWorldPointVect(mImageCount);

	for (uint32_t i = 0; i < mImageCount; i++)
	{
		bool mLeftFindRet = cv::findChessboardCorners(mLeftImages[i], mPatternSize, mLeftCornersVect[i]);
		if (!mLeftFindRet)
		{
			std::cout << "Invalid left image " << i << "." << std::endl;
			return -1;
		}

		bool mRightFindRet = cv::findChessboardCorners(mRightImages[i], mPatternSize, mRightCornersVect[i]);
		if (!mRightFindRet)
		{
			std::cout << "Invalid right image " << i << "." << std::endl;
			return -1;
		}

		mWorldPointVect[i] = mWorldPoints;
	}

	cv::Mat mLeftCamera, mRightCamera;
	cv::Mat mLeftDistCoeffs, mRightDistCoeffs;

	cv::Mat R, T, E, F;
	cv::stereoCalibrate(mWorldPointVect, mLeftCornersVect, mRightCornersVect, mLeftCamera, mLeftDistCoeffs, mRightCamera, mRightDistCoeffs, mImageSize, R, T, E, F);



    return 0;
}

