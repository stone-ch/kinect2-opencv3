// LapalicanClass.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "LapalicanClass.h"


void LapalicanClass::buildLaplacianPyramid(const Mat_<Vec3f>& img, vector<Mat_<Vec3f> >& lapPyr)
{
	lapPyr.clear();
	Mat currentImg = img;
	for (int l = 0; l<levels; l++) {
		Mat down, up;
		pyrDown(currentImg, down);
		pyrUp(down, up, currentImg.size());
		Mat lap = currentImg - up;

		lapPyr.push_back(lap);
		currentImg = down;
		
	}
}

void LapalicanClass::buildGaussianPyramid(const Mat_<Vec3f>& img, vector < Mat_<Vec3f> >& gauPyr)
{
	gauPyr.clear();
	Mat currentImg = img;
	
	gauPyr.push_back(currentImg);
	for (int l = 1; l < levels; l++)
	{
		Mat down;
		pyrDown(currentImg, down);
		gauPyr.push_back(down);
		currentImg = down;
	}
}

void LapalicanClass::buildGaussianPyramid(const Mat& img, vector < Mat_<Vec3b> >& gauPyr)
{
	gauPyr.clear();
	Mat currentImg = img;

	gauPyr.push_back(currentImg);
	for (int l = 1; l < levels; l++)
	{
		Mat down;
		pyrDown(currentImg, down);
		gauPyr.push_back(down);
		currentImg = down;
	}
}

void LapalicanClass::blendInEachLevel()
{
	Mat down;

	for (int l = 0; l < levels; l++)
	{
		fgLapPyr[l].copyTo(bgLapPyr[l], maskGauPyramid[l]);
		fgGauPyramid[l].copyTo(bgGauPyramid[l], maskGauPyramid[l]);
	}

}

Mat LapalicanClass::reconstructImg()
{
	targetImg = bgGauPyramid[levels - 1];
	pyrDown(targetImg, targetImg);
	Mat up;
	for (int l = levels - 1; l >= 0; l--)
	{
		pyrUp(targetImg, up);
		targetImg = up + bgLapPyr[l];	
	}
	return targetImg;
}

Mat LapalicanClass::Run()
{
	
	buildLaplacianPyramid(backgroundImg, bgLapPyr);
	
	buildLaplacianPyramid(foregroundImg, fgLapPyr);
	
	buildGaussianPyramid(foregroundImg, fgGauPyramid);
	
	buildGaussianPyramid(fgMask, maskGauPyramid);
	
	buildGaussianPyramid(backgroundImg, bgGauPyramid);
	
	
	blendInEachLevel();
	
	return reconstructImg();
		
}

void LapalicanClass::setBGImage(Mat img)
{
	backgroundImg = NULL;
	img.convertTo(backgroundImg, CV_32F, 1.0 / 255.0);
	if (backgroundImg.empty())
	{
		cout << "ERROR: Cannot Load background Image!" << endl;
		return ;
	}
}

void LapalicanClass::setFGImage(Mat img)
{
	foregroundImg = NULL;
	img.convertTo(foregroundImg, CV_32F, 1.0 / 255.0);
	if (foregroundImg.empty())
	{
		cout << "ERROR: Cannot Load foregroundImg Image!" << endl;
		return ;
	}
}

void LapalicanClass::setMaskImage(Mat img)
{
	fgMask = NULL;
	img.convertTo(fgMask, CV_32F, 1.0 / 255.0);
	if (fgMask.empty())
	{
		cout << "ERROR: Cannot Load fgMask Image!" << endl;
		return ;
	}
}


LapalicanClass::LapalicanClass()
{
}

LapalicanClass::~LapalicanClass()
{
}

