// LapalicanClass.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "LapalicanClass.h"


void LapalicanClass::buildLaplacianPyramid(const cuda::GpuMat& img, vector<cuda::GpuMat >& lapPyr)
{
	lapPyr.clear();
	cuda::GpuMat currentImg = img;
	for (int l = 0; l<levels; l++)
	{
		cuda::GpuMat down, up, lap;
		cuda::pyrDown(currentImg, down);
		cuda::pyrUp(down, up);
		//cuda::GpuMat lap = currentImg - up;
		cuda::subtract(currentImg, up, lap);

		lapPyr.push_back(lap);
		currentImg = down;
		
	}
}

//void LapalicanClass::buildGaussianPyramid(const cuda::GpuMat& img, vector <cuda::GpuMat>& gauPyr)
//{
//	gauPyr.clear();
//	cuda::GpuMat currentImg = img;
//	
//	gauPyr.push_back(currentImg);
//	for (int l = 1; l < levels; l++)
//	{
//		cuda::GpuMat down;
//		cuda::pyrDown(currentImg, down);
//		gauPyr.push_back(down);
//		currentImg = down;
//	}
//}

void LapalicanClass::buildGaussianPyramid(const cuda::GpuMat& img, vector <cuda::GpuMat>& gauPyr)
{
	gauPyr.clear();
	cuda::GpuMat currentImg = img;

	gauPyr.push_back(currentImg);
	for (int l = 1; l < levels; l++)
	{
		cuda::GpuMat down;
		cuda::pyrDown(currentImg, down);
		gauPyr.push_back(down);
		currentImg = down;
	}
}

void LapalicanClass::blendInEachLevel()
{
	cuda::GpuMat down;
	Mat imgshow;

	for (int l = 0; l < levels; l++)
	{
		fgLapPyr[l].copyTo(bgLapPyr[l], maskGauPyramid[l]);
		fgGauPyramid[l].copyTo(bgGauPyramid[l], maskGauPyramid[l]);
		//bgLapPyr[l].download(imgshow);
		//imshow("gpushow", imgshow);
		//waitKey(100);
	}

}

Mat LapalicanClass::reconstructImg()
{
	gpu_targetImg = bgGauPyramid[levels - 1];
	cuda::pyrDown(gpu_targetImg, gpu_targetImg);
	cuda::GpuMat up;
	for (int l = levels - 1; l >= 0; l--)
	{
		cuda::pyrUp(gpu_targetImg, up);
		//gpu_targetImg = up + bgLapPyr[l];
		cuda::add(up, bgLapPyr[l], gpu_targetImg);
	}
	gpu_targetImg.download(targetImg);
	return targetImg;
}

Mat LapalicanClass::Run()
{
	buildLaplacianPyramid(gpu_backgroundImg, bgLapPyr);	
	buildLaplacianPyramid(gpu_foregroundImg, fgLapPyr);	

	buildGaussianPyramid(gpu_foregroundImg, fgGauPyramid);	
	buildGaussianPyramid(gpu_fgMask, maskGauPyramid);
	buildGaussianPyramid(gpu_backgroundImg, bgGauPyramid);

	blendInEachLevel();
	return reconstructImg();		
}

void LapalicanClass::setBGImage(Mat img)
{
	backgroundImg = NULL;
	img.convertTo(backgroundImg, CV_32F, 1.0 / 255.0);
	gpu_backgroundImg.upload(backgroundImg);
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
	gpu_foregroundImg.upload(foregroundImg);
	if (foregroundImg.empty())
	{
		cout << "ERROR: Cannot Load foregroundImg Image!" << endl;
		return ;
	}
}

void LapalicanClass::setMaskImage(Mat img)
{
	fgMask = NULL;
	//img.convertTo(fgMask, CV_32F, 1.0 / 255.0);
	fgMask = img;
	gpu_fgMask.upload(fgMask);
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

