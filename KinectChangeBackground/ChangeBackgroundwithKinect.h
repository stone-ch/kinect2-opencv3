#pragma once

class changeBG
{
public:
	changeBG();
	~changeBG();
	void Run();

private:
	static const int cHeight = 1080;
	static const int cWidth = 1920;
	static const int dHeight = 424;
	static const int dWidth = 512;
	IKinectSensor*				pMyKinect;
	IMultiSourceFrameReader*	pMultiSourceFrameReader;
	ICoordinateMapper*			pCoordinateMapper;
	HRESULT						hr;
	DepthSpacePoint*			pDepthCoordinates;
	Mat							backgroundImage;
	Mat							resultImage;
	Mat							colorImage;
	RGBQUAD*					pColorBuffer;
	clock_t						cstart, cend;

	void initKinect();	
	void Update();
	void ProcessFrame
	(
		INT64 time,
		const UINT16* pDepthBuffer, int depthHeight, int depthwidth,
		const RGBQUAD* pColorBuffer, int colorHeight, int colorWidth,
		const BYTE* pBodyIndexBuffer, int bodyIndexHeight, int bodyIndexWidth
		);
};

