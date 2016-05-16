// LapalicanClass.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "LapalicanClass.h"


void LapalicanClass::buildLaplacianPyramid(const Mat& img, vector<Mat_<Vec3b> >& lapPyr)
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

void LapalicanClass::reconstructImg()
{
	targetImg = bgGauPyramid[levels - 1];
	pyrDown(targetImg, targetImg);
	Mat up;
	for (int l = levels - 1; l >= 0; l--)
	{
		pyrUp(targetImg, up);
		targetImg = up + bgLapPyr[l];	
	}
	imshow("ta", targetImg);
	waitKey(0);
}

void LapalicanClass::Run()
{
	LoadImg();
	buildLaplacianPyramid(backgroundImg, bgLapPyr);
	buildLaplacianPyramid(foregroundImg, fgLapPyr);
	buildGaussianPyramid(foregroundImg, fgGauPyramid);
	buildGaussianPyramid(fgMask, maskGauPyramid);
	buildGaussianPyramid(backgroundImg, bgGauPyramid);
	
	blendInEachLevel();
	reconstructImg();
	
}

int LapalicanClass::LoadImg()
{
	backgroundImg = imread("e:/data/kinect/960x512bg.jpg");
	foregroundImg = imread("e:/data/kinect/960x512fg.jpg");
	fgMask = imread("e:/data/kinect/960x512mask.jpg");
	if (backgroundImg.empty())
	{
		cout << "ERROR: Cannot Load background Image!" << endl;
		return 0;
	}
	if (foregroundImg.empty())
	{
		cout << "ERROR: Cannot Load foregroundImg Image!" << endl;
		return 0;
	}
	if (fgMask.empty())
	{
		cout << "ERROR: Cannot Load fgMask Image!" << endl;
		return 0;
	}
}


int main()
{
	LapalicanClass lap;
	return 0;
}
