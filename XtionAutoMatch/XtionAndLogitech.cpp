// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

/*
XN_SKEL_HEAD			= 1,    XN_SKEL_NECK				= 2,
XN_SKEL_TORSO			= 3,    XN_SKEL_WAIST				= 4,
XN_SKEL_LEFT_COLLAR		= 5,    XN_SKEL_LEFT_SHOULDER       = 6,
XN_SKEL_LEFT_ELBOW		= 7,	XN_SKEL_LEFT_WRIST			= 8,
XN_SKEL_LEFT_HAND		= 9,    XN_SKEL_LEFT_FINGERTIP		=10,
XN_SKEL_RIGHT_COLLAR	=11,    XN_SKEL_RIGHT_SHOULDER		=12,
XN_SKEL_RIGHT_ELBOW		=13,	XN_SKEL_RIGHT_WRIST			=14,
XN_SKEL_RIGHT_HAND      =15,    XN_SKEL_RIGHT_FINGERTIP		=16,
XN_SKEL_LEFT_HIP		=17,    XN_SKEL_LEFT_KNEE           =18,
XN_SKEL_LEFT_ANKLE		=19,	XN_SKEL_LEFT_FOOT			=20,
XN_SKEL_RIGHT_HIP		=21,    XN_SKEL_RIGHT_KNEE          =22,
XN_SKEL_RIGHT_ANKLE		=23,    XN_SKEL_RIGHT_FOOT          =24
*/
int startSkelPoints[14] = { 1, 2, 6, 6, 12, 17, 6, 7, 12, 13, 17, 18, 21, 22 };
int endSkelPoints[14] = { 2, 3, 12, 21, 17, 21, 7, 9, 13, 15, 18, 20, 22, 24 };

void CheckOpenNIError(XnStatus result, string status)
{
	if (result != XN_STATUS_OK)
		cerr << status << " Error: " << xnGetStatusString(result) << endl;
}

// callback function of user generator: new user
void XN_CALLBACK_TYPE NewUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
	cout << "New user identified: " << user << endl;
	//userGenerator.GetSkeletonCap().LoadCalibrationDataFromFile( user, "UserCalibration.txt" );
	generator.GetSkeletonCap().RequestCalibration(user, FALSE);
}

// callback function of user generator: lost user
void XN_CALLBACK_TYPE LostUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
	cout << "User " << user << " lost" << endl;
}

// callback function of skeleton: calibration start
void XN_CALLBACK_TYPE CalibrationStart(xn::SkeletonCapability& skeleton, XnUserID user, void* pCookie)
{
	cout << "Calibration start for user " << user << endl;
}

// callback function of skeleton: calibration end 
void XN_CALLBACK_TYPE CalibrationEnd(xn::SkeletonCapability& skeleton, XnUserID user, XnCalibrationStatus calibrationError, void* pCookie)
{
	cout << "Calibration complete for user " << user << ", ";
	if (calibrationError == XN_CALIBRATION_STATUS_OK)
	{
		cout << "Success" << endl;
		skeleton.StartTracking(user);
		//userGenerator.GetSkeletonCap().SaveCalibrationDataToFile(user, "UserCalibration.txt" );
	}
	else
	{
		cout << "Failure" << endl;
		//For the current version of OpenNI, only Psi pose is available
		((xn::UserGenerator*)pCookie)->GetPoseDetectionCap().StartPoseDetection("Psi", user);
	}
}

int main()
{
	int offset_x, offset_y;
	offset_x = -70;
	offset_y = -60;
	int times = 2;
	XnStatus result = XN_STATUS_OK;
	xn::DepthMetaData depthMD;
	xn::ImageMetaData imageMD;

	xn::Context context;
	result = context.Init();
	CheckOpenNIError(result, "initialize context");

	xn::DepthGenerator depthGenerator;
	result = depthGenerator.Create(context);
	CheckOpenNIError(result, "Create depth generator");

	xn::ImageGenerator imageGenerator;
	result = imageGenerator.Create(context);
	CheckOpenNIError(result, "Create image generator");

	XnMapOutputMode mapMode;
	mapMode.nXRes = 320;
	mapMode.nYRes = 240;
	mapMode.nFPS = 30;
	result = imageGenerator.SetMapOutputMode(mapMode);
	result = depthGenerator.SetMapOutputMode(mapMode);

	//XnMapOutputMode mapMode2;
	//mapMode.nXRes = 640;
	//mapMode.nYRes = 480;
	//mapMode.nFPS = 30;
	//result = depthGenerator.SetMapOutputMode(mapMode2); 
	//result = imageGenerator.SetMapOutputMode(mapMode2);

	depthGenerator.GetAlternativeViewPointCap().SetViewPoint(imageGenerator);

	xn::UserGenerator userGenerator;
	result = userGenerator.Create(context);
	CheckOpenNIError(result, "Create depth generator");

	// Register callback functions of user generator
	XnCallbackHandle userCBHandle;
	userGenerator.RegisterUserCallbacks(NewUser, LostUser, NULL, userCBHandle);

	// Register callback functions of skeleton capability
	xn::SkeletonCapability skeletonCap = userGenerator.GetSkeletonCap();
	skeletonCap.SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	XnCallbackHandle calibCBHandle;
	skeletonCap.RegisterToCalibrationStart(CalibrationStart, &userGenerator, calibCBHandle);
	skeletonCap.RegisterToCalibrationComplete(CalibrationEnd, &userGenerator, calibCBHandle);

	result = context.StartGeneratingAll();
	result = context.WaitNoneUpdateAll();

	Mat RGBframe;
	VideoCapture cap(0);
	cap.set(CAP_PROP_FRAME_WIDTH, 640 * times);
	cap.set(CAP_PROP_FRAME_HEIGHT, 480 * times);

	namedWindow("RGBframe", CV_WINDOW_AUTOSIZE);
	//namedWindow("RGBframe", CV_WINDOW_NORMAL);
	//setWindowProperty("RGBframe", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	
	namedWindow("skeleton", CV_WINDOW_AUTOSIZE);
	//namedWindow("skeleton", CV_WINDOW_NORMAL);
	//setWindowProperty("skeleton", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	while ((waitKey(10) != 27) && !(result = context.WaitNoneUpdateAll()))
	{
		cap >> RGBframe;

		Mat imgDepth16u(Size(320, 240), CV_16U);
		Mat imgRGB8u(Size(320, 240), CV_8UC3);
		Mat depthShow(Size(320, 240), CV_8UC3);
		Mat imageShow(Size(320, 240), CV_8UC3);

		//get meta data  
		depthGenerator.GetMetaData(depthMD);
		imageGenerator.GetMetaData(imageMD);

		//Show Xtion RGB image and depth image 
		memcpy(imgDepth16u.data, depthMD.Data(), 320 * 240 * 2);
		imgDepth16u.convertTo(depthShow, CV_8UC3, 255 / 4096.0, 0);
		memcpy(imgRGB8u.data, imageMD.Data(), 320 * 240 * 3);
		cvtColor(imgRGB8u, imageShow, CV_RGB2BGR);
		resize(imageShow, imageShow, Size(imageShow.cols * 2, imageShow.rows * 2));
		//cv::imshow("depth", depthShow);
		//cv::imshow("image", imageShow);

		// get users
		XnUInt16 userCounts = userGenerator.GetNumberOfUsers();
		if (userCounts > 0)
		{
			XnUserID* userID = new XnUserID[userCounts];
			userGenerator.GetUsers(userID, userCounts);
			for (int i = 0; i < userCounts; ++i)
			{
				// if is tracking skeleton
				if (skeletonCap.IsTracking(userID[i]))
				{
					XnPoint3D skelPointsIn[24], skelPointsOut[24];
					XnSkeletonJointTransformation mJointTran;
					for (int iter = 0; iter<24; iter++)
					{
						//XnSkeletonJoint from 1 to 24			
						skeletonCap.GetSkeletonJoint(userID[i], XnSkeletonJoint(iter + 1), mJointTran);
						skelPointsIn[iter] = mJointTran.position.position;
					}
					depthGenerator.ConvertRealWorldToProjective(24, skelPointsIn, skelPointsOut);

					/*Point startpoint = Point((skelPointsOut[XN_SKEL_RIGHT_HAND].X - 100) * 3, (skelPointsOut[XN_SKEL_RIGHT_HAND].Y + 70)* 2);
					circle(RGBframe, startpoint, 3, Scalar(0, 0, 255), 12);*/

					/*Point startpoint = Point(skelPointsOut[XN_SKEL_RIGHT_HAND].X - 40.0, skelPointsOut[XN_SKEL_RIGHT_HAND].Y + 20.0);
					circle(RGBframe, startpoint, 3, Scalar(0, 0, 255), 12);

					Point startpoint2 = Point(skelPointsOut[XN_SKEL_LEFT_HAND].X - 40.0, skelPointsOut[XN_SKEL_LEFT_HAND].Y + 20.0);
					circle(RGBframe, startpoint2, 3, Scalar(0, 0, 255), 12);

					Point startpoint3 = Point(skelPointsOut[XN_SKEL_RIGHT_FOOT].X , skelPointsOut[XN_SKEL_RIGHT_FOOT].Y );
					circle(RGBframe, startpoint3, 3, Scalar(0, 0, 255), 12);

					Point startpoint4 = Point(skelPointsOut[XN_SKEL_LEFT_FOOT].X - 40.0, skelPointsOut[XN_SKEL_LEFT_FOOT].Y + 20.0);
					circle(RGBframe, startpoint4, 3, Scalar(0, 0, 255), 12);*/

					for (int d = 0; d<14; d++)
					{
						Point startpoint = Point(skelPointsOut[startSkelPoints[d] - 1].X*2 + offset_x,skelPointsOut[startSkelPoints[d] - 1].Y*2 + offset_y);
						Point endpoint = Point(skelPointsOut[endSkelPoints[d] - 1].X*2 + offset_x, skelPointsOut[endSkelPoints[d] - 1].Y *2+ offset_y);
						circle(RGBframe, startpoint * times, 3, Scalar(0, 0, 255), 12);
						circle(RGBframe, endpoint * times, 3, Scalar(0, 0, 255), 12);
						line(RGBframe, startpoint * times, endpoint * times, Scalar(0, 0, 255), 4);
					}

					for (int d = 0; d<14; d++)
					{
						Point startpoint = Point(skelPointsOut[startSkelPoints[d] - 1].X*2, skelPointsOut[startSkelPoints[d] - 1].Y*2);
						Point endpoint = Point(skelPointsOut[endSkelPoints[d] - 1].X*2, skelPointsOut[endSkelPoints[d] - 1].Y*2);
						circle(imageShow, startpoint, 3, Scalar(0, 0, 255), 12);
						circle(imageShow, endpoint, 3, Scalar(0, 0, 255), 12);
						line(imageShow, startpoint, endpoint, Scalar(0, 0, 255), 4);
					}
				}
			}
			delete[] userID;
		}

		
		//cv::imshow("RGBframe", RGBframe);

		cv::imshow("skeleton", imageShow);
	}
	cap.release();
	context.StopGeneratingAll();
	context.Release();
	return 0;
}
