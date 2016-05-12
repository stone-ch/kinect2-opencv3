#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <time.h>
#include <Kinect.h>
#include <opencv.hpp>

using namespace std;
using namespace cv;


// TODO:  在此处引用程序需要的其他头文件


// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}
