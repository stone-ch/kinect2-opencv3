#include "stdafx.h"

int main()
{
	HRESULT hr = S_OK;
	IKinectSensor* pMyKinect;

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

	clock_t cstart = 0, cend = 0;
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

	IDepthFrameSource* pDepthSource = NULL;
	IDepthFrameReader* pDepthReader = NULL;
	IFrameDescription* pDepthdDescription = NULL;

	if (SUCCEEDED(hr))
	{
		hr = pMyKinect->get_DepthFrameSource(&pDepthSource);
	}

	if (SUCCEEDED(hr))
	{
		hr = pDepthSource->OpenReader(&pDepthReader);
	}

	if (SUCCEEDED(hr))
	{
		hr = pDepthSource->get_FrameDescription(&pDepthdDescription);
	}

	int depthWidth = 0;
	int depthHeight = 0;

	if (SUCCEEDED(hr))
	{
		hr = pDepthdDescription->get_Height(&depthHeight);
	}

	if (SUCCEEDED(hr))
	{
		hr = pDepthdDescription->get_Width(&depthWidth);
	}
	
	Mat depthImage(depthHeight, depthWidth, CV_8UC1);
	UINT16 *depthData = new UINT16[depthWidth * depthHeight];
	while (1)
	{
		IDepthFrame* depthFrame;
		hr = pDepthReader->AcquireLatestFrame(&depthFrame);
		if (SUCCEEDED(hr))
		{
			depthFrame->CopyFrameDataToArray(depthHeight * depthWidth, depthData);
			for (int i = 0; i < depthWidth * depthHeight; i++)
			{
				byte intensity = (byte)(depthData[i]);
				reinterpret_cast<BYTE*>(depthImage.data)[i] = intensity;
			}
		}
		else
		{
			cout << "no frames" << rand() % 20 << endl;
		}
		if (depthFrame != NULL)
		{
			depthFrame->Release();
			depthFrame = NULL;
		}
		if (waitKey(30) == VK_ESCAPE)
			break;
		imshow("depthImage", depthImage);
	}
	if (pDepthSource != NULL)
	{
		pDepthSource->Release();
		pDepthSource = NULL;
	}
	if (pDepthReader != NULL)
	{
		pDepthReader->Release();
		pDepthReader = NULL;
	}
	if (pDepthdDescription != NULL)
	{
		pDepthdDescription->Release();
		pDepthdDescription = NULL;
	}
	if (pMyKinect)
	{
		pMyKinect->Close();
	}
	if (pMyKinect != NULL)
	{
		pMyKinect->Release();
		pMyKinect = NULL;
	}
	destroyAllWindows();
}