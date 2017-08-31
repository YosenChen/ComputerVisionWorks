
#include "Client.hpp"
#include <iostream>
#include "process.h"

using namespace std;

unsigned WINAPI sendThread(LPVOID);
unsigned WINAPI getThread(LPVOID);

unsigned WINAPI sendThread2(LPVOID);
unsigned WINAPI getThread2(LPVOID);
unsigned WINAPI getThread_CAM1(LPVOID);
unsigned WINAPI getThread_CAM2(LPVOID);
unsigned WINAPI sendThread_CAM1(LPVOID);
unsigned WINAPI sendThread_CAM2(LPVOID);

#define MAINTAINED_NUM 3
#define PREV 2
#define LAST 1
#define CURR 0

IplImage *sample_img, *sample_YCrCb, *sample_Y, *sample_Cr, *sample_Cb, *histimg_Cr, *histimg_Cb, *hist_mask;
CvHistogram *refHist, *getHist;
IplImage *img, *imgCr, *imgCb, *imgY, *imgYCrCb;
IplImage* imgResCr_CAM1[MAINTAINED_NUM];
IplImage* imgResCb_CAM1[MAINTAINED_NUM];
IplImage* imgResCr_CAM2[MAINTAINED_NUM];
IplImage* imgResCb_CAM2[MAINTAINED_NUM];
IplImage* imgResCr_CAM3[MAINTAINED_NUM];
IplImage* imgResCb_CAM3[MAINTAINED_NUM];
CameraLine3D CameraLine3D_CAM1[MAINTAINED_NUM*MAX_TARGET_NUM];
CameraLine3D CameraLine3D_CAM2[MAINTAINED_NUM*MAX_TARGET_NUM];
CameraLine3D CameraLine3D_CAM3[MAINTAINED_NUM*MAX_TARGET_NUM];
CvFont orderFont;
CameraLine3D targetGndLine_CAM1[MAX_TARGET_NUM];
CameraLine3D targetGndLine_CAM2[MAX_TARGET_NUM];
CameraLine3D targetGndLine_CAM3[MAX_TARGET_NUM];
BodyInfo body_CAM1[MAX_TARGET_NUM];
BodyInfo body_CAM2[MAX_TARGET_NUM];
BodyInfo body_CAM3[MAX_TARGET_NUM];
double targetHeight_CAM1[MAX_TARGET_NUM];
double targetHeight_CAM2[MAX_TARGET_NUM];
double targetHeight_CAM3[MAX_TARGET_NUM];
Position3D targetPos_CAM1[MAX_TARGET_NUM];
Position3D targetPos_CAM2[MAX_TARGET_NUM];
Position3D targetPos_CAM3[MAX_TARGET_NUM];
AliveStaMgr_CAM1 Alive_CAM1[MAINTAINED_NUM];
AliveStaMgr_CAM2 Alive_CAM2[MAINTAINED_NUM];
AliveStaMgr_CAM3 Alive_CAM3[MAINTAINED_NUM];

bool occupiedList_cam1[MAX_TARGET_NUM];
bool occupiedList_cam2[MAX_TARGET_NUM];
OBJ_CONFIDENCE objConfMap[MAX_TARGET_NUM][MAX_TARGET_NUM];
int matchCntMap[MAX_TARGET_NUM][MAX_TARGET_NUM];
double colorScoreMap[MAX_TARGET_NUM][MAX_TARGET_NUM];
bool Cam1DispReady = false;
bool Cam2DispReady = false;

	CamLineMgr_CAM1_OB1 camlinemgr11;
	CamLineMgr_CAM1_OB2 camlinemgr12;
	CamLineMgr_CAM1_OB3 camlinemgr13;
	CamLineMgr_CAM1_OB4 camlinemgr14;
	CamLineMgr_CAM1_OB5 camlinemgr15;
	CamLineMgr_CAM2_OB1 camlinemgr21;
	CamLineMgr_CAM2_OB2 camlinemgr22;
	CamLineMgr_CAM2_OB3 camlinemgr23;
	CamLineMgr_CAM2_OB4 camlinemgr24;
	CamLineMgr_CAM2_OB5 camlinemgr25;
	CamLineMgr_CAM3_OB1 camlinemgr31;
	CamLineMgr_CAM3_OB2 camlinemgr32;
	CamLineMgr_CAM3_OB3 camlinemgr33;
	CamLineMgr_CAM3_OB4 camlinemgr34;
	CamLineMgr_CAM3_OB5 camlinemgr35;


void prepare();
void showImage();
void showReceiveData();
void syncThreadFilter();
bool syncTimeWithinCAMs(int & cam1_time, int & cam2_time, int & cam3_time);


//class type1
//{
//public:
//	int x;
//};
//class type2
//{
//public:
//	int x;
//};
//void test(type1 & a){a.x++;}
//void test(type2 & b){b.x++;}

void main()
{
	//-------------------------------------------//
	// connect to central
	// you can also assign a specific IP
	// connectToServer("192.168.10.201");
	//-------------------------------------------//
	IPCClient.connectToServer(/*"192.168.1.87"*/);

	//-------------------------------------------//
	// Subscribe all the messages you need.
	// All the messages you'll use get() function
	// should be subscribed here, or you will get 
	// nothing.
	// The length of argument is not limited. So 
	// you can use this :
	// subscribe( A, B, C, D, ...., NULL );
	//-------------------------------------------//
	//IPCClient.subscribe( SUBGOAL_NAME, ODOMETRY_NAME, NULL );
	IPCClient.subscribe( 
		HISTCRCB_CAM1_OB1_NAME, 
		HISTCRCB_CAM1_OB2_NAME,
		HISTCRCB_CAM1_OB3_NAME,
		HISTCRCB_CAM1_OB4_NAME,
		HISTCRCB_CAM1_OB5_NAME,
		HISTCRCB_CAM2_OB1_NAME, 
		HISTCRCB_CAM2_OB2_NAME,
		HISTCRCB_CAM2_OB3_NAME,
		HISTCRCB_CAM2_OB4_NAME,
		HISTCRCB_CAM2_OB5_NAME,
		HISTCRCB_CAM3_OB1_NAME, 
		HISTCRCB_CAM3_OB2_NAME,
		HISTCRCB_CAM3_OB3_NAME,
		HISTCRCB_CAM3_OB4_NAME,
		HISTCRCB_CAM3_OB5_NAME,

		ODOMETRY_NAME, GOAL_NAME,

		TEST_NAME,
		BODYINFO_CAM1_OB1_NAME,
		BODYINFO_CAM1_OB2_NAME,
		BODYINFO_CAM1_OB3_NAME,
		BODYINFO_CAM1_OB4_NAME,
		BODYINFO_CAM1_OB5_NAME,
		BODYINFO_CAM2_OB1_NAME,
		BODYINFO_CAM2_OB2_NAME,
		BODYINFO_CAM2_OB3_NAME,
		BODYINFO_CAM2_OB4_NAME,
		BODYINFO_CAM2_OB5_NAME,
		BODYINFO_CAM3_OB1_NAME,
		BODYINFO_CAM3_OB2_NAME,
		BODYINFO_CAM3_OB3_NAME,
		BODYINFO_CAM3_OB4_NAME,
		BODYINFO_CAM3_OB5_NAME,



		POS3D_CAM1_OB1_NAME,
		POS3D_CAM1_OB2_NAME,
		POS3D_CAM1_OB3_NAME,
		POS3D_CAM1_OB4_NAME,
		POS3D_CAM1_OB5_NAME,
		POS3D_CAM2_OB1_NAME,
		POS3D_CAM2_OB2_NAME,
		POS3D_CAM2_OB3_NAME,
		POS3D_CAM2_OB4_NAME,
		POS3D_CAM2_OB5_NAME,
		POS3D_CAM3_OB1_NAME,
		POS3D_CAM3_OB2_NAME,
		POS3D_CAM3_OB3_NAME,
		POS3D_CAM3_OB4_NAME,
		POS3D_CAM3_OB5_NAME,



		CAMLINE_CAM1_OB1_NAME,
		CAMLINE_CAM1_OB2_NAME,
		CAMLINE_CAM1_OB3_NAME,
		CAMLINE_CAM1_OB4_NAME,
		CAMLINE_CAM1_OB5_NAME,
		CAMLINE_CAM2_OB1_NAME,
		CAMLINE_CAM2_OB2_NAME,
		CAMLINE_CAM2_OB3_NAME,
		CAMLINE_CAM2_OB4_NAME,
		CAMLINE_CAM2_OB5_NAME,
		CAMLINE_CAM3_OB1_NAME,
		CAMLINE_CAM3_OB2_NAME,
		CAMLINE_CAM3_OB3_NAME,
		CAMLINE_CAM3_OB4_NAME,
		CAMLINE_CAM3_OB5_NAME,


		ALIVESTA_CAM1_NAME,
		ALIVESTA_CAM2_NAME,
		ALIVESTA_CAM3_NAME,

		LABELINTEGRATE_CAM1_NAME,
		LABELINTEGRATE_CAM2_NAME,
		LABELINTEGRATE_CAM3_NAME,

		SUBGOAL_NAME, 
		NULL );

	//-------------------------------------------//
	// Publish all the messages you'll send.
	// All the messages you'll use set() function
	// should be published here, or you will see 
	// lots of errors.
	// The length of argument is not limited. So 
	// you can use this :
	// publish( A, B, C, D, ...., NULL );
	//-------------------------------------------//
	//IPCClient.publish( SUBGOAL_NAME, ODOMETRY_NAME, NULL );
	IPCClient.publish(
		HISTCRCB_CAM1_OB1_NAME, 
		HISTCRCB_CAM1_OB2_NAME,
		HISTCRCB_CAM1_OB3_NAME,
		HISTCRCB_CAM1_OB4_NAME,
		HISTCRCB_CAM1_OB5_NAME,
		HISTCRCB_CAM2_OB1_NAME, 
		HISTCRCB_CAM2_OB2_NAME,
		HISTCRCB_CAM2_OB3_NAME,
		HISTCRCB_CAM2_OB4_NAME,
		HISTCRCB_CAM2_OB5_NAME,
		HISTCRCB_CAM3_OB1_NAME, 
		HISTCRCB_CAM3_OB2_NAME,
		HISTCRCB_CAM3_OB3_NAME,
		HISTCRCB_CAM3_OB4_NAME,
		HISTCRCB_CAM3_OB5_NAME,

		ODOMETRY_NAME, GOAL_NAME,

		TEST_NAME,
		BODYINFO_CAM1_OB1_NAME,
		BODYINFO_CAM1_OB2_NAME,
		BODYINFO_CAM1_OB3_NAME,
		BODYINFO_CAM1_OB4_NAME,
		BODYINFO_CAM1_OB5_NAME,
		BODYINFO_CAM2_OB1_NAME,
		BODYINFO_CAM2_OB2_NAME,
		BODYINFO_CAM2_OB3_NAME,
		BODYINFO_CAM2_OB4_NAME,
		BODYINFO_CAM2_OB5_NAME,
		BODYINFO_CAM3_OB1_NAME,
		BODYINFO_CAM3_OB2_NAME,
		BODYINFO_CAM3_OB3_NAME,
		BODYINFO_CAM3_OB4_NAME,
		BODYINFO_CAM3_OB5_NAME,



		POS3D_CAM1_OB1_NAME,
		POS3D_CAM1_OB2_NAME,
		POS3D_CAM1_OB3_NAME,
		POS3D_CAM1_OB4_NAME,
		POS3D_CAM1_OB5_NAME,
		POS3D_CAM2_OB1_NAME,
		POS3D_CAM2_OB2_NAME,
		POS3D_CAM2_OB3_NAME,
		POS3D_CAM2_OB4_NAME,
		POS3D_CAM2_OB5_NAME,
		POS3D_CAM3_OB1_NAME,
		POS3D_CAM3_OB2_NAME,
		POS3D_CAM3_OB3_NAME,
		POS3D_CAM3_OB4_NAME,
		POS3D_CAM3_OB5_NAME,



		CAMLINE_CAM1_OB1_NAME,
		CAMLINE_CAM1_OB2_NAME,
		CAMLINE_CAM1_OB3_NAME,
		CAMLINE_CAM1_OB4_NAME,
		CAMLINE_CAM1_OB5_NAME,
		CAMLINE_CAM2_OB1_NAME,
		CAMLINE_CAM2_OB2_NAME,
		CAMLINE_CAM2_OB3_NAME,
		CAMLINE_CAM2_OB4_NAME,
		CAMLINE_CAM2_OB5_NAME,
		CAMLINE_CAM3_OB1_NAME,
		CAMLINE_CAM3_OB2_NAME,
		CAMLINE_CAM3_OB3_NAME,
		CAMLINE_CAM3_OB4_NAME,
		CAMLINE_CAM3_OB5_NAME,


		ALIVESTA_CAM1_NAME,
		ALIVESTA_CAM2_NAME,
		ALIVESTA_CAM3_NAME,

		LABELINTEGRATE_CAM1_NAME,
		LABELINTEGRATE_CAM2_NAME,
		LABELINTEGRATE_CAM3_NAME,

		SUBGOAL_NAME,  
		NULL );




	// you can also use listen( time );
	IPCClient.listen();

	prepare();

	unsigned threadID;
	HANDLE hthreads[3];
	int a=0;
	cin >> a;
	if (a == 0) //server
	{
		hthreads[0] = (HANDLE)_beginthreadex( NULL, 0, getThread_CAM1, NULL, 0, &threadID );
		hthreads[1] = (HANDLE)_beginthreadex( NULL, 0, getThread_CAM2, NULL, 0, &threadID );
	}
	else if (a == 1) //CAM1 tracker
		hthreads[0] = (HANDLE)_beginthreadex( NULL, 0, sendThread_CAM1, NULL, 0, &threadID );
	else if (a == 2) //CAM2 tracker
		hthreads[0] = (HANDLE)_beginthreadex( NULL, 0, sendThread_CAM2, NULL, 0, &threadID );
	
	while( cvWaitKey(30) != 27)
	{
		if ((a == 0) && Cam1DispReady && Cam2DispReady ) //server
		{
			showReceiveData();
		}
	}

	WaitForMultipleObjects( 2, hthreads, TRUE, INFINITE );
	CloseHandle( hthreads[0] );
	CloseHandle( hthreads[1] );
	CloseHandle( hthreads[2] );

}

unsigned WINAPI sendThread(LPVOID)
{
	SubgoalMgr send_data;
	AliveStaMgr_CAM1 as;
	OdometryMgr od;
	Pos3DMgr_CAM1_OB1 p;
	GoalMgr gl;
	test tt;
	BodyInfoMgr_CAM1_OB1 bi;
	HistCrCbMgr_CAM1_OB1 hcc;



	as.timeIndex = 0;
	send_data.x = 0;
	od.x = 0;
	p.x = 0;
	gl.x = 0;
	tt.a = 0;
	//tt.s = 0.1;
	hcc.timeIndex = 0;
	bi.timeIndex = 0;
	//while(1){
	for(int i = 0 ; i < 100 ; ++i){
		
		send_data.x ++ ;
		as.timeIndex ++;
		od.x ++;
		p.x ++;
		gl.x ++;
		tt.a ++;
		//tt.s ++;
		hcc.timeIndex ++;
		bi.timeIndex ++;

		setSubgoal( send_data );
		setAliveSta( as );
		setOdometry( od );
		setPos3D( p );
		setGoal( gl );
		settest( tt );
		setHistCrCb( hcc );
		setBodyInfo( bi);

		int a = true;
		int b = false;
		double c = true;
		double d = false;
		
		double W = 2.5;
		bool A = W;
		W = -1.2;
		bool B = W;
		W = 0;
		bool C = W;
		
		int Q = 2;
		bool D = Q;
		Q = -2;
		bool E = Q;
		Q = 0;
		bool F = Q;

		cout << "SET SUB : " << send_data.x << endl;
		cout << "SET timeIndex : " << as.timeIndex << endl;
		cout << "SET Odometry : " << od.x << endl;
		cout << "SET POS : " << p.x << endl;
		cout << "SET GOAL : " << gl.x << endl;
		cout << "SET TEST : " << tt.a << endl;
		//cout << "SET TEST : " << tt.s << endl;
		cout << "SET HISTCC : " << hcc.timeIndex << endl;
		cout << "SET BDIF  : " << bi.timeIndex << endl;
		cout << "int true : " << a << endl;
		cout << "int false : " << b << endl;
		cout << "double true : " << c << endl;
		cout << "double false : " << d << endl;
		cout << "bool 2.5 : " << A << endl;
		cout << "bool -1.2 : " << B << endl;
		cout << "bool (double)0 : " << C << endl;
		cout << "bool (int)2 : " << D << endl;
		cout << "bool (int)-2 : " << E << endl;
		cout << "bool (int)0 : " << F << endl;
		Sleep(100);
	}
	return 0;
}

unsigned WINAPI getThread(LPVOID)
{
	SubgoalMgr get_data;
	AliveStaMgr_CAM1 as;
	OdometryMgr od;
	Pos3DMgr_CAM1_OB1 p;
	GoalMgr gl;
	test tt;
	HistCrCbMgr_CAM1_OB1 hcc;
	BodyInfoMgr_CAM1_OB1 bi;

	//while(1){
	for(int i = 0 ; i < 100 ; ++i){
		
		getSubgoal( get_data );
		getAliveSta( as );
		getOdometry( od );
		getPos3D( p );
		getGoal( gl );
		gettest( tt );
		getHistCrCb( hcc );
		getBodyInfo( bi );

		cout << "				GET SUB : " << get_data.x << endl;
		cout << "				GET timeIndex : " << as.timeIndex << endl;
		cout << "				GET Odometry : " << od.x << endl;
		cout << "				GET POS : " << p.x << endl;
		cout << "				GET GOAL : " << gl.x << endl;
		cout << "				GET TEST : " << tt.a << endl;
		//cout << "				GET TEST : " << tt.s << endl;
		cout << "				GET HISTCC : " << hcc.timeIndex << endl;
		cout << "				GET BDIF : " << bi.timeIndex << endl;
		Sleep(100);
	}
	return 0;
}

void prepare()
{
	for (int i=0; i<MAINTAINED_NUM*MAX_TARGET_NUM; i++)
	{
		CameraLine3D_CAM1[i].camID = CAM1;
		CameraLine3D_CAM2[i].camID = CAM2;
		CameraLine3D_CAM3[i].camID = CAM3;
		CameraLine3D_CAM3[i].IsInit = CameraLine3D_CAM2[i].IsInit = CameraLine3D_CAM1[i].IsInit = true;
	}
	cvInitFont( &orderFont, CV_FONT_HERSHEY_PLAIN, 0.8, 0.8, 0, 1 );
	sample_img = cvLoadImage("green_CAM1.bmp", 1);
	sample_YCrCb = cvCreateImage(cvGetSize(sample_img), 8, 3);
	sample_Y = cvCreateImage(cvGetSize(sample_img), 8, 1);
	sample_Cr = cvCreateImage(cvGetSize(sample_img), 8, 1);
	sample_Cb = cvCreateImage(cvGetSize(sample_img), 8, 1);

	//
	float * Cr_ranges, * Cb_ranges;
	Cr_ranges = new float[2];
	Cb_ranges = new float[2];
	Cr_ranges[0] = Cb_ranges[0] = RANGE_MIN;
	Cr_ranges[1] = Cb_ranges[1] = RANGE_MAX;

	int hist_size[] = { Cr_BINS, Cb_BINS };
	float * ranges[] = { Cr_ranges, Cb_ranges };

	refHist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	getHist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	delete [] Cr_ranges;
	delete [] Cb_ranges;
	//

	histimg_Cr = cvCreateImage( cvSize(320,200), 8, 3 );
	histimg_Cb = cvCreateImage( cvSize(320,200), 8, 3 );
	cvZero(histimg_Cr);
	cvZero(histimg_Cb);

	cvCvtColor(sample_img, sample_YCrCb, CV_BGR2YCrCb);
	cvSplit(sample_YCrCb, sample_Y, sample_Cr, sample_Cb, NULL );
	IplImage * sample_planes[] = {sample_Cr, sample_Cb};
	cvCalcHist( sample_planes, refHist, 0, 0 );
	float max_val = 0.0f;
	cvGetMinMaxHistValue( refHist, 0, &max_val, 0, 0 );
	cvConvertScale( refHist->bins, refHist->bins, max_val ? 255. / max_val : 0., 0 );
		
//	cvNamedWindow("Transfered Cr Hist", 1);
//	cvNamedWindow("Transfered Cb Hist", 1);
	cvNamedWindow("imgResCr_CAM1[CURR]", 1);
	cvNamedWindow("imgResCb_CAM1[CURR]", 1);
	cvNamedWindow("imgResCr_CAM2[CURR]", 1);
	cvNamedWindow("imgResCb_CAM2[CURR]", 1);
	//cvNamedWindow("imgResCr_CAM1[LAST]", 1);
	//cvNamedWindow("imgResCb_CAM1[LAST]", 1);
	//cvNamedWindow("imgResCr_CAM2[LAST]", 1);
	//cvNamedWindow("imgResCb_CAM2[LAST]", 1);
	//cvNamedWindow("imgResCr_CAM1[PREV]", 1);
	//cvNamedWindow("imgResCb_CAM1[PREV]", 1);
	//cvNamedWindow("imgResCr_CAM2[PREV]", 1);
	//cvNamedWindow("imgResCb_CAM2[PREV]", 1);

	img = cvLoadImage("take_A_image.bmp", 1);
	
	imgCr = cvCreateImage(cvGetSize(img), 8, 1);
	imgCb = cvCreateImage(cvGetSize(img), 8, 1);
	imgY = cvCreateImage(cvGetSize(img), 8, 1);
	hist_mask = cvCreateImage(cvGetSize(img), 8, 1);
	imgYCrCb = cvCreateImage(cvGetSize(img), 8, 3);

	cvCvtColor(img, imgYCrCb, CV_BGR2YCrCb);
	cvSplit(imgYCrCb, imgY, imgCr, imgCb, NULL);

	for (int i=0; i<MAINTAINED_NUM; i++)
	{
		imgResCr_CAM1[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResCb_CAM1[i] = cvCreateImage(cvGetSize(img), 8, 1);
		cvZero(imgResCr_CAM1[i]);
		cvZero(imgResCb_CAM1[i]);
		imgResCr_CAM2[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResCb_CAM2[i] = cvCreateImage(cvGetSize(img), 8, 1);
		cvZero(imgResCr_CAM2[i]);
		cvZero(imgResCb_CAM2[i]);
		imgResCr_CAM3[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResCb_CAM3[i] = cvCreateImage(cvGetSize(img), 8, 1);
		cvZero(imgResCr_CAM3[i]);
		cvZero(imgResCb_CAM3[i]);
	}

	int defaultHeight = 155;
	for (int g=0; g<MAX_TARGET_NUM; g++)
	{
		targetHeight_CAM1[g] = targetHeight_CAM2[g] = targetHeight_CAM3[g] = defaultHeight;
		occupiedList_cam1[g] = occupiedList_cam2[g] = false;
		for (int f=0; f<MAX_TARGET_NUM; f++)
		{
			objConfMap[g][f] = OBJ_NON_DEFINED;
			matchCntMap[g][f] = 0;
			colorScoreMap[g][f] = NONDEFINED;
		}

	}
 

}

unsigned WINAPI sendThread2(LPVOID)
{
	//type1 A;
	//A.x = 1;
	//type2 B;
	//B.x = 1;
	//test(A);
	//test(B);



	BodyInfoMgr_CAM1_OB1 send_data;
	BodyInfo bi;
	HistCrCbMgr_CAM1_OB1 shist;
	trans2HistCrCbMgr(refHist, &shist, 200);
	CamLineMgr_CAM1_OB1 linemgr;
	CamLineMgr_CAM1_OB4 linemgrb;
	CameraLine3D line, lineb;
	line.camID = CAM1;


	bi.height = 10;
	line.cm2pixelRatio = 10.2;
	lineb = line;
	line.imageLoc = cvPoint(80, 40);
	lineb.imageLoc = cvPoint(220, 150);
	//while(1){
	for(int i = 0 ; i < 10000 ; ++i){
		
		bi.height ++ ;
		line.cm2pixelRatio ++;
		lineb.cm2pixelRatio ++;
		trans2BodyInfoMgr(&bi, &send_data, 100);
		trans2CamLineMgr(imgCr, imgCb, &line, &linemgr, 150);
		trans2CamLineMgr(imgCr, imgCb, &lineb, &linemgrb, 150);
		setBodyInfo( send_data );
		setCamLine( linemgr );
		setCamLine( linemgrb );
		setHistCrCb( shist );
		cout << "SET HEIGHT : " << send_data.height << endl;
		cout << "SET RATIO : " << linemgr.cm2pixelRatio << endl;
		cout << "SET B RATIO : " << linemgrb.cm2pixelRatio << endl;
		Sleep(50);
	}
	return 0;
}

unsigned WINAPI getThread2(LPVOID)
{
	//type1 A;
	//A.x = 1;
	//type2 B;
	//B.x = 1;
	//test(A);
	//test(B);

	HistCrCbMgr_CAM1_OB1 ghist;

	BodyInfoMgr_CAM1_OB1 get_data;
	BodyInfo bi;
	CamLineMgr_CAM1_OB1 linemgr;
	CamLineMgr_CAM1_OB4 linemgrb;
	CameraLine3D line, lineb;


	//while(1){
	for(int i = 0 ; i < 10000 ; ++i){
		
		getBodyInfo( get_data );
		getCamLine( linemgr);
		getCamLine( linemgrb);
		getHistCrCb( ghist );
		trans2BodyInfo(&get_data, &bi);
		trans2CamLine(&linemgr, &line, imgResCr_CAM1[CURR], imgResCb_CAM1[CURR]);
		trans2CamLine(&linemgrb, &lineb, imgResCr_CAM1[CURR], imgResCb_CAM1[CURR]);
		trans2HistCrCb(&ghist, getHist);
		cout << "							GET HEIGHT : " << bi.height << endl;
		cout << "							GET RATIO : " << line.cm2pixelRatio << endl;
		cout << "							GET RATIO : " << lineb.cm2pixelRatio << endl;

		//////////int bin_w = histimg_Cr->width / Cr_BINS;
		//////////cvZero(histimg_Cr);
		//////////cvZero(histimg_Cb);
		//////////for( int i = 0; i < Cr_BINS; i++ )
		//////////{
		//////////	long int val = 0;
		//////////	for (int j=0; j<Cb_BINS; j++)
		//////////		val += cvRound( cvGetReal2D(getHist->bins,i, j));

		//////////	val = val/Cr_BINS/128;
		//////////	
		//////////	cvRectangle( histimg_Cr, cvPoint(i*bin_w,histimg_Cr->height),
		//////////			cvPoint((i+1)*bin_w,histimg_Cr->height - val),
		//////////			CV_RGB(100,20,30), -1, 8, 0 );
		//////////}
		//////////cvShowImage("Transfered Cr Hist", histimg_Cr);

		//////////for( int i = 0; i < Cb_BINS; i++ )
		//////////{
		//////////	long int val = 0;
		//////////	for (int j=0; j<Cr_BINS; j++)
		//////////		val += cvRound( cvGetReal2D(getHist->bins,j, i));
		//////////	val = val/Cr_BINS/128;
		//////////	
		//////////	cvRectangle( histimg_Cb, cvPoint(i*bin_w,histimg_Cb->height),
		//////////			cvPoint((i+1)*bin_w,histimg_Cb->height - val),
		//////////			CV_RGB(100,20,30), -1, 8, 0 );
		//////////}
		//////////cvShowImage("Transfered Cb Hist", histimg_Cb);

		Sleep(50);
	}
	return 0;
}
unsigned WINAPI getThread_CAM1(LPVOID)
{
	time_t curTime, lastTime;
	AliveStaMgr_CAM1 as_cam1;
	CamLineMgr_CAM1_OB1 clinemgr_OB1;
	CamLineMgr_CAM1_OB2 clinemgr_OB2;
	CamLineMgr_CAM1_OB3 clinemgr_OB3;
	CamLineMgr_CAM1_OB4 clinemgr_OB4;
	CamLineMgr_CAM1_OB5 clinemgr_OB5;

	time(&lastTime);

	clinemgr_OB1.timeIndex = clinemgr_OB2.timeIndex = clinemgr_OB3.timeIndex = clinemgr_OB4.timeIndex = clinemgr_OB5.timeIndex = -333;

	//bool NeedUpdateAlive = true, NeedUpdate1 = false, NeedUpdate2 = false, NeedUpdate3 = false, NeedUpdate4 = false, NeedUpdate5 = false;
	while(1)
	{
		//time(&curTime);
		getAliveSta(as_cam1);

//		if (((int)as_cam1.timeIndex%10000) != ((int)lastTime%10000 + 1))
		{
//			continue;
		}

		curTime = (int)as_cam1.timeIndex%10000;
		
		if (as_cam1.aliveArr[0]) 
		{
//			while((cvWaitKey(5) != 27) && ((int)clinemgr_OB1.timeIndex%10000 != (int)curTime%10000))
			{
				getCamLine(clinemgr_OB1);
//				cout << "CAM1 OB1 " << (int)clinemgr_OB1.timeIndex%10000 << endl;
//				cout << clinemgr_OB1.imageLoc[0] << endl;
			}
		}
		if (as_cam1.aliveArr[1]) 
		{
//			while((cvWaitKey(5) != 27) && ((int)clinemgr_OB2.timeIndex%10000 != (int)curTime%10000))
			{
				getCamLine(clinemgr_OB2);
//				cout << "CAM1 OB2 " << (int)clinemgr_OB2.timeIndex%10000 << endl;
//				cout << clinemgr_OB2.imageLoc[0] << endl;
			}
		}
		if (as_cam1.aliveArr[2]) 
		{
//			while((cvWaitKey(5) != 27) && ((int)clinemgr_OB3.timeIndex%10000 != (int)curTime%10000))
			{
				getCamLine(clinemgr_OB3);
//				cout << "CAM1 OB3 " << (int)clinemgr_OB3.timeIndex%10000 << endl;
//				cout << clinemgr_OB3.imageLoc[0] << endl;
			}
		}
		if (as_cam1.aliveArr[3]) 
		{
//			while((cvWaitKey(5) != 27) && ((int)clinemgr_OB4.timeIndex%10000 != (int)curTime%10000))
			{
				getCamLine(clinemgr_OB4);
//				cout << "CAM1 OB4 " << (int)clinemgr_OB4.timeIndex%10000 << endl;
//				cout << clinemgr_OB4.imageLoc[0] << endl;
			}
		}
		if (as_cam1.aliveArr[4]) 
		{
//			while((cvWaitKey(5) != 27) && ((int)clinemgr_OB5.timeIndex%10000 != (int)curTime%10000))
			{
				getCamLine(clinemgr_OB5);
//				cout << "CAM1 OB5 " << (int)clinemgr_OB5.timeIndex%10000 << endl;
//				cout << clinemgr_OB5.imageLoc[0] << endl;
			}
		}

		lastTime = lastTime + 1;
		for (int i=0; i<MAX_TARGET_NUM; i++)
		{
			CameraLine3D_CAM1[PREV + i*3] = CameraLine3D_CAM1[LAST + i*3];
			CameraLine3D_CAM1[LAST + i*3] = CameraLine3D_CAM1[CURR + i*3];
		}

		cvCopy(imgResCr_CAM1[LAST], imgResCr_CAM1[PREV]);
		cvCopy(imgResCr_CAM1[CURR], imgResCr_CAM1[LAST]);
		cvCopy(imgResCb_CAM1[LAST], imgResCb_CAM1[PREV]);
		cvCopy(imgResCb_CAM1[CURR], imgResCb_CAM1[LAST]);

		Cam1DispReady = false;

		cvZero(imgResCr_CAM1[CURR]);
		cvZero(imgResCb_CAM1[CURR]);

		if (as_cam1.aliveArr[0]) trans2CamLine(&clinemgr_OB1, &CameraLine3D_CAM1[CURR + 0*3], imgResCr_CAM1[CURR], imgResCb_CAM1[CURR]);
		if (as_cam1.aliveArr[1]) trans2CamLine(&clinemgr_OB2, &CameraLine3D_CAM1[CURR + 1*3], imgResCr_CAM1[CURR], imgResCb_CAM1[CURR]);
		if (as_cam1.aliveArr[2]) trans2CamLine(&clinemgr_OB3, &CameraLine3D_CAM1[CURR + 2*3], imgResCr_CAM1[CURR], imgResCb_CAM1[CURR]);
		if (as_cam1.aliveArr[3]) trans2CamLine(&clinemgr_OB4, &CameraLine3D_CAM1[CURR + 3*3], imgResCr_CAM1[CURR], imgResCb_CAM1[CURR]);
		if (as_cam1.aliveArr[4]) trans2CamLine(&clinemgr_OB5, &CameraLine3D_CAM1[CURR + 4*3], imgResCr_CAM1[CURR], imgResCb_CAM1[CURR]);
		
		Cam1DispReady = true;
		cvWaitKey(100);
		
		 
		Alive_CAM1[PREV] = Alive_CAM1[LAST];
		Alive_CAM1[LAST] = Alive_CAM1[CURR];
		Alive_CAM1[CURR] = as_cam1;

		

	}
	return 0;
}
unsigned WINAPI getThread_CAM2(LPVOID)
{
	time_t curTime, lastTime;
	AliveStaMgr_CAM2 as_cam2;
	CamLineMgr_CAM2_OB1 clinemgr_OB1;
	CamLineMgr_CAM2_OB2 clinemgr_OB2;
	CamLineMgr_CAM2_OB3 clinemgr_OB3;
	CamLineMgr_CAM2_OB4 clinemgr_OB4;
	CamLineMgr_CAM2_OB5 clinemgr_OB5;

	time(&lastTime);

	clinemgr_OB1.timeIndex = clinemgr_OB2.timeIndex = clinemgr_OB3.timeIndex = clinemgr_OB4.timeIndex = clinemgr_OB5.timeIndex = -333;

	//bool NeedUpdateAlive = true, NeedUpdate1 = false, NeedUpdate2 = false, NeedUpdate3 = false, NeedUpdate4 = false, NeedUpdate5 = false;
	while(1)
	{
		//time(&curTime);
		getAliveSta(as_cam2);

//		if (((int)as_cam2.timeIndex%10000) != ((int)lastTime%10000 + 1))
		{
//			continue;
		}

		curTime = (int)as_cam2.timeIndex%10000;
		
		if (as_cam2.aliveArr[0]) 
		{
//			while((cvWaitKey(5) != 27) && ((int)clinemgr_OB1.timeIndex%10000 != (int)curTime%10000))
			{
				getCamLine(clinemgr_OB1);
//				cout << "CAM2 OB1 " << (int)clinemgr_OB1.timeIndex%10000 << endl;
//				cout << clinemgr_OB1.imageLoc[0] << endl;
			}
		}
		if (as_cam2.aliveArr[1]) 
		{
//			while((cvWaitKey(5) != 27) && ((int)clinemgr_OB2.timeIndex%10000 != (int)curTime%10000))
			{
				getCamLine(clinemgr_OB2);
//				cout << "CAM2 OB2 " << (int)clinemgr_OB2.timeIndex%10000 << endl;
//				cout << clinemgr_OB2.imageLoc[0] << endl;
			}
		}
		if (as_cam2.aliveArr[2]) 
		{
//			while((cvWaitKey(5) != 27) && ((int)clinemgr_OB3.timeIndex%10000 != (int)curTime%10000))
			{
				getCamLine(clinemgr_OB3);
//				cout << "CAM2 OB3 " << (int)clinemgr_OB3.timeIndex%10000 << endl;
//				cout << clinemgr_OB3.imageLoc[0] << endl;
			}
		}
		if (as_cam2.aliveArr[3]) 
		{
//			while((cvWaitKey(5) != 27) && ((int)clinemgr_OB4.timeIndex%10000 != (int)curTime%10000))
			{
				getCamLine(clinemgr_OB4);
//				cout << "CAM2 OB4 " << (int)clinemgr_OB4.timeIndex%10000 << endl;
//				cout << clinemgr_OB4.imageLoc[0] << endl;
			}
		}
		if (as_cam2.aliveArr[4]) 
		{
//			while((cvWaitKey(5) != 27) && ((int)clinemgr_OB5.timeIndex%10000 != (int)curTime%10000))
			{
				getCamLine(clinemgr_OB5);
//				cout << "CAM2 OB5 " << (int)clinemgr_OB5.timeIndex%10000 << endl;
//				cout << clinemgr_OB5.imageLoc[0] << endl;
			}
		}

		lastTime = lastTime + 1;
		for (int i=0; i<MAX_TARGET_NUM; i++)
		{
			CameraLine3D_CAM2[PREV + i*3] = CameraLine3D_CAM2[LAST + i*3];
			CameraLine3D_CAM2[LAST + i*3] = CameraLine3D_CAM2[CURR + i*3];
		}

		cvCopy(imgResCr_CAM2[LAST], imgResCr_CAM2[PREV]);
		cvCopy(imgResCr_CAM2[CURR], imgResCr_CAM2[LAST]);
		cvCopy(imgResCb_CAM2[LAST], imgResCb_CAM2[PREV]);
		cvCopy(imgResCb_CAM2[CURR], imgResCb_CAM2[LAST]);

		Cam2DispReady = false;

		cvZero(imgResCr_CAM2[CURR]);
		cvZero(imgResCb_CAM2[CURR]);

		if (as_cam2.aliveArr[0]) trans2CamLine(&clinemgr_OB1, &CameraLine3D_CAM2[CURR + 0*3], imgResCr_CAM2[CURR], imgResCb_CAM2[CURR]);
		if (as_cam2.aliveArr[1]) trans2CamLine(&clinemgr_OB2, &CameraLine3D_CAM2[CURR + 1*3], imgResCr_CAM2[CURR], imgResCb_CAM2[CURR]);
		if (as_cam2.aliveArr[2]) trans2CamLine(&clinemgr_OB3, &CameraLine3D_CAM2[CURR + 2*3], imgResCr_CAM2[CURR], imgResCb_CAM2[CURR]);
		if (as_cam2.aliveArr[3]) trans2CamLine(&clinemgr_OB4, &CameraLine3D_CAM2[CURR + 3*3], imgResCr_CAM2[CURR], imgResCb_CAM2[CURR]);
		if (as_cam2.aliveArr[4]) trans2CamLine(&clinemgr_OB5, &CameraLine3D_CAM2[CURR + 4*3], imgResCr_CAM2[CURR], imgResCb_CAM2[CURR]);
		
		Cam2DispReady = true;
		cvWaitKey(100);
		
		 
		Alive_CAM2[PREV] = Alive_CAM2[LAST];
		Alive_CAM2[LAST] = Alive_CAM2[CURR];
		Alive_CAM2[CURR] = as_cam2;

		

	}
	return 0;
}
unsigned WINAPI sendThread_CAM1(LPVOID)
{
	int q = 0, bias = 0;
	while(cvWaitKey(700) != 27)
	{
		q ++;
		bias += 70;
		AliveStaMgr_CAM1 aliveSta;
		time_t timeNow;
		time(&timeNow);
		aliveSta.timeIndex = (int)timeNow;
		for (int h=0; h<MAX_TARGET_NUM; h++)
			CameraLine3D_CAM1[CURR + h*3].imageLoc = cvPoint((h*30 + bias)%320, 160);
		for (int i=0; i<MAX_TARGET_NUM; i++)
		{
			aliveSta.aliveArr[i] = true;
		}
		setAliveSta(aliveSta);	

		if (aliveSta.aliveArr[0])
		{
			trans2CamLineMgr(imgCr, imgCb, &CameraLine3D_CAM1[CURR + 0*3], &camlinemgr11, aliveSta.timeIndex);
			setCamLine(camlinemgr11);
		}
		if (aliveSta.aliveArr[1])
		{
			trans2CamLineMgr(imgCr, imgCb, &CameraLine3D_CAM1[CURR + 1*3], &camlinemgr12, aliveSta.timeIndex);
			setCamLine(camlinemgr12);
		}
		if (aliveSta.aliveArr[2])
		{
			trans2CamLineMgr(imgCr, imgCb, &CameraLine3D_CAM1[CURR + 2*3], &camlinemgr13, aliveSta.timeIndex);
			setCamLine(camlinemgr13);
		}
		if (aliveSta.aliveArr[3])
		{
			trans2CamLineMgr(imgCr, imgCb, &CameraLine3D_CAM1[CURR + 3*3], &camlinemgr14, aliveSta.timeIndex);
			setCamLine(camlinemgr14);
		}
		if (aliveSta.aliveArr[4])
		{
			trans2CamLineMgr(imgCr, imgCb, &CameraLine3D_CAM1[CURR + 4*3], &camlinemgr15, aliveSta.timeIndex);
			setCamLine(camlinemgr15);
		}
	}
	return 0;
}
unsigned WINAPI sendThread_CAM2(LPVOID)
{
	int q = 0, bias = 0;
	while(cvWaitKey(700) != 27)
	{
		q ++;
		bias += 70;
		AliveStaMgr_CAM2 aliveSta;
		time_t timeNow;
		time(&timeNow);
		aliveSta.timeIndex = (int)timeNow;
		for (int h=0; h<MAX_TARGET_NUM; h++)
			CameraLine3D_CAM2[CURR + h*3].imageLoc = cvPoint((h*30 + bias)%320, 160);

		for (int i=0; i<MAX_TARGET_NUM; i++)
		{
			aliveSta.aliveArr[i] = true;
		}
		setAliveSta(aliveSta);	

		if (aliveSta.aliveArr[0])
		{
			trans2CamLineMgr(imgCr, imgCb, &CameraLine3D_CAM2[CURR + 0*3], &camlinemgr21, aliveSta.timeIndex);
			setCamLine(camlinemgr21);
		}
		if (aliveSta.aliveArr[1])
		{
			trans2CamLineMgr(imgCr, imgCb, &CameraLine3D_CAM2[CURR + 1*3], &camlinemgr22, aliveSta.timeIndex);
			setCamLine(camlinemgr22);
		}
		if (aliveSta.aliveArr[2])
		{
			trans2CamLineMgr(imgCr, imgCb, &CameraLine3D_CAM2[CURR + 2*3], &camlinemgr23, aliveSta.timeIndex);
			setCamLine(camlinemgr23);
		}
		if (aliveSta.aliveArr[3])
		{
			trans2CamLineMgr(imgCr, imgCb, &CameraLine3D_CAM2[CURR + 3*3], &camlinemgr24, aliveSta.timeIndex);
			setCamLine(camlinemgr24);
		}
		if (aliveSta.aliveArr[4])
		{
			trans2CamLineMgr(imgCr, imgCb, &CameraLine3D_CAM2[CURR + 4*3], &camlinemgr25, aliveSta.timeIndex);
			setCamLine(camlinemgr25);
		}
	}
	return 0;
}
void showImage()
{
		int bin_w = histimg_Cr->width / Cr_BINS;
		cvZero(histimg_Cr);
		cvZero(histimg_Cb);
		for( int i = 0; i < Cr_BINS; i++ )
		{
			long int val = 0;
			for (int j=0; j<Cb_BINS; j++)
				val += cvRound( cvGetReal2D(getHist->bins,i, j));

			val = val/Cr_BINS*9;///128;
			
			cvRectangle( histimg_Cr, cvPoint(i*bin_w,histimg_Cr->height),
					cvPoint((i+1)*bin_w,histimg_Cr->height - val),
					CV_RGB(100,20,30), -1, 8, 0 );
		}
		cvShowImage("Transfered Cr Hist", histimg_Cr);

		for( int i = 0; i < Cb_BINS; i++ )
		{
			long int val = 0;
			for (int j=0; j<Cr_BINS; j++)
				val += cvRound( cvGetReal2D(getHist->bins,j, i));
			val = val/Cr_BINS*9;///128;
			
			cvRectangle( histimg_Cb, cvPoint(i*bin_w,histimg_Cb->height),
					cvPoint((i+1)*bin_w,histimg_Cb->height - val),
					CV_RGB(100,20,30), -1, 8, 0 );
		}
		cvShowImage("Transfered Cb Hist", histimg_Cb);
		cvShowImage("imgResCr_CAM1[CURR]", imgResCr_CAM1[CURR]);
		cvShowImage("imgResCb_CAM1[CURR]", imgResCb_CAM1[CURR]);
}
void showReceiveData()
{
	time_t now;
	time(&now);
	cout << "now " << (int)now << endl;
	cout << "CAM1 " << (int)Alive_CAM1[CURR].timeIndex%10000 <<endl; 
	cout << "CAM2 " << (int)Alive_CAM2[CURR].timeIndex%10000 <<endl;

	cvShowImage("imgResCr_CAM1[CURR]", imgResCr_CAM1[CURR]);
	cvShowImage("imgResCb_CAM1[CURR]", imgResCb_CAM1[CURR]);
	cvShowImage("imgResCr_CAM2[CURR]", imgResCr_CAM2[CURR]);
	cvShowImage("imgResCb_CAM2[CURR]", imgResCb_CAM2[CURR]);
}