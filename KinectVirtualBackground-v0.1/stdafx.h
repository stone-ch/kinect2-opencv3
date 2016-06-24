#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <time.h>
#include <Kinect.h>
#include <opencv.hpp>

using namespace std;
using namespace cv;


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
