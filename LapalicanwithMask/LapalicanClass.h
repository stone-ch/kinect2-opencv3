#include "stdafx.h"

class LapalicanClass
{
public:
	LapalicanClass();
	~LapalicanClass();

private:
	Mat backgroundImg, foregroundImg, fgMask;

	vector< Mat_<Vec3b>> bgLapPyr, fgLapPyr, resultLapPyr;
	vector< Mat_<Vec3b>> fgGauPyramid, bgGauPyramid;
	vector< Mat_<Vec3b>> maskGauPyramid;

	int levels = 5;

	Mat targetImg;

	void buildLaplacianPyramid(const Mat& img, vector < Mat_<Vec3b> >& lapPyr);
	void buildGaussianPyramid(const Mat& img, vector < Mat_<Vec3b> >& gauPyr);
	void blendInEachLevel();
	void reconstructImg();

public:
	int LoadImg();
	void Run();
};

LapalicanClass::LapalicanClass()
{
	
 	Run();
}

LapalicanClass::~LapalicanClass()
{
	destroyAllWindows();
}