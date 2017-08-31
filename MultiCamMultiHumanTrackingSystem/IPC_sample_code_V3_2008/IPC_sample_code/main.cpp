
#include "Client.hpp"
#include <iostream>
#include "process.h"

using namespace std;

unsigned WINAPI sendThread(LPVOID);
unsigned WINAPI getThread(LPVOID);

unsigned WINAPI sendThread2(LPVOID);
unsigned WINAPI getThread2(LPVOID);

IplImage *sample_img, *sample_YCrCb, *sample_Y, *sample_Cr, *sample_Cb, *histimg_Cr, *histimg_Cb;
CvHistogram *refHist, *getHist;


void prepare();
void showImage();

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
	HANDLE hthreads[2];
	hthreads[0] = (HANDLE)_beginthreadex( NULL, 0, sendThread2, NULL, 0, &threadID );
	hthreads[1] = (HANDLE)_beginthreadex( NULL, 0, getThread2, NULL, 0, &threadID );

	while( cvWaitKey(30) != 27)
		showImage();

	WaitForMultipleObjects( 2, hthreads, TRUE, INFINITE );
	CloseHandle( hthreads[0] );
	CloseHandle( hthreads[1] );

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
		
	cvNamedWindow("Transfered Cr Hist", 1);
	cvNamedWindow("Transfered Cb Hist", 1);
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
	CameraLine3D line;
	line.camID = CAM1;


	bi.height = 10;
	line.cm2pixelRatio = 10.2;
	//while(1){
	for(int i = 0 ; i < 100 ; ++i){
		
		bi.height ++ ;
		line.cm2pixelRatio ++;
		trans2BodyInfoMgr(&bi, &send_data, 100);
		trans2CamLineMgr(&line, &linemgr, 150);
		setBodyInfo( send_data );
		setCamLine( linemgr);
		setHistCrCb( shist );
		cout << "SET HEIGHT : " << send_data.height << endl;
		cout << "SET RATIO : " << linemgr.cm2pixelRatio << endl;
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
	CameraLine3D line;

	//while(1){
	for(int i = 0 ; i < 100 ; ++i){
		
		getBodyInfo( get_data );
		getCamLine( linemgr);
		getHistCrCb( ghist );
		trans2BodyInfo(&get_data, &bi);
		trans2CamLine(&linemgr, &line);
		trans2HistCrCb(&ghist, getHist);
		cout << "							GET HEIGHT : " << bi.height << endl;
		cout << "							GET RATIO : " << line.cm2pixelRatio << endl;

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
}