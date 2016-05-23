#include "stdafx.h"
#include "VirtualBackground-v0.1.h"
#include "LapalicanPyramid.h"

int _tmain(int argc, _TCHAR* argv[])
{

	changeBG CBG;
	CBG.SetChartlet("E:/data/clothes/fangaos128x128.jpg");
	CBG.Run();

	return 0;
}

changeBG::changeBG() :
pMyKinect(NULL),
pCoordinateMapper(NULL),
pMultiSourceFrameReader(NULL),
pDepthCoordinates(NULL)
{
	bg = imread("E:/data/clothes/19201080.jpg");
	backgroundImage = Mat::ones(Size(1920, 1080), CV_8UC3);
	colorImage.create(cHeight, cWidth, CV_8UC4);
	resultImage.create(cHeight, cWidth, CV_8UC3);
	hr = S_FALSE;
	cstart = 0, cend = 0;

	// create heap storage for the coorinate mapping from color to depth
	pDepthCoordinates = new DepthSpacePoint[cWidth * cHeight];
	pColorBuffer = new RGBQUAD[cHeight * cWidth];
	initKinect();
}

changeBG::~changeBG()
{
	SafeRelease(pMultiSourceFrameReader);
	SafeRelease(pCoordinateMapper);
	if (pMyKinect)
		pMyKinect->Close();
	SafeRelease(pMyKinect);
}

void changeBG::Run()
{
	while (true)
	{
		Update();
		resize(resultImage, resultImage, Size(1920, 1080));
		imshow("resultImage", resultImage);

		if (waitKey(10) == 27)
		{
			break;
		}
	}
}

void changeBG::SetChartlet(string path)
{
	Mat charlet8u = imread(path);
	resize(charlet8u, charlet8u, Size(charlet8u.cols / 2, charlet8u.rows / 2));
	charlet8u.convertTo(chartlet, CV_32F, 1.0 / 255.0);
	assert(!chartlet.empty());
}

void changeBG::initKinect()
{
	//Get Kinect
	hr = GetDefaultKinectSensor(&pMyKinect);
	if (SUCCEEDED(hr))
	{
		hr = pMyKinect->Open();
	}

	//Open Kinect
	BOOLEAN bIsOpen = false;
	if (SUCCEEDED(hr))
	{
		hr = pMyKinect->get_IsOpen(&bIsOpen);
	}
	if (!bIsOpen)
	{
		cout << "ERROR Kinect open Failed !" << endl;
	}
	cout << "Kinect is opening..." << endl;

	//Check Kinect is available or not
	BOOLEAN bAvaliable = 0;
	if (SUCCEEDED(hr))
	{
		hr = pMyKinect->get_IsAvailable(&bAvaliable);
	}


	cstart = clock();

	//Loop until Kinect is avaliable
	while (!bAvaliable)
	{
		if (SUCCEEDED(hr))
		{
			hr = pMyKinect->get_IsAvailable(&bAvaliable);
		}

		Sleep(20);
		cend = clock();
		if ((cend - cstart) > 5000)
		{
			cout << "ERROR cannot open Kinect!" << endl;
		}
	}
	cout << "opening time:" << cend - cstart << endl;

	if (SUCCEEDED(hr))
	{
		hr = pMyKinect->OpenMultiSourceFrameReader(FrameSourceTypes::FrameSourceTypes_Depth |
			FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_BodyIndex,
			&pMultiSourceFrameReader);
	}
	if (SUCCEEDED(hr))
	{
		hr = pMyKinect->get_CoordinateMapper(&pCoordinateMapper);
	}
}

void changeBG::Update()
{
	if (!pMultiSourceFrameReader)
	{
		return;
	}

	IMultiSourceFrame* pMultiSourceFrame = NULL;
	IDepthFrame* pDepthFrame = NULL;
	IColorFrame* pColorFrame = NULL;
	IBodyIndexFrame* pBodyIndexFrame = NULL;
	IBodyFrame* pBodyFrame = NULL;

	hr = pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);

	while (FAILED(hr))
	{
		SafeRelease(pMultiSourceFrame);
		SafeRelease(pMultiSourceFrameReader);
		hr = pMyKinect->OpenMultiSourceFrameReader(FrameSourceTypes::FrameSourceTypes_Depth |
			FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_BodyIndex | FrameSourceTypes::FrameSourceTypes_Body,
			&pMultiSourceFrameReader);
		Sleep(40);
		if (SUCCEEDED(hr))
		{
			hr = pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);
		}
	}

	if (SUCCEEDED(hr))
	{
		IDepthFrameReference* pDepthFrameReference = NULL;

		hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);
		}
		SafeRelease(pDepthFrameReference);
	}

	if (SUCCEEDED(hr))
	{
		IColorFrameReference* pColorFrameReference = NULL;

		hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pColorFrameReference->AcquireFrame(&pColorFrame);
		}
		SafeRelease(pColorFrameReference);
	}

	if (SUCCEEDED(hr))
	{
		IBodyIndexFrameReference* pBodyIndexFrameReference = NULL;

		hr = pMultiSourceFrame->get_BodyIndexFrameReference(&pBodyIndexFrameReference);

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameReference->AcquireFrame(&pBodyIndexFrame);
		}
		SafeRelease(pBodyIndexFrameReference);
	}

	if (SUCCEEDED(hr))
	{
		IBodyFrameReference* pBodyFrameReference = NULL;

		hr = pMultiSourceFrame->get_BodyFrameReference(&pBodyFrameReference);

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameReference->AcquireFrame(&pBodyFrame);
		}
		SafeRelease(pBodyFrameReference);
	}

	if (SUCCEEDED(hr))
	{
		INT64 depthTime = 0;
		IFrameDescription* pDepthFrameDescription = NULL;
		int depthWidth = 0;
		int depthHeight = 0;
		UINT depthBufferSize = 0;
		UINT16* pDepthBuffer = NULL;

		IFrameDescription* pColorFrameDescription = NULL;
		int colorWidth = 0;
		int colorHeight = 0;
		ColorImageFormat colorFormate = ColorImageFormat_None;
		UINT colorBufferSize = 0;
		//RGBQUAD* pColorBuffer = NULL;

		IFrameDescription* pBodyIndexFrameDescription = NULL;
		int bodyIndexWidth = 0;
		int bodyIndexHeight = 0;
		UINT bodyIndexBufferSize = 0;
		BYTE* pBodyIndexBuffer = NULL;

		//get depth frame data
		hr = pDepthFrame->get_RelativeTime(&depthTime);
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_FrameDescription(&pDepthFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameDescription->get_Height(&depthHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameDescription->get_Width(&depthWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->AccessUnderlyingBuffer(&depthBufferSize, &pDepthBuffer);
		}
		SafeRelease(pDepthFrameDescription);

		//get color frame data
		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_FrameDescription(&pColorFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameDescription->get_Height(&colorHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameDescription->get_Width(&colorWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_RawColorImageFormat(&colorFormate);
		}

		if (SUCCEEDED(hr))
		{
			if (colorFormate == ColorImageFormat_Rgba)
			{
				hr = pColorFrame->CopyRawFrameDataToArray(colorHeight * colorWidth * sizeof(RGBQUAD), reinterpret_cast<BYTE*>(colorImage.data));
			}
			else
			{
				hr = pColorFrame->CopyConvertedFrameDataToArray(colorHeight * colorWidth * sizeof(RGBQUAD), reinterpret_cast<BYTE*>(colorImage.data), ColorImageFormat_Bgra);
			}
		}
		SafeRelease(pColorFrameDescription);

		//ge body index frame data
		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrame->get_FrameDescription(&pBodyIndexFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameDescription->get_Height(&bodyIndexHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameDescription->get_Width(&bodyIndexWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrame->AccessUnderlyingBuffer(&bodyIndexBufferSize, &pBodyIndexBuffer);
		}

		if (SUCCEEDED(hr))
		{
			z = Mat::zeros(1080, 1920, CV_8UC3);
			ProcessFrame(depthTime, pDepthBuffer, depthHeight, depthWidth,
				pColorBuffer, colorHeight, colorWidth,
				pBodyIndexBuffer, bodyIndexHeight, bodyIndexWidth);
		}
		SafeRelease(pBodyIndexFrameDescription);

		//get body frame data and show the chartlet
		if (SUCCEEDED(hr))
		{
			IBody *body[BODY_COUNT] = { 0 };
			hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, body);
			for (int i = 0; i < BODY_COUNT; i++)
			{
				BOOLEAN tracked = false;
				hr = body[i]->get_IsTracked(&tracked);
				if (tracked)
				{
					Joint joint[JointType_Count];
					hr = body[i]->GetJoints(JointType_Count, joint);
					ColorSpacePoint csp1;
					pCoordinateMapper->MapCameraPointToColorSpace(joint[JointType_Head].Position, &csp1);

					int chartlet_x = ((int)csp1.X - 200 - (chartlet.cols / 2)) / 2;
					int chartlet_y = ((int)csp1.Y - (chartlet.rows / 2)) / 2;
					if (chartlet_x > 150 && chartlet_x < resultImage.cols - 100 && chartlet_y > 100 && chartlet_y < resultImage.rows - 100)
					{
						Mat_<Vec3f> chartletROI = resultImage(Range(chartlet_y, chartlet_y + chartlet.cols), Range(chartlet_x, chartlet_x + chartlet.rows));
						chartlet.copyTo(chartletROI);
					}
				}
			}
			for (int i = 0; i < BODY_COUNT; i++)
			{
				SafeRelease(body[i]);
			}
			SafeRelease(*body);
		}
	}

	SafeRelease(pBodyFrame);
	SafeRelease(pDepthFrame);
	SafeRelease(pColorFrame);
	SafeRelease(pBodyIndexFrame);
	SafeRelease(pMultiSourceFrame);
}


void changeBG::ProcessFrame
(
INT64 time,
const UINT16* pDepthBuffer, int depthHeight, int depthWidth,
const RGBQUAD* pColorBuffer, int colorHeight, int colorWidth,
const BYTE* pBodyIndexBuffer, int bodyIndexHeight, int bodyIndexWidth
)
{
	if (pCoordinateMapper && pDepthCoordinates &&
		pDepthBuffer && (depthWidth == dWidth) && (depthHeight == dHeight) &&
		pColorBuffer && (colorWidth == cWidth) && (colorHeight == cHeight) &&
		pBodyIndexBuffer && (bodyIndexWidth == dWidth) && (bodyIndexHeight == dHeight))
	{
		hr = pCoordinateMapper->MapColorFrameToDepthSpace(depthHeight * depthWidth, (UINT16*)pDepthBuffer, colorHeight * colorWidth, pDepthCoordinates);
		cend = clock();
		Mat colorImage3, bgc;
		bgc = bg.clone();

		cvtColor(colorImage, colorImage3, COLOR_BGRA2BGR);
		cend = clock();

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < colorHeight; i++)
			{
				for (int j = 0; j < colorWidth; j++)
				{
					int colorIndex = i * colorWidth + j;
					DepthSpacePoint p = pDepthCoordinates[colorIndex];

					int depthX = static_cast<int>(p.X + 0.5f);
					int depthY = static_cast<int>(p.Y + 0.5f);

					if ((depthX >= 0 && depthX < depthWidth) && (depthY >= 0 && depthY < depthHeight))
					{
						BYTE player = pBodyIndexBuffer[depthX + (depthY * dWidth)];


						if (player != 0xff)
						{
							z.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
						}
					}
				}
			}

			resize(bgc, bgc, Size(bgc.cols / 2, bgc.rows / 2));
			resize(colorImage3, colorImage3, Size(colorImage3.cols / 2, colorImage3.rows / 2));
			resize(z, z, Size(z.cols / 2, z.rows / 2));

			LapalicanClass lap;
			lap.setBGImage(bgc);
			lap.setFGImage(colorImage3);
			lap.setMaskImage(z);
			resultImage = lap.Run();
		}
	}
}
