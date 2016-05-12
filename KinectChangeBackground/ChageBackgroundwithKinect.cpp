#include "stdafx.h"
#include "ChangeBackgroundwithKinect.h"

int _tmain(int argc, _TCHAR* argv[])
{
	
	changeBG CBG;
	
	CBG.Run();
	
	return 0;
}

changeBG::changeBG() : 
	pMyKinect(NULL),
	pCoordinateMapper(NULL),
	pMultiSourceFrameReader(NULL),
	pDepthCoordinates(NULL)
{
	backgroundImage = imread("E:/data/clothes/19201080.jpg");
	colorImage.create(cHeight, cWidth, CV_8UC4);
	resultImage.create(cHeight, cWidth, CV_8UC3);
	hr = S_FALSE;
	cstart = 0, cend = 0;

	// create heap storage for the coorinate mapping from color to depth
	pDepthCoordinates = new DepthSpacePoint[cWidth * cHeight];
	// pDepthCoordinates = NULL;
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
		if (waitKey(10) == 27)
		{
			break;
		}
	}
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

	hr = pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);

	if (SUCCEEDED(hr))
	{
		IDepthFrameReference* pDepthFrameReference = NULL;

		hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);
		}
		SafeRelease(pDepthFrameReference);
		cstart = clock();
	}
	else
	{
		cout << "no frames" << rand() % 20 << endl;
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
				//hr = pColorFrame->AccessRawUnderlyingBuffer(&colorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));
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
			
			ProcessFrame(depthTime, pDepthBuffer, depthHeight, depthWidth,
				pColorBuffer, colorHeight, colorWidth,
				pBodyIndexBuffer, bodyIndexHeight, bodyIndexWidth);
			//cend = clock();
			cout << "process time: " << cend - cstart << endl;
		}
		SafeRelease(pBodyIndexFrameDescription);
	}

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
		resultImage = backgroundImage.clone();
		Mat colorImage3;
		cvtColor(colorImage, colorImage3, COLOR_BGRA2BGR);
		if (SUCCEEDED(hr))
		{
			for (size_t i = 0; i < colorHeight; i++)
			{
				for (size_t j = 0; j < colorWidth; j++)
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
							resultImage.at<Vec3b>(i, j) = colorImage3.at<Vec3b>(i, j);
							//resultImage.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
						}
						
					}
				}
				cend = clock();
				if ((cend - cstart) > 50)
				{
					break;
				}
			}
		}
		
		imshow("color", resultImage);
	}
}
