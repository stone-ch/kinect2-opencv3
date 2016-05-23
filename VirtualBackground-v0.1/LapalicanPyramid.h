#include "stdafx.h"

class LapalicanClass
{
public:
	LapalicanClass();
	~LapalicanClass();

private:
	Mat_<Vec3f> backgroundImg, foregroundImg;
	Mat fgMask;

	vector< Mat_<Vec3f>> bgLapPyr, fgLapPyr, resultLapPyr;
	vector< Mat_<Vec3f>> fgGauPyramid, bgGauPyramid;
	vector< Mat_<Vec3b>> maskGauPyramid;

	int levels = 2;

	Mat targetImg;

	void buildLaplacianPyramid(const Mat_<Vec3f>& img, vector < Mat_<Vec3f> >& lapPyr);
	void buildGaussianPyramid(const Mat_<Vec3f>& img, vector < Mat_<Vec3f> >& gauPyr);
	void buildGaussianPyramid(const Mat& img, vector < Mat_<Vec3b> >& gauPyr);
	void blendInEachLevel();
	Mat reconstructImg();

	int cstartLap, cendLap;


public:
	void setBGImage(Mat img);
	void setFGImage(Mat img);
	void setMaskImage(Mat img);
	Mat Run();
};

