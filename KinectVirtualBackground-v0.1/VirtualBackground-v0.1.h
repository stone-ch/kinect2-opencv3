#pragma once

class changeBG
{
public:
	changeBG();
	~changeBG();
	void SetChartlet(string path);
	void SetChartletMask(string path);
	void Run();

private:
	static const int CHEIGHT = 1080;
	static const int CWIDTH = 1920;
	static const int DHEIGHT = 424;
	static const int DWIDTH = 512;
	const float TRACKEDPEOPLE_MAXZ = 2.0;
	IKinectSensor*				pMyKinect;
	IMultiSourceFrameReader*	pMultiSourceFrameReader;
	ICoordinateMapper*			pCoordinateMapper;
	HRESULT						hr;
	DepthSpacePoint*			pDepthCoordinates;
	Mat							bg;
	Mat							mask;
	Mat							backgroundImage;
	Mat							resultImage;
	Mat							colorImage;
	Mat							z;
	RGBQUAD*					pColorBuffer;
	clock_t						cstart, cend;
	//VideoWriter					frameWriter;
	Mat_<Vec3f>					chartlet;
	Mat							chartlet_mask;
	bool						showChartlet;
	

	void initKinect();
	void Update();
	void ProcessFrame
		(
		INT64 time,
		const UINT16* pDepthBuffer, int depthHeight, int depthwidth,
		const RGBQUAD* pColorBuffer, int colorHeight, int colorWidth,
		const BYTE* pBodyIndexBuffer, int bodyIndexHeight, int bodyIndexWidth
		);
	void changeBG::saveVideo(Mat* frame);
};

