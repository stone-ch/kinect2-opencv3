#include "stdafx.h"

class LapalicanClass
{
public:
	LapalicanClass();
	~LapalicanClass();

private:
	Mat_<Vec3f> backgroundImg, foregroundImg;
	Mat fgMask;

	cuda::GpuMat gpu_backgroundImg, gpu_foregroundImg, gpu_fgMask;

	vector<cuda::GpuMat> bgLapPyr, fgLapPyr, resultLapPyr;
	vector<cuda::GpuMat> fgGauPyramid, bgGauPyramid;
	vector<cuda::GpuMat> maskGauPyramid;

	int levels = 2;

	Mat targetImg;
	cuda::GpuMat gpu_targetImg;

	void buildLaplacianPyramid(const cuda::GpuMat& img, vector<cuda::GpuMat>& lapPyr);
	void buildGaussianPyramid(const cuda::GpuMat& img, vector <cuda::GpuMat>& gauPyr);
	//void buildGaussianPyramid(const cuda::GpuMat& img, vector <cuda::GpuMat>& gauPyr);
	void blendInEachLevel();
	Mat reconstructImg();

	int cstartLap, cendLap;
	

public:
	void setBGImage(Mat img);
	void setFGImage(Mat img);
	void setMaskImage(Mat img);
	Mat Run();
};

