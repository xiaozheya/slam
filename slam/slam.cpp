// slam.cpp : 定义控制台应用程序的入口点。
//

#include <opencv/cv.hpp>
#include <iostream>
#include <chrono>
#include <Skuld/OptionsParser.h>
#if defined(_WIN32)
#include <direct.h>
#include <conio.h>
#include <io.h>
#else
#include <unistd.h>
#endif
#include <thread>
#include <array>

#define SHOW_EDGE

using namespace Skuld;

cv::Mat mGlobalDisparity;
cv::Mat mGlobalQ;

static void CalcWorldPos(cv::Mat& mQ, cv::Mat& mDisparity, int x, int y, std::array<double, 3>& mOutput)
{
	cv::Mat mW_(4, 1, mQ.type());

	mW_.at<double>(0, 0) = (double)x;
	mW_.at<double>(1, 0) = (double)y;
	mW_.at<double>(2, 0) = (double)mDisparity.at<short>(y, x);
	mW_.at<double>(3, 0) = 1.0;

	cv::Mat mW = mQ * mW_;

	mOutput[0] = mW.at<double>(0, 0) / mW.at<double>(3, 0);
	mOutput[1] = mW.at<double>(1, 0) / mW.at<double>(3, 0);
	mOutput[2] = mW.at<double>(2, 0) / mW.at<double>(3, 0);
}

static void FindContours(cv::Mat& mGray, std::vector<std::vector<cv::Point>>& mContours)
{
	cv::Mat mTmp;
	double mThresh = cv::threshold(mGray, mTmp, 0, 255, cv::THRESH_OTSU);
	cv::Mat mEdge;
	cv::Canny(mGray, mEdge, mThresh, mThresh * 3);
	cv::findContours(mEdge, mContours, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
}

static void ProcessObject(cv::Mat& mTL, cv::Mat& mTR,
	cv::Mat& Q)
{
	cv::Mat mGrayL, mGrayR;
	cv::cvtColor(mTL, mGrayL, cv::COLOR_RGB2GRAY);
	cv::cvtColor(mTR, mGrayR, cv::COLOR_RGB2GRAY);

	cv::Ptr<cv::StereoBM> mStereoBM = cv::StereoBM::create();
	cv::Mat mDisparity;
	mStereoBM->compute(mGrayL, mGrayR, mDisparity);
	
	mGlobalQ = Q.clone();
	mGlobalDisparity = mDisparity.clone();

	std::vector<std::vector<cv::Point>> mLContours, mRContours;
	FindContours(mGrayL, mLContours);

	std::vector<std::vector<std::array<double, 3>>> mLineLists(mLContours.size());
	for (size_t i = 0; i < mLContours.size(); i++)
	{
		std::vector<cv::Point>& mContour = mLContours[i];
		std::vector<std::array<double, 3>>& mLineList = mLineLists[i];
		mLineList.resize(mContour.size());
		for (size_t j = 0; j < mContour.size(); j++)
		{
			CalcWorldPos(Q, mDisparity, mContour[j].x, mContour[j].y, mLineList[j]);
		}
	}


#ifdef SHOW_EDGE
	cv::Mat mContourImage = cv::Mat::zeros(mGrayL.size[0], mGrayL.size[1], mGrayL.type());

	for (size_t i = 0; i < mLContours.size(); i++)
		cv::drawContours(mContourImage, mLContours, (int)i, cv::Scalar(255, 0, 255));

	cv::imshow("LC", mContourImage);

	cv::setMouseCallback("LC", [](int _event, int x, int y, int flags, void* param) {
		if (_event == cv::EVENT_LBUTTONUP)
		{
			std::array<double, 3> mPos;
			CalcWorldPos(mGlobalQ, mGlobalDisparity, x, y, mPos);

			std::cout << mPos[0] << ", " << mPos[1] << ", " << mPos[2] << std::endl;
		}
	});
#endif
}

int main(int argc, char** argv)
{
	OptionsParser mDesc(U"slam");

	mDesc
		(U"help", 'h', U"")
		(U"caribrate-image-count", U"", 1)
		(U"chess-board-size", U"", 2)
		(U"caribrate-use-image-file", U"")
		(U"object-use-image-file", U"")
		(U"image-dir", U"", 1);

	std::vector<String> mArgv(argc);
	for (int i = 0; i < argc; i++) mArgv[i] = argv[i];
	OptionsVariableMap mVM;
	mDesc.Parse(mArgv.data(), argc, mVM);

	uint32_t mLeftIndex = 0, mRightIndex = 1;

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
	cv::VideoCapture mCamera[2];

	if (!mVM.Has(U"caribrate-use-image-file") || !mVM.Has(U"object-use-image-file"))
	{
		if (!mCamera[0].open(0) || !mCamera[1].open(1))
		{
			std::cout << "Failed to init camera" << std::endl;
			return -1;
		}

		cv::namedWindow("L");
		cv::namedWindow("R");
	}

	std::string mImageSavePath = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
	_mkdir(mImageSavePath.c_str());
	mImageSavePath += "/";

	if (mVM.Has(U"caribrate-use-image-file"))
	{
		std::string mImageDir = mVM.Has(U"image-dir") ? mVM(U"image-dir").GetStr() : ".";
		mImageDir += "/";
		//读取图片数据
		for (uint32_t i = 0; i < mImageCount; i++)
		{
			mLeftImages[i] = cv::imread(mImageDir + "left_image" + std::to_string(i) + ".jpg", cv::IMREAD_GRAYSCALE);
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

			mRightImages[i] = cv::imread(mImageDir + "right_image" + std::to_string(i) + ".jpg", cv::IMREAD_GRAYSCALE);
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
		uint32_t i = 0;

		std::cout << "Press any key to capture image[" << i << "] from camera" << std::endl;
		while (true)
		{
			cv::Mat mTL, mTR;
			if (!mCamera[mLeftIndex].read(mTL))
			{
				std::cout << "Can't read image from left camera" << std::endl;
				return -1;
			}
			if (!mCamera[mRightIndex].read(mTR))
			{
				std::cout << "Can't read image from right camera" << std::endl;
				return -1;
			}

			cv::imshow("L", mTL);
			cv::imshow("R", mTR);

			int mPress = cv::waitKey(16);

			if (mPress != -1)
			{
				std::cout << "Press key" << mPress << std::endl;
				mLeftImages[i] = mTL.clone();
				mRightImages[i] = mTR.clone();

				cv::imwrite(mImageSavePath + "left_image" + std::to_string(i) + ".jpg", mLeftImages[i]);
				cv::imwrite(mImageSavePath + "right_image" + std::to_string(i) + ".jpg", mRightImages[i]);

				i++;

				if (i == mImageCount)
				{
					std::cout << "Press any key to continue" << std::endl;
					break;
				}
				else
				{
					std::cout << "Press any key to capture image[" << i << "] from camera" << std::endl;
				}
			}
		}

		mImageSize = cv::Size(mLeftImages[0].size[0], mLeftImages[0].size[1]);
	}
	//标定

	std::cout << "Begin to caribrate" << std::endl;

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

	cv::Mat mLeftCamera = cv::Mat::ones(cv::Size(3, 3), CV_32S), mRightCamera = cv::Mat::ones(cv::Size(3, 3), CV_32S);
	cv::Mat mLeftDistCoeffs, mRightDistCoeffs;

	cv::Mat R, T, E, F;
	cv::stereoCalibrate(mWorldPointVect, mLeftCornersVect, mRightCornersVect, mLeftCamera, mLeftDistCoeffs, mRightCamera, mRightDistCoeffs, mImageSize, R, T, E, F);

	cv::Mat R1, R2, P1, P2, Q;
	cv::stereoRectify(mLeftCamera, mLeftDistCoeffs, mRightCamera, mRightDistCoeffs, mImageSize, R, T, R1, R2, P1, P2, Q);

	std::cout << "Q: " << Q << std::endl;

	if (mVM.Has(U"object-use-image-file"))
	{
		std::string mImageDir = mVM.Has(U"image-dir") ? mVM(U"image-dir").GetStr() : ".";
		mImageDir += "/";

		uint32_t mObjectIndex = 0;

		while (true)
		{
			cv::Mat mTL = cv::imread(mImageDir + "left_object" + std::to_string(mObjectIndex) + ".jpg", cv::IMREAD_GRAYSCALE);
			cv::Mat mTR = cv::imread(mImageDir + "right_object" + std::to_string(mObjectIndex) + ".jpg", cv::IMREAD_GRAYSCALE);

			if (mTL.data == nullptr || mTR.data == nullptr)
				return -1;

			ProcessObject(mTL, mTR, Q);
		}
	}
	else
	{
		uint32_t mObjectIndex = 0;
		std::cout << "Press 'q' to quit or other to capture" << std::endl;
		while (true)
		{
			cv::Mat mTL, mTR;
			if (!mCamera[mLeftIndex].read(mTL))
			{
				std::cout << "Can't read image from left camera" << std::endl;
				return -1;
			}
			if (!mCamera[mRightIndex].read(mTR))
			{
				std::cout << "Can't read image from right camera" << std::endl;
				return -1;
			}

			cv::imshow("L", mTL);
			cv::imshow("R", mTR);

			int mPress = cv::waitKey(16);

			if (mPress == 'q') break;
			else if (mPress != -1)
			{
				std::cout << "Press key" << mPress << std::endl;
				
				cv::imwrite(mImageSavePath + "left_object" + std::to_string(mObjectIndex) + ".jpg", mTL);
				cv::imwrite(mImageSavePath + "right_object" + std::to_string(mObjectIndex) + ".jpg", mTR);

				ProcessObject(mTL, mTR, Q);

				std::cout << "Press 'q' to quit or other to capture" << std::endl;
			}
		}
	}

    return 0;
}

