class drawBI
{
public:
	drawBI();
	~drawBI();
	void run();

private:
	static const int cHeight = 1080;
	static const int cWidth = 1920;
	static const int dHeight = 424;
	static const int dWidth = 512;
	IKinectSensor*				pMyKinect;
	IMultiSourceFrameReader*	pMultiSourceFrameReader;
	HRESULT						hr;
	clock_t						cstart, cend;
	Mat							bodyIndexImage;

	void initKinect();
	void draw();

};

drawBI::drawBI() :
	pMyKinect(NULL),
	pMultiSourceFrameReader(NULL)
{
	bodyIndexImage.create(dHeight, dWidth, CV_8UC3);
	initKinect();
}

drawBI::~drawBI()
{
	SafeRelease(pMultiSourceFrameReader);
	if (pMyKinect)
		pMyKinect->Close();
	SafeRelease(pMyKinect);
}