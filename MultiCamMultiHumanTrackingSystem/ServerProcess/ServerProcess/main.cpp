
#include "Client.hpp"
#include "targetDatabase.h"
#include <iostream>
#include "process.h"

using namespace std;

unsigned WINAPI sendThread(LPVOID);
unsigned WINAPI getThread(LPVOID);

unsigned WINAPI sendThread2(LPVOID);
unsigned WINAPI getThread2(LPVOID);
unsigned WINAPI getThread_CAM1(LPVOID);
unsigned WINAPI getThread_CAM2(LPVOID);
unsigned recordGMMVideoThread( void *pMyID );

#define MAINTAINED_NUM 3
#define PREV 2
#define LAST 1
#define CURR 0

#define REC_SAMPLINGRATE 25

IplImage *sample_img, *sample_YCrCb, *sample_Y, *sample_Cr, *sample_Cb, *histimg_Cr, *histimg_Cb, *hist_mask;
CvHistogram *refHist, *getHist;
IplImage *img, *imgCr, *imgCb, *imgY, *imgYCrCb;
IplImage* imgResCr_CAM1[MAINTAINED_NUM];
IplImage* imgResCb_CAM1[MAINTAINED_NUM];
IplImage* imgResY_CAM1[MAINTAINED_NUM];
IplImage* imgResYCrCb_CAM1[MAINTAINED_NUM];
IplImage* imgRes_CAM1[MAINTAINED_NUM];
IplImage* imgResHSV_CAM1[MAINTAINED_NUM];
IplImage* imgResH_CAM1[MAINTAINED_NUM];
IplImage* imgResS_CAM1[MAINTAINED_NUM];
IplImage* imgResV_CAM1[MAINTAINED_NUM];
IplImage* imgResCr_CAM2[MAINTAINED_NUM];
IplImage* imgResCb_CAM2[MAINTAINED_NUM];
IplImage* imgResY_CAM2[MAINTAINED_NUM];
IplImage* imgResYCrCb_CAM2[MAINTAINED_NUM];
IplImage* imgRes_CAM2[MAINTAINED_NUM];
IplImage* imgResHSV_CAM2[MAINTAINED_NUM];
IplImage* imgResH_CAM2[MAINTAINED_NUM];
IplImage* imgResS_CAM2[MAINTAINED_NUM];
IplImage* imgResV_CAM2[MAINTAINED_NUM];
IplImage* imgResCr_CAM3[MAINTAINED_NUM];
IplImage* imgResCb_CAM3[MAINTAINED_NUM];
IplImage* imgResY_CAM3[MAINTAINED_NUM];
IplImage* imgResYCrCb_CAM3[MAINTAINED_NUM];
IplImage* imgRes_CAM3[MAINTAINED_NUM];
IplImage* imgResHSV_CAM3[MAINTAINED_NUM];
IplImage* imgResH_CAM3[MAINTAINED_NUM];
IplImage* imgResS_CAM3[MAINTAINED_NUM];
IplImage* imgResV_CAM3[MAINTAINED_NUM];
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

LabelIntegrateMgr_CAM1 integratedResult; //All Cameras share this

targetDatabase targetData;

//for rms error analysis
int sampleNum_H = 0;
int sampleNum_D = 0;
double stdDev_H = 0;
double mean_H = 0;
double stdDev_D = 0;
double mean_D = 0;
//for rms error analysis

long int GMMCnt = 0;
int shortCnt = 0;

bool* CurrAliveList_CAM1;
bool* CurrAliveList_CAM2;
bool* CurrAliveList_CAM3;
bool* PrevAliveList_CAM1;
bool* PrevAliveList_CAM2;
bool* PrevAliveList_CAM3;

bool occupiedList_cam1[MAX_TARGET_NUM];
bool occupiedList_cam2[MAX_TARGET_NUM];

int** objConfMap;
int** objExistCnt;
int** geoMatchCnt;
int** matchCntMap;
double** colorScoreMap;
bool Cam1DispReady, Cam2DispReady;
bool stopGMMVideo = false;

CvVideoWriter* GMMTargetVideo[5];

void prepare();
void showImage();
void syncThreadFilter();
bool syncTimeWithinCAMs(int & cam1_time, int & cam2_time, int & cam3_time);
void SolveObservationIntegration_greedy();
void SolveObservationIntegration_ConfidenceTable();
int correspondenceLikelihoodThresholding(int g, int f, int intersect_Z, Position3D headpt1, Position3D headpt2, double color_score, int matchCnt, int best_color_index, double best_color_dist, int CtsGeoMatch, int & Lv);
int correspondenceLikelihoodThresholding_powWeight(int g, int f, int intersect_Z, Position3D headpt1, Position3D headpt2, double color_log_score, int matchCnt, int best_color_index, double best_color_log_dist, int CtsGeoMatch, int & Lv);
void DrawIntegratedObservation(int CAMID, int tar_id, IplImage* image_Cr, IplImage* image_Cb, int Lv);
int sendIntResCnt = 0;
void SendIntegrateResult();
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
	//type1 A;
	//A.x = 1;
	//type2 B;
	//B.x = 1;
	//test(A);
	//test(B);





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
		//////////HISTCRCB_CAM1_OB1_NAME, 
		//////////HISTCRCB_CAM1_OB2_NAME,
		//////////HISTCRCB_CAM1_OB3_NAME,
		//////////HISTCRCB_CAM1_OB4_NAME,
		//////////HISTCRCB_CAM1_OB5_NAME,
		//////////HISTCRCB_CAM2_OB1_NAME, 
		//////////HISTCRCB_CAM2_OB2_NAME,
		//////////HISTCRCB_CAM2_OB3_NAME,
		//////////HISTCRCB_CAM2_OB4_NAME,
		//////////HISTCRCB_CAM2_OB5_NAME,
		//////////HISTCRCB_CAM3_OB1_NAME, 
		//////////HISTCRCB_CAM3_OB2_NAME,
		//////////HISTCRCB_CAM3_OB3_NAME,
		//////////HISTCRCB_CAM3_OB4_NAME,
		//////////HISTCRCB_CAM3_OB5_NAME,

		//////////ODOMETRY_NAME, GOAL_NAME,

		//////////TEST_NAME,
		//////////BODYINFO_CAM1_OB1_NAME,
		//////////BODYINFO_CAM1_OB2_NAME,
		//////////BODYINFO_CAM1_OB3_NAME,
		//////////BODYINFO_CAM1_OB4_NAME,
		//////////BODYINFO_CAM1_OB5_NAME,
		//////////BODYINFO_CAM2_OB1_NAME,
		//////////BODYINFO_CAM2_OB2_NAME,
		//////////BODYINFO_CAM2_OB3_NAME,
		//////////BODYINFO_CAM2_OB4_NAME,
		//////////BODYINFO_CAM2_OB5_NAME,
		//////////BODYINFO_CAM3_OB1_NAME,
		//////////BODYINFO_CAM3_OB2_NAME,
		//////////BODYINFO_CAM3_OB3_NAME,
		//////////BODYINFO_CAM3_OB4_NAME,
		//////////BODYINFO_CAM3_OB5_NAME,



		//////////POS3D_CAM1_OB1_NAME,
		//////////POS3D_CAM1_OB2_NAME,
		//////////POS3D_CAM1_OB3_NAME,
		//////////POS3D_CAM1_OB4_NAME,
		//////////POS3D_CAM1_OB5_NAME,
		//////////POS3D_CAM2_OB1_NAME,
		//////////POS3D_CAM2_OB2_NAME,
		//////////POS3D_CAM2_OB3_NAME,
		//////////POS3D_CAM2_OB4_NAME,
		//////////POS3D_CAM2_OB5_NAME,
		//////////POS3D_CAM3_OB1_NAME,
		//////////POS3D_CAM3_OB2_NAME,
		//////////POS3D_CAM3_OB3_NAME,
		//////////POS3D_CAM3_OB4_NAME,
		//////////POS3D_CAM3_OB5_NAME,



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

		//////////SUBGOAL_NAME, 
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
		//////////HISTCRCB_CAM1_OB1_NAME, 
		//////////HISTCRCB_CAM1_OB2_NAME,
		//////////HISTCRCB_CAM1_OB3_NAME,
		//////////HISTCRCB_CAM1_OB4_NAME,
		//////////HISTCRCB_CAM1_OB5_NAME,
		//////////HISTCRCB_CAM2_OB1_NAME, 
		//////////HISTCRCB_CAM2_OB2_NAME,
		//////////HISTCRCB_CAM2_OB3_NAME,
		//////////HISTCRCB_CAM2_OB4_NAME,
		//////////HISTCRCB_CAM2_OB5_NAME,
		//////////HISTCRCB_CAM3_OB1_NAME, 
		//////////HISTCRCB_CAM3_OB2_NAME,
		//////////HISTCRCB_CAM3_OB3_NAME,
		//////////HISTCRCB_CAM3_OB4_NAME,
		//////////HISTCRCB_CAM3_OB5_NAME,

		//////////ODOMETRY_NAME, GOAL_NAME,

		//////////TEST_NAME,
		//////////BODYINFO_CAM1_OB1_NAME,
		//////////BODYINFO_CAM1_OB2_NAME,
		//////////BODYINFO_CAM1_OB3_NAME,
		//////////BODYINFO_CAM1_OB4_NAME,
		//////////BODYINFO_CAM1_OB5_NAME,
		//////////BODYINFO_CAM2_OB1_NAME,
		//////////BODYINFO_CAM2_OB2_NAME,
		//////////BODYINFO_CAM2_OB3_NAME,
		//////////BODYINFO_CAM2_OB4_NAME,
		//////////BODYINFO_CAM2_OB5_NAME,
		//////////BODYINFO_CAM3_OB1_NAME,
		//////////BODYINFO_CAM3_OB2_NAME,
		//////////BODYINFO_CAM3_OB3_NAME,
		//////////BODYINFO_CAM3_OB4_NAME,
		//////////BODYINFO_CAM3_OB5_NAME,



		//////////POS3D_CAM1_OB1_NAME,
		//////////POS3D_CAM1_OB2_NAME,
		//////////POS3D_CAM1_OB3_NAME,
		//////////POS3D_CAM1_OB4_NAME,
		//////////POS3D_CAM1_OB5_NAME,
		//////////POS3D_CAM2_OB1_NAME,
		//////////POS3D_CAM2_OB2_NAME,
		//////////POS3D_CAM2_OB3_NAME,
		//////////POS3D_CAM2_OB4_NAME,
		//////////POS3D_CAM2_OB5_NAME,
		//////////POS3D_CAM3_OB1_NAME,
		//////////POS3D_CAM3_OB2_NAME,
		//////////POS3D_CAM3_OB3_NAME,
		//////////POS3D_CAM3_OB4_NAME,
		//////////POS3D_CAM3_OB5_NAME,



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

		//////////SUBGOAL_NAME,  
		NULL );




	// you can also use listen( time );
	IPCClient.listen();

	prepare();

	char a = (int)150;
	cout << "a = " << a << endl;
	cout << "(int)a = " << (int)a << endl;
	cout << "(unsigned char)a = " << (unsigned char)a << endl;
	cout << "(int)(unsigned char)a = " << (int)(unsigned char)a << endl;
	unsigned char b = (int)150;
	cout << "b = " << b << endl;
	cout << "(int)b = " << (int)b << endl;

	unsigned threadID;
	HANDLE hthreads[2];
	hthreads[0] = (HANDLE)_beginthreadex( NULL, 0, getThread_CAM1, NULL, 0, &threadID );
	hthreads[1] = (HANDLE)_beginthreadex( NULL, 0, getThread_CAM2, NULL, 0, &threadID );

	////////test
	//////int a = 2;
	//////double b = 2.1;
	//////float c = 3.1;
	//////cout << "int + double = " << a+b << endl;
	//////cout << "double + int = " << b+a << endl;
	//////cout << "double + float = " << b+c << endl;
	//////cout << "float + double = " << c+b << endl;
	//////cout << "double - int = " << b-(int)b << endl;
	//////cout << "bool + double = " << (bool)b + b << endl;
	//////cout << "double + bool = " << b + (bool)b << endl;
	//////cout << "int*double = " << a*b << endl;
	//////cout << "double*int = " << b*a << endl;
	//////cout << "double*double = " << b*b << endl;
	//////cout << "(int)double*double = " << (int)b*b << endl;
	//////cout << "((int)double)*double = " << ((int)b)*b << endl;
	//////cout << "double*((int)double) = " << b*((int)b) << endl;

	while(1)
	{
		if (Cam1DispReady && Cam2DispReady) showImage();
		if (cvWaitKey(30) == 27) break; 
	}

	WaitForMultipleObjects( 2, hthreads, TRUE, INFINITE );
	CloseHandle( hthreads[0] );
	CloseHandle( hthreads[1] );
	stopGMMVideo = true; 

}

//////////unsigned WINAPI sendThread(LPVOID)
//////////{
//////////	SubgoalMgr send_data;
//////////	AliveStaMgr_CAM1 as;
//////////	OdometryMgr od;
//////////	Pos3DMgr_CAM1_OB1 p;
//////////	GoalMgr gl;
//////////	test tt;
//////////	BodyInfoMgr_CAM1_OB1 bi;
//////////	HistCrCbMgr_CAM1_OB1 hcc;
//////////
//////////
//////////
//////////	as.timeIndex = 0;
//////////	send_data.x = 0;
//////////	od.x = 0;
//////////	p.x = 0;
//////////	gl.x = 0;
//////////	tt.a = 0;
//////////	//tt.s = 0.1;
//////////	hcc.timeIndex = 0;
//////////	bi.timeIndex = 0;
//////////	//while(1){
//////////	for(int i = 0 ; i < 100 ; ++i){
//////////		
//////////		send_data.x ++ ;
//////////		as.timeIndex ++;
//////////		od.x ++;
//////////		p.x ++;
//////////		gl.x ++;
//////////		tt.a ++;
//////////		//tt.s ++;
//////////		hcc.timeIndex ++;
//////////		bi.timeIndex ++;
//////////
//////////		setSubgoal( send_data );
//////////		setAliveSta( as );
//////////		setOdometry( od );
//////////		setPos3D( p );
//////////		setGoal( gl );
//////////		settest( tt );
//////////		setHistCrCb( hcc );
//////////		setBodyInfo( bi);
//////////
//////////		int a = true;
//////////		int b = false;
//////////		double c = true;
//////////		double d = false;
//////////		
//////////		double W = 2.5;
//////////		bool A = W;
//////////		W = -1.2;
//////////		bool B = W;
//////////		W = 0;
//////////		bool C = W;
//////////		
//////////		int Q = 2;
//////////		bool D = Q;
//////////		Q = -2;
//////////		bool E = Q;
//////////		Q = 0;
//////////		bool F = Q;
//////////
//////////		cout << "SET SUB : " << send_data.x << endl;
//////////		cout << "SET timeIndex : " << as.timeIndex << endl;
//////////		cout << "SET Odometry : " << od.x << endl;
//////////		cout << "SET POS : " << p.x << endl;
//////////		cout << "SET GOAL : " << gl.x << endl;
//////////		cout << "SET TEST : " << tt.a << endl;
//////////		//cout << "SET TEST : " << tt.s << endl;
//////////		cout << "SET HISTCC : " << hcc.timeIndex << endl;
//////////		cout << "SET BDIF  : " << bi.timeIndex << endl;
//////////		cout << "int true : " << a << endl;
//////////		cout << "int false : " << b << endl;
//////////		cout << "double true : " << c << endl;
//////////		cout << "double false : " << d << endl;
//////////		cout << "bool 2.5 : " << A << endl;
//////////		cout << "bool -1.2 : " << B << endl;
//////////		cout << "bool (double)0 : " << C << endl;
//////////		cout << "bool (int)2 : " << D << endl;
//////////		cout << "bool (int)-2 : " << E << endl;
//////////		cout << "bool (int)0 : " << F << endl;
//////////		Sleep(100);
//////////	}
//////////	return 0;
//////////}
//////////
//////////unsigned WINAPI getThread(LPVOID)
//////////{
//////////	SubgoalMgr get_data;
//////////	AliveStaMgr_CAM1 as;
//////////	OdometryMgr od;
//////////	Pos3DMgr_CAM1_OB1 p;
//////////	GoalMgr gl;
//////////	test tt;
//////////	HistCrCbMgr_CAM1_OB1 hcc;
//////////	BodyInfoMgr_CAM1_OB1 bi;
//////////
//////////	//while(1){
//////////	for(int i = 0 ; i < 100 ; ++i){
//////////		
//////////		getSubgoal( get_data );
//////////		getAliveSta( as );
//////////		getOdometry( od );
//////////		getPos3D( p );
//////////		getGoal( gl );
//////////		gettest( tt );
//////////		getHistCrCb( hcc );
//////////		getBodyInfo( bi );
//////////
//////////		cout << "				GET SUB : " << get_data.x << endl;
//////////		cout << "				GET timeIndex : " << as.timeIndex << endl;
//////////		cout << "				GET Odometry : " << od.x << endl;
//////////		cout << "				GET POS : " << p.x << endl;
//////////		cout << "				GET GOAL : " << gl.x << endl;
//////////		cout << "				GET TEST : " << tt.a << endl;
//////////		//cout << "				GET TEST : " << tt.s << endl;
//////////		cout << "				GET HISTCC : " << hcc.timeIndex << endl;
//////////		cout << "				GET BDIF : " << bi.timeIndex << endl;
//////////		Sleep(100);
//////////	}
//////////	return 0;
//////////}

void prepare()
{
	for (int y=0; y<MAX_TARGET_NUM*3; y++)
	{
		integratedResult.Alive[y] = -1;//store conf Lv; target index maps to OBJECT conf level (not TARGET conf level)
		integratedResult.CAM1_id[y] = -1;//store target index; observation index maps to target index
		integratedResult.CAM2_id[y] = -1;//store target index; observation index maps to target index
		integratedResult.CAM3_id[y] = -1;//store target index; observation index maps to target index
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
	cvNamedWindow("imgResH_CAM1[CURR]", 1);
	cvNamedWindow("imgResS_CAM1[CURR]", 1);
	cvNamedWindow("imgRes_CAM1[CURR]", 1);
	cvNamedWindow("imgResH_CAM2[CURR]", 1);
	cvNamedWindow("imgResS_CAM2[CURR]", 1);
	cvNamedWindow("imgRes_CAM2[CURR]", 1);
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
		imgResY_CAM1[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResYCrCb_CAM1[i] = cvCreateImage(cvGetSize(img), 8, 3);
		imgRes_CAM1[i] = cvCreateImage(cvGetSize(img), 8, 3);
		imgResHSV_CAM1[i] = cvCreateImage(cvGetSize(img), 8, 3);
		imgResH_CAM1[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResS_CAM1[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResV_CAM1[i] = cvCreateImage(cvGetSize(img), 8, 1);
		cvZero(imgResCr_CAM1[i]);
		cvZero(imgResCb_CAM1[i]);
		cvZero(imgResY_CAM1[i]);
		cvZero(imgResYCrCb_CAM1[i]);
		cvZero(imgRes_CAM1[i]);
		cvZero(imgResHSV_CAM1[i]);
		cvZero(imgResH_CAM1[i]);
		cvZero(imgResS_CAM1[i]);
		cvZero(imgResV_CAM1[i]);
		imgResCr_CAM2[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResCb_CAM2[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResY_CAM2[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResYCrCb_CAM2[i] = cvCreateImage(cvGetSize(img), 8, 3);
		imgRes_CAM2[i] = cvCreateImage(cvGetSize(img), 8, 3);
		imgResHSV_CAM2[i] = cvCreateImage(cvGetSize(img), 8, 3);
		imgResH_CAM2[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResS_CAM2[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResV_CAM2[i] = cvCreateImage(cvGetSize(img), 8, 1);
		cvZero(imgResCr_CAM2[i]);
		cvZero(imgResCb_CAM2[i]);
		cvZero(imgResY_CAM2[i]);
		cvZero(imgResYCrCb_CAM2[i]);
		cvZero(imgRes_CAM2[i]);
		cvZero(imgResHSV_CAM2[i]);
		cvZero(imgResH_CAM2[i]);
		cvZero(imgResS_CAM2[i]);
		cvZero(imgResV_CAM2[i]);
		imgResCr_CAM3[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResCb_CAM3[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResY_CAM3[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResYCrCb_CAM3[i] = cvCreateImage(cvGetSize(img), 8, 3);
		imgRes_CAM3[i] = cvCreateImage(cvGetSize(img), 8, 3);
		imgResHSV_CAM3[i] = cvCreateImage(cvGetSize(img), 8, 3);
		imgResH_CAM3[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResS_CAM3[i] = cvCreateImage(cvGetSize(img), 8, 1);
		imgResV_CAM3[i] = cvCreateImage(cvGetSize(img), 8, 1);
		cvZero(imgResCr_CAM3[i]);
		cvZero(imgResCb_CAM3[i]);
		cvZero(imgResY_CAM3[i]);
		cvZero(imgResYCrCb_CAM3[i]);
		cvZero(imgRes_CAM3[i]);
		cvZero(imgResHSV_CAM3[i]);
		cvZero(imgResH_CAM3[i]);
		cvZero(imgResS_CAM3[i]);
		cvZero(imgResV_CAM3[i]);
	}

	
	CurrAliveList_CAM1 = new bool[MAX_TARGET_NUM];
	CurrAliveList_CAM2 = new bool[MAX_TARGET_NUM];
	CurrAliveList_CAM3 = new bool[MAX_TARGET_NUM];
	PrevAliveList_CAM1 = new bool[MAX_TARGET_NUM];
	PrevAliveList_CAM2 = new bool[MAX_TARGET_NUM];
	PrevAliveList_CAM3 = new bool[MAX_TARGET_NUM];
	objConfMap = new int*[MAX_TARGET_NUM];
	objExistCnt = new int*[MAX_TARGET_NUM];
	geoMatchCnt = new int*[MAX_TARGET_NUM];
	matchCntMap = new int*[MAX_TARGET_NUM];
	colorScoreMap = new double*[MAX_TARGET_NUM];
	for (int g=0; g<MAX_TARGET_NUM; g++)
	{
		targetHeight_CAM1[g] = targetHeight_CAM2[g] = targetHeight_CAM3[g] = DEFAULT_H;
		occupiedList_cam1[g] = occupiedList_cam2[g] = false;
		PrevAliveList_CAM1[g] = PrevAliveList_CAM2[g] = PrevAliveList_CAM3[g] = false;
		objConfMap[g] = new int[MAX_TARGET_NUM];
		objExistCnt[g] = new int[MAX_TARGET_NUM];
		geoMatchCnt[g] = new int[MAX_TARGET_NUM];
		matchCntMap[g] = new int[MAX_TARGET_NUM];
		colorScoreMap[g] = new double[MAX_TARGET_NUM];
		for (int f=0; f<MAX_TARGET_NUM; f++)
		{
			objConfMap[g][f] = 0;
			objExistCnt[g][f] = 0;
			geoMatchCnt[g][f] = 0;
			matchCntMap[g][f] = 0;
			colorScoreMap[g][f] = NONDEFINED;
		}

	}
 

}

//////////unsigned WINAPI sendThread2(LPVOID)
//////////{
//////////	//type1 A;
//////////	//A.x = 1;
//////////	//type2 B;
//////////	//B.x = 1;
//////////	//test(A);
//////////	//test(B);
//////////
//////////
//////////
//////////	BodyInfoMgr_CAM1_OB1 send_data;
//////////	BodyInfo bi;
//////////	HistCrCbMgr_CAM1_OB1 shist;
//////////	trans2HistCrCbMgr(refHist, &shist, 200);
//////////	CamLineMgr_CAM1_OB1 linemgr;
//////////	CamLineMgr_CAM1_OB4 linemgrb;
//////////	CameraLine3D line, lineb;
//////////	line.camID = CAM1;
//////////
//////////
//////////	bi.height = 10;
//////////	line.cm2pixelRatio = 10.2;
//////////	lineb = line;
//////////	line.imageLoc = cvPoint(80, 40);
//////////	lineb.imageLoc = cvPoint(220, 150);
//////////	//while(1){
//////////	for(int i = 0 ; i < 10000 ; ++i){
//////////		
//////////		bi.height ++ ;
//////////		line.cm2pixelRatio ++;
//////////		lineb.cm2pixelRatio ++;
//////////		trans2BodyInfoMgr(&bi, &send_data, 100);
//////////		trans2CamLineMgr(imgCr, imgCb, &line, &linemgr, 150);
//////////		trans2CamLineMgr(imgCr, imgCb, &lineb, &linemgrb, 150);
//////////		setBodyInfo( send_data );
//////////		setCamLine( linemgr );
//////////		setCamLine( linemgrb );
//////////		setHistCrCb( shist );
//////////		cout << "SET HEIGHT : " << send_data.height << endl;
//////////		cout << "SET RATIO : " << linemgr.cm2pixelRatio << endl;
//////////		cout << "SET B RATIO : " << linemgrb.cm2pixelRatio << endl;
//////////		Sleep(50);
//////////	}
//////////	return 0;
//////////}
//////////
//////////unsigned WINAPI getThread2(LPVOID)
//////////{
//////////	//type1 A;
//////////	//A.x = 1;
//////////	//type2 B;
//////////	//B.x = 1;
//////////	//test(A);
//////////	//test(B);
//////////
//////////	HistCrCbMgr_CAM1_OB1 ghist;
//////////
//////////	BodyInfoMgr_CAM1_OB1 get_data;
//////////	BodyInfo bi;
//////////	CamLineMgr_CAM1_OB1 linemgr;
//////////	CamLineMgr_CAM1_OB4 linemgrb;
//////////	CameraLine3D line, lineb;
//////////
//////////
//////////	//while(1){
//////////	for(int i = 0 ; i < 10000 ; ++i){
//////////		
//////////		getBodyInfo( get_data );
//////////		getCamLine( linemgr);
//////////		getCamLine( linemgrb);
//////////		getHistCrCb( ghist );
//////////		trans2BodyInfo(&get_data, &bi);
//////////		trans2CamLine(&linemgr, &line, imgResCr_CAM1[CURR], imgResCb_CAM1[CURR]);
//////////		trans2CamLine(&linemgrb, &lineb, imgResCr_CAM1[CURR], imgResCb_CAM1[CURR]);
//////////		trans2HistCrCb(&ghist, getHist);
//////////		cout << "							GET HEIGHT : " << bi.height << endl;
//////////		cout << "							GET RATIO : " << line.cm2pixelRatio << endl;
//////////		cout << "							GET RATIO : " << lineb.cm2pixelRatio << endl;
//////////
//////////		//////////int bin_w = histimg_Cr->width / Cr_BINS;
//////////		//////////cvZero(histimg_Cr);
//////////		//////////cvZero(histimg_Cb);
//////////		//////////for( int i = 0; i < Cr_BINS; i++ )
//////////		//////////{
//////////		//////////	long int val = 0;
//////////		//////////	for (int j=0; j<Cb_BINS; j++)
//////////		//////////		val += cvRound( cvGetReal2D(getHist->bins,i, j));
//////////
//////////		//////////	val = val/Cr_BINS/128;
//////////		//////////	
//////////		//////////	cvRectangle( histimg_Cr, cvPoint(i*bin_w,histimg_Cr->height),
//////////		//////////			cvPoint((i+1)*bin_w,histimg_Cr->height - val),
//////////		//////////			CV_RGB(100,20,30), -1, 8, 0 );
//////////		//////////}
//////////		//////////cvShowImage("Transfered Cr Hist", histimg_Cr);
//////////
//////////		//////////for( int i = 0; i < Cb_BINS; i++ )
//////////		//////////{
//////////		//////////	long int val = 0;
//////////		//////////	for (int j=0; j<Cr_BINS; j++)
//////////		//////////		val += cvRound( cvGetReal2D(getHist->bins,j, i));
//////////		//////////	val = val/Cr_BINS/128;
//////////		//////////	
//////////		//////////	cvRectangle( histimg_Cb, cvPoint(i*bin_w,histimg_Cb->height),
//////////		//////////			cvPoint((i+1)*bin_w,histimg_Cb->height - val),
//////////		//////////			CV_RGB(100,20,30), -1, 8, 0 );
//////////		//////////}
//////////		//////////cvShowImage("Transfered Cb Hist", histimg_Cb);
//////////
//////////		Sleep(50);
//////////	}
//////////	return 0;
//////////}
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
		cvZero(imgResY_CAM1[CURR]);

		if (as_cam1.aliveArr[0]) trans2CamLine(&clinemgr_OB1, &CameraLine3D_CAM1[CURR + 0*3], imgResCr_CAM1[CURR], imgResCb_CAM1[CURR], imgResY_CAM1[CURR]);
		if (as_cam1.aliveArr[1]) trans2CamLine(&clinemgr_OB2, &CameraLine3D_CAM1[CURR + 1*3], imgResCr_CAM1[CURR], imgResCb_CAM1[CURR], imgResY_CAM1[CURR]);
		if (as_cam1.aliveArr[2]) trans2CamLine(&clinemgr_OB3, &CameraLine3D_CAM1[CURR + 2*3], imgResCr_CAM1[CURR], imgResCb_CAM1[CURR], imgResY_CAM1[CURR]);
		if (as_cam1.aliveArr[3]) trans2CamLine(&clinemgr_OB4, &CameraLine3D_CAM1[CURR + 3*3], imgResCr_CAM1[CURR], imgResCb_CAM1[CURR], imgResY_CAM1[CURR]);
		if (as_cam1.aliveArr[4]) trans2CamLine(&clinemgr_OB5, &CameraLine3D_CAM1[CURR + 4*3], imgResCr_CAM1[CURR], imgResCb_CAM1[CURR], imgResY_CAM1[CURR]);
		
		cvMerge(imgResY_CAM1[CURR], imgResCr_CAM1[CURR], imgResCb_CAM1[CURR], NULL, imgResYCrCb_CAM1[CURR]);
		cvCvtColor(imgResYCrCb_CAM1[CURR], imgRes_CAM1[CURR], CV_YCrCb2BGR);
		cvCvtColor(imgRes_CAM1[CURR], imgResHSV_CAM1[CURR], CV_BGR2HSV);
		cvSplit(imgResHSV_CAM1[CURR], imgResH_CAM1[CURR], imgResS_CAM1[CURR], imgResV_CAM1[CURR], NULL);
		Cam1DispReady = true;
		cvWaitKey(20);
		
		 
		Alive_CAM1[PREV] = Alive_CAM1[LAST];
		Alive_CAM1[LAST] = Alive_CAM1[CURR];
		Alive_CAM1[CURR] = as_cam1;
		for (int h=0; h<MAX_TARGET_NUM; h++)
		{
			CurrAliveList_CAM1[h] = as_cam1.aliveArr[h];
		}
		

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
		cvZero(imgResY_CAM2[CURR]);

		if (as_cam2.aliveArr[0]) trans2CamLine(&clinemgr_OB1, &CameraLine3D_CAM2[CURR + 0*3], imgResCr_CAM2[CURR], imgResCb_CAM2[CURR], imgResY_CAM2[CURR]);
		if (as_cam2.aliveArr[1]) trans2CamLine(&clinemgr_OB2, &CameraLine3D_CAM2[CURR + 1*3], imgResCr_CAM2[CURR], imgResCb_CAM2[CURR], imgResY_CAM2[CURR]);
		if (as_cam2.aliveArr[2]) trans2CamLine(&clinemgr_OB3, &CameraLine3D_CAM2[CURR + 2*3], imgResCr_CAM2[CURR], imgResCb_CAM2[CURR], imgResY_CAM2[CURR]);
		if (as_cam2.aliveArr[3]) trans2CamLine(&clinemgr_OB4, &CameraLine3D_CAM2[CURR + 3*3], imgResCr_CAM2[CURR], imgResCb_CAM2[CURR], imgResY_CAM2[CURR]);
		if (as_cam2.aliveArr[4]) trans2CamLine(&clinemgr_OB5, &CameraLine3D_CAM2[CURR + 4*3], imgResCr_CAM2[CURR], imgResCb_CAM2[CURR], imgResY_CAM2[CURR]);

		cvMerge(imgResY_CAM2[CURR], imgResCr_CAM2[CURR], imgResCb_CAM2[CURR], NULL, imgResYCrCb_CAM2[CURR]);
		cvCvtColor(imgResYCrCb_CAM2[CURR], imgRes_CAM2[CURR], CV_YCrCb2BGR);
		cvCvtColor(imgRes_CAM2[CURR], imgResHSV_CAM2[CURR], CV_BGR2HSV);
		cvSplit(imgResHSV_CAM2[CURR], imgResH_CAM2[CURR], imgResS_CAM2[CURR], imgResV_CAM2[CURR], NULL);
		Cam2DispReady = true;
		cvWaitKey(20);
		 
		Alive_CAM2[PREV] = Alive_CAM2[LAST];
		Alive_CAM2[LAST] = Alive_CAM2[CURR];
		Alive_CAM2[CURR] = as_cam2;
		for (int h=0; h<MAX_TARGET_NUM; h++)
		{
			CurrAliveList_CAM2[h] = as_cam2.aliveArr[h];
		}
		

	}
	return 0;
}

void SolveObservationIntegration_ConfidenceTable()
{
	Position3D intersect, line1pt, line2pt, headpt1, headpt2;
	int intersect_Z = 0;
	char number[20];
	int matchCnt, best_color_index;
	double color_log_score, best_color_log_dist;
	int Lv;

	for (int g=0; g<MAX_TARGET_NUM; g++)
	{
		if ((Alive_CAM1[CURR].aliveArr[g] == PrevAliveList_CAM1[g]) && Alive_CAM1[CURR].aliveArr[g])  //stay alive
		{
			body_CAM1[g].LocateAndMeasureInfoH(&CameraLine3D_CAM1[g], targetGndLine_CAM1[g], true, targetHeight_CAM1[g], imgResH_CAM1[CURR], imgResS_CAM1[CURR], hist_mask);
		}
		else if ((Alive_CAM1[CURR].aliveArr[g] == PrevAliveList_CAM1[g])) //stay dead
		{
			targetHeight_CAM1[g] = DEFAULT_H;
			; //do nothing 
		}
		else if (Alive_CAM1[CURR].aliveArr[g]) //new observation
		{
			body_CAM1[g].LocateAndMeasureInfoH(&CameraLine3D_CAM1[g], targetGndLine_CAM1[g], true, targetHeight_CAM1[g], imgResH_CAM1[CURR], imgResS_CAM1[CURR], hist_mask);
			observationShowIndep(objConfMap, CAM1, g, CurrAliveList_CAM1, CurrAliveList_CAM2);
			cout << g << " ShowIndep in CAM1" << endl;
		}
		else //observation disappear
		{
			targetHeight_CAM1[g] = DEFAULT_H;
			observationDisappear(objConfMap, CAM1, g, CurrAliveList_CAM1, CurrAliveList_CAM2);
			cout << g << " disappear in CAM1" << endl;
		}



		if ((Alive_CAM2[CURR].aliveArr[g] == PrevAliveList_CAM2[g]) && Alive_CAM2[CURR].aliveArr[g])  //stay alive
		{
			body_CAM2[g].LocateAndMeasureInfoH(&CameraLine3D_CAM2[g], targetGndLine_CAM2[g], true, targetHeight_CAM2[g], imgResH_CAM2[CURR], imgResS_CAM2[CURR], hist_mask);
		}
		else if ((Alive_CAM2[CURR].aliveArr[g] == PrevAliveList_CAM2[g])) //stay dead
		{
			targetHeight_CAM2[g] = DEFAULT_H;
			; //do nothing 
		}
		else if (Alive_CAM2[CURR].aliveArr[g]) //new observation
		{
			body_CAM2[g].LocateAndMeasureInfoH(&CameraLine3D_CAM2[g], targetGndLine_CAM2[g], true, targetHeight_CAM2[g], imgResH_CAM2[CURR], imgResS_CAM2[CURR], hist_mask);
			observationShowIndep(objConfMap, CAM2, g, CurrAliveList_CAM1, CurrAliveList_CAM2);
			cout << g << " ShowIndep in CAM2" << endl;
		}
		else //observation disappear
		{
			targetHeight_CAM2[g] = DEFAULT_H;
			observationDisappear(objConfMap, CAM2, g, CurrAliveList_CAM1, CurrAliveList_CAM2);
			cout << g << " disappear in CAM2" << endl;
		}

	}
	//update PrevAliveList
	for (int e=0; e<MAX_TARGET_NUM; e++)
	{
		PrevAliveList_CAM1[e] = CurrAliveList_CAM1[e];
		PrevAliveList_CAM2[e] = CurrAliveList_CAM2[e];
		PrevAliveList_CAM3[e] = CurrAliveList_CAM3[e];
	}




	bool IsMatched = false;
	for (int g=0; g<MAX_TARGET_NUM; g++)
	{
		if (!Alive_CAM1[CURR].aliveArr[g]) 
		{
			//occupiedList_cam1[g] = false; //set available
			continue;
		}

		for (int f=0; f<MAX_TARGET_NUM; f++)
		{
			if (!Alive_CAM2[CURR].aliveArr[f]) 
			{
				//occupiedList_cam2[f] = false; //set available
				continue;
			}

			//if (occupiedList_cam1[g] || occupiedList_cam2[f]) continue;

			if (CheckLineIntersect(&CameraLine3D_CAM1[CURR + g*3], &CameraLine3D_CAM2[CURR + f*3], intersect, line1pt, line2pt))
			{
				if (intersect.z < 125.0)
				{
					geoMatchCnt[g][f] = 0;
					continue;
				}

				intersect_Z = intersect.z;
				bool show3DPos = CameraLine3D_CAM1[CURR + g*3].getPosFromZ(intersect_Z, headpt1);
				show3DPos = show3DPos && CameraLine3D_CAM2[CURR + f*3].getPosFromZ(intersect_Z, headpt2);

				if (!show3DPos || MAGNITUDE((headpt1 - headpt2)) > 40) 
				{
					geoMatchCnt[g][f] = 0;
					continue;
				}
				//now it's geometrically acceptable, record h as height
				geoMatchCnt[g][f]++;

				body_CAM1[g].LocateAndMeasureInfoH(&CameraLine3D_CAM1[CURR + g*3], targetGndLine_CAM1[g], true, intersect_Z/*+10-10*/, imgResH_CAM1[CURR], imgResS_CAM1[CURR], hist_mask);
				body_CAM2[f].LocateAndMeasureInfoH(&CameraLine3D_CAM2[CURR + f*3], targetGndLine_CAM2[f], true, intersect_Z/*+10+5*/, imgResH_CAM2[CURR], imgResS_CAM2[CURR], hist_mask);

				matchCnt = NONDEFINED;
				color_log_score = NONDEFINED;
				Lv = 0;
				color_log_score = AppearanceComparison_powWeight_H(&body_CAM1[g], &body_CAM2[f], matchCnt, Lv, best_color_log_dist, best_color_index);

				//cout << "color comparison: " << int(100.0*color_score) << "\n ";
				//std::stringstream score_ss;
				//score_ss << int(100.0*color_score);
				//std::string outSig = "color comparison: " + score_ss.str() + "\n ";

				int totalLikelihood;
				if (totalLikelihood = correspondenceLikelihoodThresholding_powWeight(g,f, intersect_Z, headpt1, headpt2, color_log_score, matchCnt, best_color_index, best_color_log_dist, geoMatchCnt[g][f], Lv))
				{
					matchCntMap[g][f] = matchCnt;
					colorScoreMap[g][f] = color_log_score;
					objExistCnt[g][f]++;
					//objConfMap[g][f] = Lv;
					observationIntegrate(objConfMap, g, f, Lv, CurrAliveList_CAM1, CurrAliveList_CAM2);
					//cout << "integrated with Lv " << Lv << endl;;
					
					//simplest method: accept that they are the same person now.
					targetHeight_CAM1[g] = intersect_Z/* + 10 - 10*/;
					targetPos_CAM1[g] = headpt1;
					targetHeight_CAM2[f] = intersect_Z/* + 10 + 5*/;
					targetPos_CAM2[f] = headpt2;

					////////////if (sampleNum_H == 0) //first time
					////////////{
					////////////	sampleNum_H++;
					////////////	mean_H = (double)intersect_Z/100.0;
					////////////	stdDev_H = 0;
					////////////	sampleNum_D++;
					////////////	mean_D = MAGNITUDE((line1pt-line2pt))/100.0;
					////////////	stdDev_D = 0;
					////////////}
					////////////else //not first
					////////////{
					////////////	calcMeanStdDev((double)intersect_Z/100.0, sampleNum_H, mean_H, stdDev_H, mean_H, stdDev_H);
					////////////	calcMeanStdDev(MAGNITUDE((line1pt-line2pt))/100.0, sampleNum_D, mean_D, stdDev_D, mean_D, stdDev_D); 
					////////////}
					////////////cout << "Height: sampleNum = " << sampleNum_H << ", stdDev = " << stdDev_H << ", mean = " << mean_H << endl;
					////////////cout << "Distance: sampleNum = " << sampleNum_D << ", stdDev = " << stdDev_D << ", mean = " << mean_D << endl;

					//now we got headline, gndline, targetheight, targetPos(3D), and have measured and comparison histograms by blocks
					//DrawIntegratedObservation(CAM1, g, imgResCr_CAM1[CURR], imgResCb_CAM1[CURR], Lv);
					//DrawIntegratedObservation(CAM2, f, imgResCr_CAM2[CURR], imgResCb_CAM2[CURR], Lv);
					
					//cout << " matched!\n";
					//outSig += "matched!\n";
					//stringstream h_s;
					//cout << "height = " << ((intersect_Z < 0) ? 0 : (intersect_Z > 200) ? 200 : (int)intersect_Z) << ", ";
					//h_s << ((intersect_Z < 0) ? 0 : (intersect_Z > 200) ? 200 : (int)intersect_Z);
					//(mSingletonInstancePtr->mControlMessage) += (outSig + "height = " + h_s.str() + ", ");

					//cout << "totalLikelihood = " << (int)totalLikelihood << " .\n";
					//std::stringstream tscore;
					//tscore << ((int)total);
					//(mSingletonInstancePtr->mControlMessage) += ("total = " + tscore.str() + " .\n");

					IsMatched = true;


				}
				else continue;


			}
			else geoMatchCnt[g][f] = 0;

		}
	}
	if (IsMatched == false) 
	{
		//std::string outSig = "not matched\n";
		//(mSingletonInstancePtr->mControlMessage) += outSig;
	}
	for (int c=0; c<MAX_TARGET_NUM; c++)
	{
		for (int b=0; b<MAX_TARGET_NUM; b++)
		{
			map<Pair, int>::iterator iter;
			iter = targetData.objTarMapping.find(makePair(c, b, NULL, NULL));
			if ((objConfMap[c][b] <= 0) && (iter != targetData.objTarMapping.end()))
			{
				targetData.labelAvail[iter->second] = 1;
				objExistCnt[c][b] = 0;
				targetData.objTarMapping.erase(iter);
				//integratedResult.Alive[iter->second] = -1;
				integratedResult.CAM1_id[c] = -1;
				integratedResult.CAM2_id[b] = -1;
			}

			if (objConfMap[c][b] <= 0/*!= OBJ_HIGH_CONF*/) 
			{
				objExistCnt[c][b] = 0;
				continue;
			}
			//if (occupiedList_cam1[c] == false || occupiedList_cam2[b] == false)
			//{
			//	cout << "destroy Integration" << endl;
			//	objConfMap[c][b] = 0;
			//	occupiedList_cam1[c] = occupiedList_cam2[b] = false;
			//	continue;
			//}
			//cout << "DrawIntegrated by memory" << endl;
			DrawIntegratedObservation(CAM1, c, imgResH_CAM1[CURR], imgResS_CAM1[CURR], objConfMap[c][b]);
			DrawIntegratedObservation(CAM2, b, imgResH_CAM2[CURR], imgResS_CAM2[CURR], objConfMap[c][b]);
			
			if (objExistCnt[c][b] < 25/*50*/) continue;
			
			int matchingLv;
			int label = targetData.matchObject_powWeight(&body_CAM1[c], &body_CAM2[b], objConfMap[c][b], matchingLv, c, b, targetHeight_CAM1[c]);
			if (objExistCnt[c][b] < 50) targetData.updateCreateDatabase(label, &body_CAM1[c], &body_CAM2[b], c, b, targetHeight_CAM1[c]);

			if (label >= 0) 
			{
				integratedResult.Alive[label] =  objConfMap[c][b]*1000.0 + targetData.targetHeight[label]/*targetHeight_CAM1[c]*/;
				integratedResult.CAM1_id[c] = label + 0.1234;
				integratedResult.CAM2_id[b] = label + 0.1234;
			}

			//cout << "targetNum = " << targetData.getTargetNum() << endl;
			//cout << "label + 0.1234 = " << label + 0.1234 << endl;
			//cout << "objConfMap[c][b]*1000.0 + targetHeight_CAM1[c] = " << objConfMap[c][b]*1000.0 + targetHeight_CAM1[c] << endl;

		}
	}


}

void DrawIntegratedObservation(int CAMID, int tar_id, IplImage* image_Cr, IplImage* image_Cb, int Lv)
{
	char LvChar;
	if (Lv == 4) LvChar = 'S';
	else if (Lv == 3) LvChar = 'H';
	else if (Lv == 2) LvChar = 'M';
	else if (Lv == 1) LvChar = 'L';
	else 
		LvChar = 'N';

	bool show3DPos;
	if (CAMID == CAM1)
	{
		show3DPos = CameraLine3D_CAM1[tar_id].getPosFromZ(targetHeight_CAM1[tar_id], targetPos_CAM1[tar_id]);
		body_CAM1[tar_id].LocateAndMeasureInfo(&CameraLine3D_CAM1[tar_id], targetGndLine_CAM1[tar_id], true, targetHeight_CAM1[tar_id], image_Cr, image_Cb, hist_mask);
	}
	if (CAMID == CAM2)
	{
		show3DPos = CameraLine3D_CAM1[tar_id].getPosFromZ(targetHeight_CAM1[tar_id], targetPos_CAM1[tar_id]);
		body_CAM1[tar_id].LocateAndMeasureInfo(&CameraLine3D_CAM1[tar_id], targetGndLine_CAM1[tar_id], true, targetHeight_CAM1[tar_id], image_Cr, image_Cb, hist_mask);
	}


	if (CAMID == CAM1)
	{
		char number[20];
		sprintf(number, "%c", LvChar);
		sprintf(number+1, "(%d,%d,%d)", (int)targetPos_CAM1[tar_id].x, (int)targetPos_CAM1[tar_id].y, (int)targetPos_CAM1[tar_id].z);
		//fileOut2 << "Integrate CAM1 = " << cam_id << endl;
		cvPutText(image_Cr, number, cvPoint(CameraLine3D_CAM1[CURR + tar_id*3].imageLoc.x - 20, CameraLine3D_CAM1[CURR + tar_id*3].imageLoc.y - 40), &orderFont, cvScalar(255));
		cvPutText(image_Cb, number, cvPoint(CameraLine3D_CAM1[CURR + tar_id*3].imageLoc.x - 20, CameraLine3D_CAM1[CURR + tar_id*3].imageLoc.y - 40), &orderFont, cvScalar(255));

		//else //cam_id == CAM2
		//{
		//	//fileOut2 << "Integrate CAM2 = " << cam_id << endl;
		//	cvPutText(image, number, cvPoint(targetList[tar_id].tgMAP.x - 20, targetList[tar_id].tgMAP.y + 50/*- 40*/ //This is for CAM2 setting
		//		), &orderFont, CV_RGB(0,250,250));
		//}
		sprintf(number, "(%d,%d)", targetGndLine_CAM1[tar_id].imageLoc.x, targetGndLine_CAM1[tar_id].imageLoc.y);
		cvPutText(image_Cr, number, cvPoint(CameraLine3D_CAM1[CURR + tar_id*3].imageLoc.x - 10, CameraLine3D_CAM1[CURR + tar_id*3].imageLoc.y + 40), &orderFont, cvScalar(255));
		cvPutText(image_Cb, number, cvPoint(CameraLine3D_CAM1[CURR + tar_id*3].imageLoc.x - 10, CameraLine3D_CAM1[CURR + tar_id*3].imageLoc.y + 40), &orderFont, cvScalar(255));

		for (int f=0; f<NUM_OF_BLOCKS; f++)
		{
			if (!body_CAM1[tar_id].IsBodySeen[f]) continue;
		
			cvRectangle(image_Cr, cvPoint(body_CAM1[tar_id].Blocks[f].x, body_CAM1[tar_id].Blocks[f].y), 
				cvPoint(body_CAM1[tar_id].Blocks[f].x+body_CAM1[tar_id].Blocks[f].width, body_CAM1[tar_id].Blocks[f].y+body_CAM1[tar_id].Blocks[f].height),
				cvScalar(200), 2);
			cvRectangle(image_Cb, cvPoint(body_CAM1[tar_id].Blocks[f].x, body_CAM1[tar_id].Blocks[f].y), 
				cvPoint(body_CAM1[tar_id].Blocks[f].x+body_CAM1[tar_id].Blocks[f].width, body_CAM1[tar_id].Blocks[f].y+body_CAM1[tar_id].Blocks[f].height),
				cvScalar(200), 2);
		}	
	}
	if (CAMID == CAM2)
	{
		char number[20];
		sprintf(number, "%c", LvChar);
		sprintf(number+1, "(%d,%d,%d)", (int)targetPos_CAM2[tar_id].x, (int)targetPos_CAM2[tar_id].y, (int)targetPos_CAM2[tar_id].z);
		//fileOut2 << "Integrate CAM2 = " << cam_id << endl;
		cvPutText(image_Cr, number, cvPoint(CameraLine3D_CAM2[CURR + tar_id*3].imageLoc.x - 20, CameraLine3D_CAM2[CURR + tar_id*3].imageLoc.y + 50), &orderFont, cvScalar(255));
		cvPutText(image_Cb, number, cvPoint(CameraLine3D_CAM2[CURR + tar_id*3].imageLoc.x - 20, CameraLine3D_CAM2[CURR + tar_id*3].imageLoc.y + 50), &orderFont, cvScalar(255));

		sprintf(number, "(%d,%d)", targetGndLine_CAM2[tar_id].imageLoc.x, targetGndLine_CAM2[tar_id].imageLoc.y);
		cvPutText(image_Cr, number, cvPoint(CameraLine3D_CAM2[CURR + tar_id*3].imageLoc.x - 10, CameraLine3D_CAM2[CURR + tar_id*3].imageLoc.y + 40), &orderFont, cvScalar(255));
		cvPutText(image_Cb, number, cvPoint(CameraLine3D_CAM2[CURR + tar_id*3].imageLoc.x - 10, CameraLine3D_CAM2[CURR + tar_id*3].imageLoc.y + 40), &orderFont, cvScalar(255));

		for (int f=0; f<NUM_OF_BLOCKS; f++)
		{
			if (!body_CAM2[tar_id].IsBodySeen[f]) continue;
		
			cvRectangle(image_Cr, cvPoint(body_CAM2[tar_id].Blocks[f].x, body_CAM2[tar_id].Blocks[f].y), 
				cvPoint(body_CAM2[tar_id].Blocks[f].x+body_CAM2[tar_id].Blocks[f].width, body_CAM2[tar_id].Blocks[f].y+body_CAM2[tar_id].Blocks[f].height),
				cvScalar(200), 2);
			cvRectangle(image_Cb, cvPoint(body_CAM2[tar_id].Blocks[f].x, body_CAM2[tar_id].Blocks[f].y), 
				cvPoint(body_CAM2[tar_id].Blocks[f].x+body_CAM2[tar_id].Blocks[f].width, body_CAM2[tar_id].Blocks[f].y+body_CAM2[tar_id].Blocks[f].height),
				cvScalar(200), 2);
		}	
	}
	if (CAMID == CAM3)
	{
		char number[20];
		sprintf(number, "%c", LvChar);
		sprintf(number+1, "(%d,%d,%d)", (int)targetPos_CAM3[tar_id].x, (int)targetPos_CAM3[tar_id].y, (int)targetPos_CAM3[tar_id].z);
		//fileOut2 << "Integrate CAM3 = " << cam_id << endl;
		cvPutText(image_Cr, number, cvPoint(CameraLine3D_CAM3[CURR + tar_id*3].imageLoc.x - 20, CameraLine3D_CAM3[CURR + tar_id*3].imageLoc.y - 40), &orderFont, cvScalar(255));
		cvPutText(image_Cb, number, cvPoint(CameraLine3D_CAM3[CURR + tar_id*3].imageLoc.x - 20, CameraLine3D_CAM3[CURR + tar_id*3].imageLoc.y - 40), &orderFont, cvScalar(255));

		sprintf(number, "(%d,%d)", targetGndLine_CAM3[tar_id].imageLoc.x, targetGndLine_CAM3[tar_id].imageLoc.y);
		cvPutText(image_Cr, number, cvPoint(CameraLine3D_CAM3[CURR + tar_id*3].imageLoc.x - 10, CameraLine3D_CAM3[CURR + tar_id*3].imageLoc.y + 40), &orderFont, cvScalar(255));
		cvPutText(image_Cb, number, cvPoint(CameraLine3D_CAM3[CURR + tar_id*3].imageLoc.x - 10, CameraLine3D_CAM3[CURR + tar_id*3].imageLoc.y + 40), &orderFont, cvScalar(255));

		for (int f=0; f<NUM_OF_BLOCKS; f++)
		{
			if (!body_CAM3[tar_id].IsBodySeen[f]) continue;
		
			cvRectangle(image_Cr, cvPoint(body_CAM3[tar_id].Blocks[f].x, body_CAM3[tar_id].Blocks[f].y), 
				cvPoint(body_CAM3[tar_id].Blocks[f].x+body_CAM3[tar_id].Blocks[f].width, body_CAM3[tar_id].Blocks[f].y+body_CAM3[tar_id].Blocks[f].height),
				cvScalar(200), 2);
			cvRectangle(image_Cb, cvPoint(body_CAM3[tar_id].Blocks[f].x, body_CAM3[tar_id].Blocks[f].y), 
				cvPoint(body_CAM3[tar_id].Blocks[f].x+body_CAM3[tar_id].Blocks[f].width, body_CAM3[tar_id].Blocks[f].y+body_CAM3[tar_id].Blocks[f].height),
				cvScalar(200), 2);
		}	
	}

}
int correspondenceLikelihoodThresholding(int g, int f, int intersect_Z, Position3D headpt1, Position3D headpt2, double color_score, int matchCnt, int best_color_index, double best_color_dist, int CtsGeoMatch, int & Lv)
{
	int total;
	total = ((abs(intersect_Z - 155) > 20) ? 0 : 5);
	total += ((MAGNITUDE((headpt1 - headpt2)) > 20) ? 0 : 3);
	total += ((color_score > 0.3) ? 0 : 4);
	//total += ((matchCnt < 2) ? 0 : 2);
	total += ((best_color_index <= 1) ? 2 : 0);
	total += ((best_color_dist > 0.2) ? 0 : 3);
	total += ((best_color_dist < 0.2) || (color_score < 0.3) || (MAGNITUDE((headpt1 - headpt2)) < 20)) ? 5 : 0;
	total += (CtsGeoMatch/10);


	if (total >= 16/*18*/) 
	{
//		objConfMap[g][f] = Lv;// = OBJ_HIGH_CONF;
//		occupiedList_cam1[g] = occupiedList_cam2[f] = true;
	}
	
	return (total >= 12/*14*/) ? total : 0;
}

int correspondenceLikelihoodThresholding_powWeight(int g, int f, int intersect_Z, Position3D headpt1, Position3D headpt2, double color_log_score, int matchCnt, int best_color_index, double best_color_log_dist, int CtsGeoMatch, int & Lv)
{
	int total;
	total = ((abs(intersect_Z - 155) > 20) ? 0 : 5);
	total += ((MAGNITUDE((headpt1 - headpt2)) > 25) ? 0 : 3);
	total += ((color_log_score > log(0.35)) ? 0 : 4);
	//total += ((matchCnt < 2) ? 0 : 2);
	total += ((best_color_index <= 1) ? 2 : 0);
	total += ((best_color_log_dist > log(0.25)) ? 0 : 3);
	total += ((best_color_log_dist < log(0.25)) || (color_log_score < log(0.35)) || (MAGNITUDE((headpt1 - headpt2)) < 25)) ? 5 : 0;
	total += ((CtsGeoMatch/10) >= 4) ? 4 : 0;

	if (total >= 16/*18*/) 
	{
//		objConfMap[g][f] = Lv;// = OBJ_HIGH_CONF;
//		occupiedList_cam1[g] = occupiedList_cam2[f] = true;
	}
	
	return (total >= 12/*14*/) ? total : 0;
}

void SendIntegrateResult()
{
	sendIntResCnt++;
	if (!(sendIntResCnt%30)) setLabelIntegrate(integratedResult);//all cameras share this status:  LabelIntegrateMgr_CAM1
}
void showImage()
{
	//SolveObservationIntegration_greedy();
	SolveObservationIntegration_ConfidenceTable();
	targetData.showDatabaseDetail();
	SendIntegrateResult();

	cvShowImage("imgResH_CAM1[CURR]", imgResH_CAM1[CURR]);
	cvShowImage("imgResS_CAM1[CURR]", imgResS_CAM1[CURR]);
	cvShowImage("imgRes_CAM1[CURR]", imgRes_CAM1[CURR]);
	char number[20];

	shortCnt++;
	if (shortCnt%15 == 0)
	{
		GMMCnt++;
		shortCnt = 0;
		if (targetData.targetNum >=1 )
		{
			sprintf(number, "target0GMM%d.jpg", GMMCnt);
			//cvSaveImage(number, targetData.candidateDemo_Large[0]);
		}
		if (targetData.targetNum >=2 )		
		{
			sprintf(number, "target1GMM%d.jpg", GMMCnt);
			//cvSaveImage(number, targetData.candidateDemo_Large[1]);
		}
		if (targetData.targetNum >=3 )
		{
			sprintf(number, "target2GMM%d.jpg", GMMCnt);
			//cvSaveImage(number, targetData.candidateDemo_Large[2]);
		}

	}

	cvShowImage("imgResH_CAM2[CURR]", imgResH_CAM2[CURR]);
	cvShowImage("imgResS_CAM2[CURR]", imgResS_CAM2[CURR]);
	cvShowImage("imgRes_CAM2[CURR]", imgRes_CAM2[CURR]);

	//cvShowImage("imgResCr_CAM1[LAST]", imgResCr_CAM1[LAST]);
	//cvShowImage("imgResCb_CAM1[LAST]", imgResCb_CAM1[LAST]);
	//cvShowImage("imgResCr_CAM2[LAST]", imgResCr_CAM2[LAST]);
	//cvShowImage("imgResCb_CAM2[LAST]", imgResCb_CAM2[LAST]);
	//cvShowImage("imgResCr_CAM1[PREV]", imgResCr_CAM1[PREV]);
	//cvShowImage("imgResCb_CAM1[PREV]", imgResCb_CAM1[PREV]);
	//cvShowImage("imgResCr_CAM2[PREV]", imgResCr_CAM2[PREV]);
	//cvShowImage("imgResCb_CAM2[PREV]", imgResCb_CAM2[PREV]);

}





//void globaladdThread( void ) 
//{
//    if (recTarID != NONDEFINED)
//	{
//		globalThreadNr = recTarID;
//		globalhThread[0] = (HANDLE)_beginthreadex(NULL, 0, recordGMMVideoThread, &globalThreadNr, 0, NULL );
//	}
//}
//
//unsigned recordGMMVideoThread( void *pMyID )
//{
//	char    *MyID = (char*)pMyID;
//	int id=(int)*MyID;
//
//	char number[20];
//	sprintf(number, "GMMVideoTarget%d.avi", id);
//	GMMTargetVideo[id] = cvCreateVideoWriter(number,-1,REC_SAMPLINGRATE,cvSize((30 + 45*5)*2, (30 + 45*4 + 20)*2),1);
//
//
//	while (!stopGMMVideo)
//	{
//		cvWaitKey(1000/REC_SAMPLINGRATE - 10);
//		cvWriteFrame(GMMTargetVideo[id], targetData.candidateDemo_Large[id]);
//
//	}
//
//	cvReleaseVideoWriter(&GMMTargetVideo[id]);
//
//	return 0;
//}