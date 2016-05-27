#include "stdafx.h"

template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

int hrCheck(HRESULT hr, string output)
{
	if (FAILED(hr))
	{
		cout << "ERROR " << output << " !" << endl;
		return 0;
	}
}

void DrawBone(JointType start, JointType end, ICoordinateMapper *coordinatemapper, Joint joint[], cv::Mat &canvas)
{
	ColorSpacePoint csp1, csp2;
	coordinatemapper->MapCameraPointToColorSpace(joint[start].Position, &csp1);
	coordinatemapper->MapCameraPointToColorSpace(joint[end].Position, &csp2);
	if (csp1.X > 0 && csp1.X < 1919 && csp1.Y > 0 && csp1.Y < 1079 && csp2.X > 0 && csp2.X < 1919 && csp2.Y > 0 && csp2.Y < 1079)
	{
		cv::line(canvas, cv::Point(csp1.X, csp1.Y), cv::Point(csp2.X, csp2.Y), cv::Scalar(0, 255, 0, 255), 3);
	}
	else
	{
		cv::line(canvas, cv::Point(csp1.X, csp1.Y), cv::Point(csp2.X, csp2.Y), cv::Scalar(255, 255, 255, 255), 1);
	}
	cv::circle(canvas, cv::Point(csp1.X, csp1.Y), 2, cv::Scalar(255, 255, 255, 255), 4);
	cv::circle(canvas, cv::Point(csp2.X, csp2.Y), 2, cv::Scalar(255, 255, 255, 255), 4);
}

void ShowSkeleton(ICoordinateMapper *pCoorinateMapper, Joint joint[], cv::Mat &bodyFrame)
{
	//Torso 8
	DrawBone(JointType_Head, JointType_Neck, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_Neck, JointType_SpineShoulder, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_SpineShoulder, JointType_SpineMid, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_SpineMid, JointType_SpineBase, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_SpineBase, JointType_HipLeft, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_SpineBase, JointType_HipRight, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_SpineShoulder, JointType_ShoulderLeft, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_SpineShoulder, JointType_ShoulderRight, pCoorinateMapper, joint, bodyFrame);

	//Right Arm 5
	DrawBone(JointType_ShoulderRight, JointType_ElbowRight, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_ElbowRight, JointType_WristRight, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_WristRight, JointType_HandRight, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_HandRight, JointType_HandTipRight, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_HandRight, JointType_ThumbRight, pCoorinateMapper, joint, bodyFrame);

	//Left Arm 5
	DrawBone(JointType_ShoulderLeft, JointType_ElbowLeft, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_ElbowLeft, JointType_WristLeft, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_WristLeft, JointType_HandLeft, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_HandLeft, JointType_HandTipLeft, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_HandLeft, JointType_ThumbLeft, pCoorinateMapper, joint, bodyFrame);

	//Right Leg 3
	DrawBone(JointType_HipRight, JointType_KneeRight, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_KneeRight, JointType_AnkleRight, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_AnkleRight, JointType_FootRight, pCoorinateMapper, joint, bodyFrame);

	//Left Leg 3
	DrawBone(JointType_HipLeft, JointType_KneeLeft, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_KneeLeft, JointType_AnkleLeft, pCoorinateMapper, joint, bodyFrame);
	DrawBone(JointType_AnkleLeft, JointType_FootLeft, pCoorinateMapper, joint, bodyFrame);
}

void DrawHandStatus(Joint joint, HandState sHand, ICoordinateMapper *coordinatemapper, cv::Mat &canvas)
{
	ColorSpacePoint cspHand;
	coordinatemapper->MapCameraPointToColorSpace(joint.Position, &cspHand);
	switch (sHand)
	{
	case HandState_Closed:
		cv::circle(canvas, cv::Point(cspHand.X, cspHand.Y), 30, cv::Scalar(0, 0, 255), -1);
		break;
	case HandState_Open:
		cv::circle(canvas, cv::Point(cspHand.X, cspHand.Y), 30, cv::Scalar(0, 255, 0), -1);
		break;
	case HandState_Lasso:
		cv::circle(canvas, cv::Point(cspHand.X, cspHand.Y), 30, cv::Scalar(255, 0, 0), -1);
		break;
	default:
		break;
	}
}

int LoadClothes(cv::Mat *image, cv::Mat *imageMask, ClothesType clothesType)
{
	string path;
	switch (clothesType)
	{
	case ClothesType_Hat:
		//path = "hat1s.jpg";
		break;
	case ClothesType_Coat:
		path = "coat1.jpg";
		break;
	default:
		break;
	}
	*image = cv::imread(path);
	if (image->empty())
	{
		//cout << "ERROR load cloathes image Failed !";
		return EXIT_FAILURE;
	}
	cv::cvtColor(*image, *image, cv::COLOR_BGR2BGRA);
	cv::Mat img_gray;
	cv::cvtColor(*image, img_gray, cv::COLOR_BGR2GRAY);
	cv::threshold(img_gray, *imageMask, 230, 255, cv::THRESH_BINARY_INV);
}

void WareClothes(cv::Mat clothesImage, cv::Mat clothes_mask, ICoordinateMapper* coordinatemapper, Joint joint[], cv::Mat *canvas, ClothesType clothesType)
{
	ColorSpacePoint cspHead, cspNeck, cspSpineShoulder, cspShoulderLeft, cspShoulderRight;
	coordinatemapper->MapCameraPointToColorSpace(joint[JointType_Head].Position, &cspHead);
	coordinatemapper->MapCameraPointToColorSpace(joint[JointType_Neck].Position, &cspNeck);
	coordinatemapper->MapCameraPointToColorSpace(joint[JointType_ShoulderLeft].Position, &cspShoulderLeft);
	coordinatemapper->MapCameraPointToColorSpace(joint[JointType_ShoulderRight].Position, &cspShoulderRight);
	int head_x = (int)cspHead.X;
	int head_y = (int)cspHead.Y;
	int neck_x = (int)cspNeck.X;
	int neck_y = (int)cspNeck.Y;
	int ShoulderWidth = abs((int)cspShoulderLeft.X - (int)cspShoulderRight.X);
 	if (ShoulderWidth > 100)
	{
		float clothesWidth, clothes_n;
		clothesWidth = ShoulderWidth * 1.5;
		clothes_n = clothesImage.cols / clothesWidth;
		if (clothes_n < 1)
		{
			clothes_n = 1;
		}

		cv::Mat clothesROI, clothesImage_copy, clothes_mask_copy;
		float clothes_x, clothes_y;
		switch (clothesType)
		{
		case ClothesType_Hat:
			clothes_x = head_x - clothesImage.cols / 2;
			clothes_y = head_y - clothesImage.rows;
			break;
		case ClothesType_Coat:
			cv::resize(clothesImage, clothesImage_copy, cv::Size(clothesImage.cols / clothes_n, clothesImage.rows / clothes_n));
			cv::resize(clothes_mask, clothes_mask_copy, cv::Size(clothes_mask.cols / clothes_n, clothes_mask.rows / clothes_n));
			clothes_x = neck_x - clothesImage_copy.cols / 2;
			clothes_y = neck_y - 20;
			break;
		default:
			break;
		}

		if (clothes_x > 0 && (clothes_x + clothesImage_copy.cols) < canvas->cols && clothes_y > 0 && (clothes_y + clothesImage_copy.rows) < canvas->rows)
		{
			clothesROI = (*canvas)(cv::Rect(clothes_x, clothes_y, clothesImage_copy.cols, clothesImage_copy.rows));
			clothesImage_copy.copyTo(clothesROI, clothes_mask_copy);
		}
	}
	
}

int main()
{
	//start************************init Kinct*******************************start

	//Get Kinect
	IKinectSensor *pMyKinect;
	HRESULT hr;
	hr = GetDefaultKinectSensor(&pMyKinect);
	hrCheck(hr, "Could not get Kinect");
	hr = pMyKinect->Open();
	hrCheck(hr, "Kinect Open Failed");

	//Open Kinect
	BOOLEAN bIsOpen = false;
	pMyKinect->get_IsOpen(&bIsOpen);
	if (!bIsOpen)
	{
		cout << "ERROR Kinect open Failed !" << endl;
		return 0;
	}
	cout << "Kinect is opening..." << endl;

	//Check Kinect is available or not
	BOOLEAN bAvaliable = 0;
	hr = pMyKinect->get_IsAvailable(&bAvaliable);
	hrCheck(hr, "Kinect is not available");

	clock_t cstart, cend;
	cstart = clock();

	//Loop until Kinect is avaliable
	while (!bAvaliable)
	{
		hr = pMyKinect->get_IsAvailable(&bAvaliable);
		hrCheck(hr, "Kinect is not available");

		Sleep(20);
		cend = clock();
		if ((cend - cstart) > 5000)
		{
			cout << "ERROR cannot open Kinect!" << endl;
			return 0;
		}
	}
	cout << "opening time:" << cend - cstart << endl;
	//end************************init Kinct*******************************end

	//Get color frame from kinect
	IColorFrameSource *pColorFrameSource;
	hr = pMyKinect->get_ColorFrameSource(&pColorFrameSource);
	hrCheck(hr, "Could not get color frame source");
	IColorFrameReader *pColorFrameReader;
	hr = pColorFrameSource->OpenReader(&pColorFrameReader);
	hrCheck(hr, "Could not get color frame reader");

	//Get body
	IBodyFrameSource *pBodyFrameSource;
	hr = pMyKinect->get_BodyFrameSource(&pBodyFrameSource);
	hrCheck(hr, "Could not get body source");
	IBodyFrameReader *pBodyFrameReader;
	hr = pBodyFrameSource->OpenReader(&pBodyFrameReader);
	hrCheck(hr, "Could not get body reader");

	ICoordinateMapper *pCoorinateMapper;
	pMyKinect->get_CoordinateMapper(&pCoorinateMapper);

	cv::Mat srcColorFrame(1080, 1920, CV_8UC4);
	cv::Mat bodyFrame = srcColorFrame(cv::Rect(0, 0, 1919, 1079));

	//Load hat
	cv::Mat hat, hat_mask;
	LoadClothes(&hat, &hat_mask, ClothesType_Hat);

	//Load coat
	cv::Mat coat, coat_mask;
	LoadClothes(&coat, &coat_mask, ClothesType_Coat);

	while (true)
	{
		//color frame
		IColorFrame *pColorFrame;
		hr = pColorFrameReader->AcquireLatestFrame(&pColorFrame);

		if (SUCCEEDED(hr))
		{
			UINT bufferSize = 1080 * 1920 * 4;

			hr = pColorFrame->CopyConvertedFrameDataToArray(bufferSize, reinterpret_cast<BYTE*>(srcColorFrame.data), ColorImageFormat_Bgra);
			hrCheck(hr, "Could not copy color frame to mat");
		}

		//body
		IBodyFrame *pBodyFrame;
		hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
		if (SUCCEEDED(hr))
		{
			IBody *body[BODY_COUNT] = { 0 };
			hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, body);
			hrCheck(hr, "Get and refresh body data Failed");

			for (size_t i = 0; i < BODY_COUNT; i++)
			{
				BOOLEAN tracked = false;
				hr = body[i]->get_IsTracked(&tracked);
				hrCheck(hr, "get body tracked Failed");
				if (tracked)
				{	
					Joint joint[JointType_Count];
					hr = body[i]->GetJoints(JointType_Count, joint);
					hrCheck(hr, "get body joints Failed");

					// show skeleton
					ShowSkeleton(pCoorinateMapper, joint, bodyFrame);

					// show the status of hands
					HandState sLeftHand;
					body[i]->get_HandLeftState(&sLeftHand);
					HandState sRightHand;
					body[i]->get_HandRightState(&sRightHand);
					DrawHandStatus(joint[JointType_HandLeft], sLeftHand, pCoorinateMapper, bodyFrame);
					DrawHandStatus(joint[JointType_HandRight], sRightHand, pCoorinateMapper, bodyFrame);

					//Ware clothes
					//WareClothes(hat, hat_mask, pCoorinateMapper, joint, &bodyFrame, ClothesType_Hat);
					WareClothes(coat, coat_mask, pCoorinateMapper, joint, &bodyFrame, ClothesType_Coat);
				}
			}
			for (size_t i = 0; i < BODY_COUNT; i++)
			{
				SafeRelease(body[i]);
			}
		}
		SafeRelease(pBodyFrame);
		SafeRelease(pColorFrame);

		cv::Mat smallFrame;
		cv::resize(srcColorFrame, smallFrame, cv::Size(srcColorFrame.cols / 2, srcColorFrame.rows / 2));

		//Show image fullsreen
		//cv::namedWindow("changeClothes", CV_WINDOW_NORMAL);
		//cv::setWindowProperty("changeClothes", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		cv::imshow("changeClothes", smallFrame);


		if (cv::waitKey(5) == 27)
		{
			break;
		}
	}

	SafeRelease(pColorFrameReader);
	SafeRelease(pColorFrameSource);
	SafeRelease(pCoorinateMapper);

	if (pMyKinect)
		pMyKinect->Close();
	SafeRelease(pMyKinect);
	cv::destroyAllWindows();
	return 0;
}
