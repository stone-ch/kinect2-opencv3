// drawBodyIndex.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "drawBodyIndex.h"

int _tmain(int argc, _TCHAR* argv[])
{
	drawBI drawBodyIndexRed;
	drawBodyIndexRed.run();
	return 0;
}


void drawBI::initKinect()
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
		hr = pMyKinect->OpenMultiSourceFrameReader(
			FrameSourceTypes::FrameSourceTypes_BodyIndex,
			&pMultiSourceFrameReader);
	}
}

void drawBI::run()
{
	while (true)
	{
		draw();
		if (waitKey(10) == 27)
		{
			break;
		}
	}
}

void drawBI::draw()
{
	if (!pMultiSourceFrameReader)
	{
		return;
	}

	IMultiSourceFrame* pMultiSourceFrame = NULL;
	IBodyIndexFrame* pBodyIndexFrame = NULL;

	hr = pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);
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
	else
	{
		cout << "no frames" << rand() % 20 << endl;
	}

	if (SUCCEEDED(hr))
	{
		IFrameDescription* pBodyIndexFrameDescription = NULL;
		int bodyIndexWidth = 0;
		int bodyIndexHeight = 0;
		UINT bodyIndexBufferSize = 0;
		BYTE* pBodyIndexBuffer = NULL;

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
		SafeRelease(pBodyIndexFrameDescription);
		if (SUCCEEDED(hr))
		{
			for (size_t i = 0; i < bodyIndexHeight; i++)
			{
				for (size_t j = 0; j < dWidth; j++)
				{
					BYTE player = pBodyIndexBuffer[j + (i * dWidth)];
					if (player != 0xff)
					{
						bodyIndexImage.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
					}
					else
					{
						bodyIndexImage.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
					}
				}
			}
		}
	}
	SafeRelease(pBodyIndexFrame);
	imshow("image", bodyIndexImage);
}
