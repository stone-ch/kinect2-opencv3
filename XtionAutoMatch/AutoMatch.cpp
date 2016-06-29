// AutoMatch.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

Point templateMatch(Mat img, Mat template_img)
{
	Mat result_match;
	int result_cols = img.cols - template_img.cols + 1;
	int result_rows = img.rows - template_img.rows + 1;
	result_match.create(result_rows, result_cols, CV_32FC1);
	matchTemplate(img, template_img, result_match, TM_SQDIFF);
	normalize(result_match, result_match, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal;
	double maxVal;
	Point minLoc;
	Point maxLoc;
	Point matchLoc;
	minMaxLoc(result_match, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	matchLoc = minLoc;
	//if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	//{
	//	matchLoc = minLoc;
	//}
	//else
	//{
	//	matchLoc = maxLoc;
	//}
	cout << matchLoc.x << " " << matchLoc.y << endl;
	rectangle(img, matchLoc, Point(matchLoc.x + template_img.cols, matchLoc.y + template_img.rows), Scalar::all(0), 2, 8, 0);
	return matchLoc;
}

//void getTemplateImg(int n, Mat sourceImage)
//{
//	Mat templ = sourceImage(Rect(p1.x, p1.y, templ_width, templ_height));
//
//}

void CheckOpenNIError(XnStatus result, string status)
{
	if (result != XN_STATUS_OK)
		cerr << status << " Error: " << xnGetStatusString(result) << endl;
}

int main(int, char** argv)
{
	Mat frame;
	Mat templ;
	Mat imgRGB8u(Size(640, 480), CV_8UC3);
	Mat imageShow(Size(640, 480), CV_8UC3);
	int templ_height = 350;
	int templ_width = 350;
	vector<Point> templ_point;
	vector<float> templ_MatchPoint_x;
	vector<float> templ_MatchPoint_y;
	int frameCount = 0;
	int discardCount = 20;
	float offset_x = 0.0;
	float offset_y = 0.0;


	templ_point.push_back(Point(150.0, 100.0));
	//templ_point.push_back(Point(100.0, 50.0));
	//templ_point.push_back(Point(340.0, 100.0));
	//templ_point.push_back(Point(245.0, 165.0));
	//templ_point.push_back(Point(150.0, 280.0));
	//templ_point.push_back(Point(340.0, 280.0));

	//Get camera
	VideoCapture cap(0);

	//Get Xtion Camera
	XnStatus result = XN_STATUS_OK;

	xn::ImageMetaData imageMD;

	xn::Context context;
	result = context.Init();
	CheckOpenNIError(result, "initialize context");

	xn::ImageGenerator imageGenerator;
	result = imageGenerator.Create(context);
	CheckOpenNIError(result, "Create image generator");

	XnMapOutputMode mapMode;
	mapMode.nXRes = 640;
	mapMode.nYRes = 480;
	mapMode.nFPS = 30;
	result = imageGenerator.SetMapOutputMode(mapMode);

	result = context.StartGeneratingAll();
	result = context.WaitNoneUpdateAll();
	

	while (frameCount < 30)
	{
		cout << frameCount << endl;
		float offset_x_t = 0.0;
		float offset_y_t = 0.0;
		Point2f matchPoint;

		cap >> frame;

		//get meta data  
		imageGenerator.GetMetaData(imageMD);

		//Show Xtion RGB image and depth image 
		memcpy(imgRGB8u.data, imageMD.Data(), 640 * 480 * 3);
		cvtColor(imgRGB8u, imageShow, CV_RGB2BGR);

		for (int i = 0; i < templ_point.size(); i++)
		{
			templ = imageShow(Rect(templ_point[i].x, templ_point[i].y, templ_width, templ_height));
			matchPoint = templateMatch(frame, templ);
			if ((matchPoint.x - templ_point[i].x) < 100 && (matchPoint.y - templ_point[i].y) < 100)
			{
				offset_x = matchPoint.x - templ_point[i].x;
				offset_y = matchPoint.y - templ_point[i].y;
			}
			
			templateMatch(imageShow, templ);
		}

		//templ_MatchPoint_x.push_back(offset_x_t / templ_point.size());
		//templ_MatchPoint_y.push_back(offset_y_t / templ_point.size());

		imshow("Xtion frame", imageShow);
		imshow("frame", frame);
		waitKey(10);
		context.WaitNoneUpdateAll();
		frameCount++;
	}

	
	/*vector<float>::iterator v_it;
	for (v_it = templ_MatchPoint_x.begin() + discardCount; v_it != templ_MatchPoint_x.end(); v_it++)
	{
		offset_x += *v_it;
	}
	for (v_it = templ_MatchPoint_y.begin() + discardCount; v_it != templ_MatchPoint_y.end(); v_it++)
	{
		offset_y += *v_it;
	}*/

	//offset_x /= templ_MatchPoint_x.size() - discardCount;
	//offset_y /= templ_MatchPoint_y.size() - discardCount;
	cout << "templ_MatchPoint_y.size" << templ_MatchPoint_y.size() << endl;

	cout << "offset_x : " << offset_x << endl;
	cout << "offset_y : " << offset_y << endl;

	context.StopGeneratingAll();
	context.Release();
	cap.release();
	destroyAllWindows();
	return 0;
}
