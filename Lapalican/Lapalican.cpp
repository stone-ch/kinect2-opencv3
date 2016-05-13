#include "stdafx.h"

class LaplacianBlending {
private:
	Mat_<Vec3f> left;
	Mat_<Vec3f> right;

	vector<Mat_<Vec3f> > leftLapPyr, rightLapPyr, resultLapPyr;
	Mat leftHighestLevel, rightHighestLevel, resultHighestLevel;
	vector<Mat_<Vec3f> > gaussianPyramid; 

	Mat rightGray, rightMask, right8U;
	vector<Mat> rightMaskPyramid;

	int levels;

	void buildPyramids() {
		buildLaplacianPyramid(left, leftLapPyr, leftHighestLevel);
		buildLaplacianPyramid(right, rightLapPyr, rightHighestLevel);
		buildGaussianPyramid();
		buildRightMask();
	}

	void getRightMask()
	{
		right.convertTo(right8U, CV_8U, 255.0);
		cvtColor(right8U, rightGray, COLOR_BGR2GRAY);
		threshold(rightGray, rightMask, 230, 255, CV_THRESH_BINARY_INV);
	}

	void buildGaussianPyramid() {
		assert(leftLapPyr.size()>0);

		gaussianPyramid.clear();
		Mat currentImg;
		
		currentImg = left.clone();
		int range_x = currentImg.rows * 0.2;
		int range_y = currentImg.cols * 0.2;
		Mat currentImgROI = currentImg(Range(range_x, range_x + right.rows), Range(range_y, range_y + right.cols));
		right.copyTo(currentImgROI, rightMask);
		//right.copyTo(currentImgROI);
		imshow("currentImg", currentImg);
		gaussianPyramid.push_back(currentImg);

		for (int l = 1; l<levels + 1; l++) {
			Mat _down;
			if (leftLapPyr.size() > l)
				pyrDown(currentImg, _down, leftLapPyr[l].size());
			else
				pyrDown(currentImg, _down, leftHighestLevel.size()); 
			
			gaussianPyramid.push_back(_down);
			currentImg = _down;
		}
	}

	void buildRightMask()
	{
		rightMaskPyramid.clear();
		rightMaskPyramid.push_back(rightMask);
		Mat currentImg = rightMask.clone();
		for (size_t l = 1; l < levels + 1; l++)
		{
			Mat _down;
			if (leftLapPyr.size() > l)
				pyrDown(currentImg, _down, rightLapPyr[l].size());
			else
				pyrDown(currentImg, _down, rightHighestLevel.size());

			rightMaskPyramid.push_back(_down);
			currentImg = _down;
		}
	}

	void buildLaplacianPyramid(const Mat& img, vector<Mat_<Vec3f> >& lapPyr, Mat& HighestLevel) {
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
		currentImg.copyTo(HighestLevel);
	}

	Mat_<Vec3f> reconstructImgFromLapPyramid() {
		Mat currentImg = leftHighestLevel;
		
		for (int l = levels - 1; l >= 0; l--) {
			Mat up;

			pyrUp(currentImg, up, resultLapPyr[l].size());
			currentImg = up + resultLapPyr[l];
			
		}
		return currentImg;
	}

	void blendLapPyrs() {
		int range_x = leftHighestLevel.rows * 0.2;
		int range_y = leftHighestLevel.cols * 0.2;
		Mat leftHighestLevelROI = leftHighestLevel(Range(range_x, range_x + rightHighestLevel.rows), Range(range_y, range_y + rightHighestLevel.cols));

		rightHighestLevel.copyTo(leftHighestLevelROI, rightMaskPyramid.back());
		for (int l = 0; l<levels; l++) {
			Mat A = rightLapPyr[l];
			int range_x = leftLapPyr[l].rows * 0.2;
			int range_y = leftLapPyr[l].cols * 0.2;
			Mat ROI = leftLapPyr[l](Range(range_x, range_x + rightLapPyr[l].rows), Range(range_y, range_y + rightLapPyr[l].cols));
			A.copyTo(ROI, rightMaskPyramid[l]);

			resultLapPyr.push_back(leftLapPyr[l]);
		}
	}

public:
	LaplacianBlending(const Mat_<Vec3f>& _left, const Mat_<Vec3f>& _right, int _levels) :
		left(_left), right(_right), levels(_levels)
	{
		getRightMask();
		buildPyramids();  
		blendLapPyrs();   
	};

	

	Mat_<Vec3f> blend() {
		return reconstructImgFromLapPyramid();
	}
};

Mat_<Vec3f> LaplacianBlend(const Mat_<Vec3f>& l, const Mat_<Vec3f>& r) {
	LaplacianBlending lb(l, r, 4);
	return lb.blend();
}

int main() {
	Mat l8u = imread("wall.jpg");
	Mat r8u = imread("apple1s.jpg");

	Mat_<Vec3f> l; l8u.convertTo(l, CV_32F, 1.0 / 255.0);
	Mat_<Vec3f> r; r8u.convertTo(r, CV_32F, 1.0 / 255.0);

	Mat_<Vec3f> blend = LaplacianBlend(l, r);
	imshow("blended", blend);

	waitKey(0);
	return 0;
}