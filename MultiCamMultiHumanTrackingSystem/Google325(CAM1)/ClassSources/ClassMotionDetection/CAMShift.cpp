#include "CAMShift.h"
#include <cmath>
#include <sstream>
#include "C_WinSocket.h"
//#include "C_PM.h"
//#include "VisionTranslate.h"

CAMShift* CAMShift::mSingletonInstancePtr = NULL;
const int CAMShift::MAX_LOSING_COUNT = 5;
const int CAMShift::MAX_THRESHOLD = 250;
const int CAMShift::MIN_THRESHOLD = 30;
const int CAMShift::STANDARD_THRESHOLD = 3600;
const int CAMShift::NOM_FOR_INIT_STATE_USING_CAMSHIFT = 5;

//.....Thread parameters......
int testCnt=0;
HANDLE hThread1;
HANDLE hThread2;
unsigned int threadID1 = 0;
unsigned int threadID2 = 0;
bool flag1 = false;
bool flag2 = false;
unsigned int ThreadNr1 = 0;
unsigned int ThreadNr2 = 0;
//.....Thread parameters......

//#include "efficientSeg\stdafx.h"
#include <time.h>
#include <cassert>
//#include "efficientSeg\image.h"
//#include "efficientSeg\misc.h"
//#include "efficientSeg\segment-image.h"

#pragma comment(lib, "cxcore.lib");
#pragma comment(lib, "cv.lib");
#pragma comment(lib, "highgui.lib");

IplImage* color_img0, *color_img01;
IplImage* color_img, *color_img1;
IplImage* gray_img0 = NULL;
IplImage* gray_img = NULL;
clock_t start, finish;
double duration;
//fstream CAMShift::fileOut2;
IplImage* sad = cvCreateImage(cvSize(TEMPLATE_WIDTH,TEMPLATE_HEIGHT),8,1);
bool orderFix;


//OK version with WaitForSingleObject
unsigned int parallelTracking( void *pMyID )
{
	unsigned int * MyID = (unsigned int*)pMyID;
	unsigned int id=(unsigned int)*MyID;
	if ( MyID == &ThreadNr1)
	{ 
		{
			testCnt++;
		}
		_endthread();
		return 0;
	}

	else if ( MyID == &ThreadNr2)
	{
		WaitForSingleObject(hThread1,INFINITE);
		if (testCnt == 1)  testCnt++;
		_endthread();
		return 0;
	}
	testCnt = 100;

	_endthread();
	return *MyID;
}

void prtPline(int r, int c, double angle, IplImage* img)
{
	double m = tan(angle);
	double b = ((double)c)-m*((double)r);

	//plot the line by scaning through rows
	for (int x=0; x<img->height; x++)
	{
		if (m*((double)x)+b < img->width && m*((double)x)+b >= 0)
			CV_IMAGE_ELEM(img, unsigned char, x, int(m*((double)x)+b)) = 0 ;
	}
	
	//plot the line by scaning through columns, to avoid dot-lines with steep slopes
	for (int y=0; y<img->width; y++)
	{
		if (((double)y-b)/m < img->height && ((double)y-b)/m >= 0)
			CV_IMAGE_ELEM(img, unsigned char, int(((double)y-b)/m), y) = 0 ;
	}

}


CAMShift::CAMShift( /*C_PM* cPM,*/ int iWidth, int iHeight, int cam_index ) /*:
	mPM(cPM)*/
{
	cam_id = cam_index;

	mapX = 0.0;
	mapY = 0.0;
	mapR = 0.0;
	mapTheta = 0.0;
	
	width = iWidth;
	height = iHeight;
	hdims = 16;
	mLosingCount = 0;
	mIterationCount = 0;
	mFlagIteration[0] = 0;
	mFlagIteration[1] = 4;
	mFlagIteration[2] = 8;
	mFlagIteration[3] = 12;
	mFlagIteration[4] = 16;
	mFlagIteration[5] = 20;
	//mStartTimeStamp = 0;
	//mEndTimeStamp = 0;
	float hranges_arr[] = {0,180};
    float* hranges = hranges_arr;
	vmin = 50;  vmax = 240;    smin = 60; 

	hsv = cvCreateImage(cvSize(width,height),8,3);
	hue = cvCreateImage(cvSize(width,height),8,1);
	sat = cvCreateImage(cvSize(width,height),8,1);
	val = cvCreateImage(cvSize(width,height),8,1);

	image_YCrCb = cvCreateImage(cvSize(width,height),8,3);
	image_Y = cvCreateImage(cvSize(width,height),8,1);
	image_Cr = cvCreateImage(cvSize(width,height),8,1);
	image_Cb = cvCreateImage(cvSize(width,height),8,1);
	
	//image variable for SobelEdge
	gray = cvCreateImage(cvSize(width,height),8,1);
	p_gray = cvCreateImage(cvSize(width,height),8,1);
	prev_gray = cvCreateImage(cvSize(width/2,height/2),8,1);
	moving_blob = cvCreateImage(cvSize(width,height),8,1);
	prev_moving_blob = cvCreateImage(cvSize(width/2,height/2),8,1);

	sobel = cvCreateImage(cvSize(width,height),8,1);
	temp_sobel = cvCreateImage(cvSize(width,height),8,1);
	graysmooth = cvCreateImage( cvSize(width,height), IPL_DEPTH_8U, 1 );
	edgetemp = cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );
	dx = cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );
	dy = cvCreateImage( cvSize(width,height), IPL_DEPTH_32F, 1 );

	crtMask = cvCreateImage(cvSize(width,height),8,1);
	mask = cvCreateImage( cvSize(width,height), 8, 1 );
	extend_mask = cvCreateImage( cvSize(width,height), 8, 1 );
	safeMask = cvCreateImage( cvSize(width,height), 8, 1 );
	cvZero(safeMask);
	cvRectangle(safeMask, cvPoint(of_x,of_y), cvPoint(width-of_x, height-of_y), cvScalar(255), -1);
	ROImask = cvCreateImage( cvSize(width,height), 8, 1 );
	cvZero(ROImask);
	edgeResponse = cvCreateImage( cvSize(width,height), 8, 1 );

    backproject = cvCreateImage( cvSize(width,height), 8, 1 );

	half_backproject = cvCreateImage( cvSize(width,height), 8, 1 );

	targetSat=0;

	ck = cvCreateStructuringElementEx( /*11*/17, /*11*/17, 8, 8, CV_SHAPE_ELLIPSE, NULL );
	erosion = cvCreateStructuringElementEx( /*11*/7, /*11*/7, 3, 3, CV_SHAPE_ELLIPSE, NULL );
	bigErosion = cvCreateStructuringElementEx( /*11*/9, /*11*/9, 4, 4, CV_SHAPE_ELLIPSE, NULL );
	smlErosion = cvCreateStructuringElementEx( /*11*/4, /*11*/4, 2, 2, CV_SHAPE_ELLIPSE, NULL );
	connectionElement = cvCreateStructuringElementEx( 26, 26, 13, 13, CV_SHAPE_ELLIPSE, NULL );
	CAM1_detect_dilation = cvCreateStructuringElementEx( 11, 11, 5, 5, CV_SHAPE_ELLIPSE, NULL );//CAM1 setting

	//for (int k=0; k<MAX_TARGET_NUM; k++)
	//{
	//	targetBodyHist_H[k] = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	//}
    hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	crtHist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	
	//
	float * Cr_ranges, * Cb_ranges;
	Cr_ranges = new float[2];
	Cb_ranges = new float[2];
	Cr_ranges[0] = Cb_ranges[0] = RANGE_MIN;
	Cr_ranges[1] = Cb_ranges[1] = RANGE_MAX;

	int hist_size[] = { Cr_BINS, Cb_BINS };
	float * ranges[] = { Cr_ranges, Cb_ranges };


	//for (int k=0; k<MAX_TARGET_NUM; k++)
	//{
	//	targetBodyHist_CrCb[k] = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	//}
	refHist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	refHairHist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	JointHist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	delete [] Cr_ranges;
	delete [] Cb_ranges;
	//

	histimg = cvCreateImage( cvSize(320,200), 8, 3 );
	
	for (int k=0; k<MAX_TARGET_NUM; k++)
		histimg_H[k] = cvCreateImage( cvSize(320,200), 8, 3 );

	crtHistimg = cvCreateImage( cvSize(320,200), 8, 3 );
	purifiedHistimg = cvCreateImage( cvSize(320,200), 8, 3 );
	cvZero( histimg );

	for (int k=0; k<MAX_TARGET_NUM; k++)
		cvZero(histimg_H[k]);

	cvZero( purifiedHistimg );

	mPrePosX = 0;
	mPrePosY = 0;
	mPreRot = 0.0f;
	//mPreArea = 0.0f;

	mStartFlag = true;
	noMotionCnt = 0;
	losingCnt = 0;
	maxIt = 0;
	ThreadNr = 0;

	fileOut2.open("error report2.txt",ios::out);

	//parameter setup for particle filter
	X = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	sub_X = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	X_rand = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	Y = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	sub_Y = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	Y_rand = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	R = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	sub_R = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	R_rand = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	Theta = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	sub_Theta = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	Theta_rand = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	Weight1 = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	Weight2 = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	Weight3 = cvCreateMat( 1, PARTICLE_NUM/*+ ADD_PARTICLE_NUM*/, CV_32FC1 );
	sub_Weight = cvCreateMat( 1, PARTICLE_NUM/*+ ADD_PARTICLE_NUM*/, CV_32FC1 );
	Weight1_temp = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	Weight2_temp = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	cvSet( Weight1, cvScalar(0.01) );
	cvSet( Weight2, cvScalar(0.01) );
	cvSet( Weight3, cvScalar(0.01) );
	tmp_X = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	tmp_Y = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	tmp_R = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	tmp_Theta = cvCreateMat( 1, PARTICLE_NUM, CV_32FC1 );
	tmp_add_X = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	tmp_add_Y = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	tmp_add_R = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	tmp_add_Theta = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	init_flag = true;
	iteration_cnt = 0;
	LH = 0;
	frontalFaceTest = 0;
	profileFaceTest = 0;
	hist_mask = cvCreateImage(cvSize(width,height),8,1);
	add_X = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	add_X_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	add_Y = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	add_Y_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	add_R = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	add_R_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	add_Theta = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	add_Theta_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	add_new_X = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	add_new_Y = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	add_new_R = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	add_new_Theta = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );


	if (cam_id == CAM1)
	{
		//先把sample color需要的channel建好
		sample_img = cvLoadImage("green_CAM1.bmp", 1);
		sample_hsv = cvCreateImage(cvGetSize(sample_img), 8, 3);
		sample_hue = cvCreateImage(cvGetSize(sample_img), 8, 1);
		sample_sat = cvCreateImage(cvGetSize(sample_img), 8, 1);
		sample_val = cvCreateImage(cvGetSize(sample_img), 8, 1);
		sample_YCrCb = cvCreateImage(cvGetSize(sample_img), 8, 3);
		sample_Y = cvCreateImage(cvGetSize(sample_img), 8, 1);
		sample_Cr = cvCreateImage(cvGetSize(sample_img), 8, 1);
		sample_Cb = cvCreateImage(cvGetSize(sample_img), 8, 1);

		sample_img2 = cvLoadImage("hair_CAM1.bmp", 1);
		sample_YCrCb2 = cvCreateImage(cvGetSize(sample_img2), 8, 3);
		sample_Y2 = cvCreateImage(cvGetSize(sample_img2), 8, 1);
		sample_Cr2 = cvCreateImage(cvGetSize(sample_img2), 8, 1);
		sample_Cb2 = cvCreateImage(cvGetSize(sample_img2), 8, 1);
	}
	else //cam_id == CAM2
	{
		//先把sample color需要的channel建好
		sample_img = cvLoadImage("green_CAM2.bmp", 1);
		sample_hsv = cvCreateImage(cvGetSize(sample_img), 8, 3);
		sample_hue = cvCreateImage(cvGetSize(sample_img), 8, 1);
		sample_sat = cvCreateImage(cvGetSize(sample_img), 8, 1);
		sample_val = cvCreateImage(cvGetSize(sample_img), 8, 1);
		sample_YCrCb = cvCreateImage(cvGetSize(sample_img), 8, 3);
		sample_Y = cvCreateImage(cvGetSize(sample_img), 8, 1);
		sample_Cr = cvCreateImage(cvGetSize(sample_img), 8, 1);
		sample_Cb = cvCreateImage(cvGetSize(sample_img), 8, 1);

		sample_img2 = cvLoadImage("hair_CAM2.bmp", 1);
		sample_YCrCb2 = cvCreateImage(cvGetSize(sample_img2), 8, 3);
		sample_Y2 = cvCreateImage(cvGetSize(sample_img2), 8, 1);
		sample_Cr2 = cvCreateImage(cvGetSize(sample_img2), 8, 1);
		sample_Cb2 = cvCreateImage(cvGetSize(sample_img2), 8, 1);
	}


	sml_sobel = cvCreateImage(cvSize(width/2, height/2), 8, 1);
	sml_temp = cvCreateImage(cvSize(width/2, height/2), IPL_DEPTH_32F, 1);
	s_dx = cvCreateImage(cvSize(width/2, height/2), IPL_DEPTH_32F, 1);
	s_dy = cvCreateImage(cvSize(width/2, height/2), IPL_DEPTH_32F, 1);
	s_blob = cvCreateImage(cvSize(width/2, height/2), 8, 1);
	s_mv_blob = cvCreateImage(cvSize(width/2, height/2), 8, 1);
	sml_gray = cvCreateImage(cvSize(width/2, height/2), 8, 1);
	s_temp_sobel = cvCreateImage(cvSize(width/2, height/2), 8, 1);
	sml_backproj = cvCreateImage(cvSize(width/2, height/2), 8, 1);
	sml_CntCmp = cvCreateImage(cvSize(width/2, height/2), 8, 1);
	cvZero(sml_CntCmp);

    detect_contour = 0;
	detect_storage = cvCreateMemStorage(0);
	cvInitFont( &orderFont, CV_FONT_HERSHEY_PLAIN, 0.9/*0.8*/, 0.9/*0.8*/, 0, 2/*1*/ );
	
	IPCTrackingClient = &IPCClient;
}

//
//CAMShift::CAMShift( string &presentedNormalString, string &presentedControlString )
//{
//	CAMShift();
//
//	mPresentedNormalString = presentedNormalString;
//	mPresentedControlString = presentedControlString;
//}

void CAMShift::addThread( void ) 
{
	ThreadNr += 2;
	//CreateThread(NULL, 0, parallelTracking, 
	hThread1 = (HANDLE)_beginthreadex(NULL, 0, parallelTracking, &ThreadNr1, 0, &threadID1 );
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, parallelTracking, &ThreadNr2, 0, &threadID2 );
}


CAMShift::~CAMShift()
{ 
	 cvReleaseImage( &hsv );
	 cvReleaseImage( &hue );
	 cvReleaseImage( &sat );
	 cvReleaseImage( &val );
	 cvReleaseImage( &crtMask );
	 cvReleaseImage( &mask );
	 cvReleaseImage( &backproject );
	 cvReleaseImage( &histimg );
	 cvReleaseImage( &crtHistimg );
	 cvReleaseImage( &purifiedHistimg );
	 cvReleaseImage( &half_backproject );
	 cvReleaseImage( &safeMask );
	 cvReleaseImage( &ROImask );

	 //
	 cvReleaseMat( & X_rand );
	 cvReleaseMat( & Y_rand );
	 cvReleaseMat( & R_rand );
	 cvReleaseMat( & Theta_rand );
	 if (LH != NULL)
	 {
		 delete LH;
		 LH = NULL;
	 }
	 

	 //sobel image variable
	 cvReleaseImage( & graysmooth );
	 cvReleaseImage( & dx );
	 cvReleaseImage( & dy );
	 cvReleaseImage( & edgetemp );
	 //

	 cvReleaseHist( &hist );
	 cvReleaseHist( &crtHist );

	 //cvReleaseImage( &morphImage );
	 cvReleaseStructuringElement( &ck );
	 cvReleaseStructuringElement( &erosion );
 	 cvReleaseStructuringElement( &bigErosion );
 	 cvReleaseStructuringElement( &smlErosion );
 	 cvReleaseStructuringElement( &connectionElement );

	 cvDestroyAllWindows();
}

void CAMShift::init( C_WinSocketClient *client, string *presentedNormalString, string *presentedControlString )
{
	mClient = client;

	mPresentedNormalString = presentedNormalString;
	mPresentedControlString = presentedControlString;
}

//use online image color information
bool CAMShift::initCamShiftChecking1(IplImage * image, IplImage * fgMask, CvRect target )
{	
	//resetTranslateParameter();
//	mPM->getVisionTranslate()->reset();

	cvResetImageROI( hue/*, target */);
	cvResetImageROI( mask/*, target*/ );
	cvResetImageROI( hsv/*, target */);
	cvResetImageROI( image/*, target*/ );
	cvResetImageROI( sat/*, target */);
	cvResetImageROI( val/*, target*/ );
	cvResetImageROI( fgMask/*, target*/ );
	cvResetImageROI( backproject/*, target*/);
	cvResetImageROI( edgeResponse/*, target*/);


	track_window = target;
	cvCvtColor( image, hsv, CV_BGR2HSV );
	cvInRangeS( hsv, cvScalar(0,smin,MIN(vmin,vmax),0),	cvScalar(180,256,MAX(vmin,vmax),0), mask );
	
	cvDilate( mask, mask, ck, 1 );

	cvSplit( hsv, hue, sat, val, 0 );

	
	cvAnd(sat, fgMask, sat);
	cvAnd(val, fgMask, val);
	
	long double sum=0;
	int count=0;
	for (int g=target.x+static_cast<int>(0.5*target.width)-static_cast<int>(0.2*target.width); 
		g<target.x+static_cast<int>(0.5*target.width)+static_cast<int>(0.2*target.width); g++)
	{
		for (int n=target.y+static_cast<int>(0.5*target.height)-static_cast<int>(0.2*target.height); 
			n<target.y+static_cast<int>(0.5*target.height)+static_cast<int>(0.2*target.height); n++)
		{
			sum += cvGetReal2D(sat, n, g);
			count++;
		}
	}
	targetSat = sum/count;

	//2009/4/15 1726
	cvInRangeS(sat, cvScalar(targetSat-SatTh), cvScalar(targetSat+SatTh), sat);
	cvEqualizeHist(sat, sat);
#ifdef _DEBUG
	cvNamedWindow("initial sat", 1);
	cvShowImage("initial sat", sat);
	//2009/4/15 1726
#endif	

	cvAnd( mask, fgMask, mask );
	cvSetImageROI( hue, target );
	cvSetImageROI( mask, target );


	cvCalcHist( &hue, hist, 0, mask );

	//........................CrCb refHist construction................................
	cvCvtColor(image, image_YCrCb, CV_BGR2YCrCb);
	cvSplit(image_YCrCb, image_Y, image_Cr, image_Cb, NULL );
//	cvAnd(image_Cr, fgMask, image_Cr);
//	cvAnd(image_Cb, fgMask, image_Cb);

	IplImage * sample_planes[] = {image_Cr, image_Cb};

	cvCalcHist( sample_planes, refHist, 0, fgMask );

	float max_val = 0.0f;
	cvGetMinMaxHistValue( refHist, 0, &max_val, 0, 0 );
	//fileOut2 << "max_val = " << max_val <<endl;
	cvConvertScale( refHist->bins, refHist->bins, max_val ? 255. / max_val : 0., 0 );

	//要backprojection的話不可以normalized!!!!!!   //cvNormalizeHist(refHist, 1);
	
	cvGetMinMaxHistValue( refHist, 0, &max_val, 0, 0 );
	//fileOut2 << "max_val = " << max_val <<endl;
	//.........................CrCb refHist constructed...................................

	max_val = 0.f;
	cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 );
	cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 );
	cvResetImageROI( hue );
	cvResetImageROI( mask );
	int bin_w = histimg->width / hdims;

	//show raw histogram
	cvZero(histimg);
	for( int i = 0; i < hdims; i++ )
	{
		int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
		CvScalar color = hsv2rgb(i*180.f/hdims);
		cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
				cvPoint((i+1)*bin_w,histimg->height - val),
				color, -1, 8, 0 );
	}
//	cvNamedWindow("target Hist(raw)", 1);
//	cvShowImage("target Hist(raw)", histimg);

	////////////start to purify the histogram
	//////////cvZero(histimg);
	//////////for (int i=0; i<hdims; i++)
	//////////{
	//////////	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//////////	{
	//////////		*(cvGetHistValue_1D(hist, i)) = 0.;
	//////////	}
	//////////	
	//////////	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//////////	CvScalar color = hsv2rgb(i*180.f/hdims);
	//////////	if (val != 0)
	//////////	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//////////			cvPoint((i+1)*bin_w,histimg->height - val),
	//////////			color, -1, 8, 0 );
	//////////}

	//2009/4/15 1705
#ifdef _DEBUG
	cvNamedWindow("target Hist(purified)", 1);
	cvShowImage("target Hist(purified)", histimg);
	//2009/4/15 1705
#endif
//????????????????	cvNormalizeHist(hist,1);
#ifdef _DEBUG
	cvNamedWindow("initial hue", 1);
	cvShowImage("initial hue", hue);
#endif
	cvZero(backproject);

/////////////////////////////////////////////////////////////////////////////////////////////////
	cvCalcBackProject( sample_planes, extend_mask, refHist );
////reserve
#ifdef _DEBUG
	cvNamedWindow("extend_mask_be");
	cvShowImage("extend_mask_be", extend_mask);
#endif
	cvRectangle( extend_mask, cvPoint(0,0), cvPoint(extend_mask->width,extend_mask->height), CV_RGB(0, 0, 0), 10, 8, 0 );

	//downsampling to half of the original size
//	cvResize(morphImage, temp_half, CV_INTER_AREA);
//	cvResize(morphImage, morph_half, CV_INTER_LINEAR);
	cvErode(extend_mask, extend_mask, /*big*/smlErosion, 1);
	cvDilate(extend_mask, extend_mask, bigErosion, 1);
	cvThreshold(extend_mask, extend_mask, 70, 250, CV_THRESH_BINARY);
////reserve
#ifdef _DEBUG
	cvNamedWindow("extend_mask");
	cvShowImage("extend_mask", extend_mask);
#endif
	//check extend_mask area here
	sum = 0;
	for (int g=0; g<width; g++)
	{
		for (int n=0; n<height; n++)
		{
			sum += (int)CV_IMAGE_ELEM(extend_mask, unsigned char, n, g);

//			fileOut2 << cvGetReal2D(backproject, n, g) << " at (" << g << ", " << n << ")" << endl;
//			fileOut2 << (int)CV_IMAGE_ELEM(backproject, unsigned char, n, g) << " at (" << g << ", " << n << ")" << endl;
		}
	}
	//fileOut2 << "extend_mask area = " << sum <<endl;
	
	if (sum > 0.1*250.0*(double)width*(double)height || sum < 0.01*250.0*(double)width*(double)height) return false;



/////////////////////////////////////////////////////////////////////////////////////////////////



	cvCalcHist(sample_planes, refHist, 0, extend_mask);
	max_val = 0.0f;
	cvGetMinMaxHistValue( refHist, 0, &max_val, 0, 0 );
	//fileOut2 << "max_val = " << max_val <<endl;
	cvConvertScale( refHist->bins, refHist->bins, max_val ? 255. / max_val : 0., 0 );

	//要backprojection的話不可以normalized!!!!!!   //cvNormalizeHist(refHist, 1);
	
	cvGetMinMaxHistValue( refHist, 0, &max_val, 0, 0 );
	//fileOut2 << "max_val = " << max_val <<endl;

	////
	cvCalcBackProject( sample_planes, extend_mask, refHist );
	cvRectangle( extend_mask, cvPoint(0,0), cvPoint(extend_mask->width,extend_mask->height), CV_RGB(0, 0, 0), 10, 8, 0 );
#ifdef _DEBUG
	cvNamedWindow("extend_mask_after");
	cvShowImage("extend_mask_after", extend_mask);
	////
#endif
	cvCalcBackProject( &hue, backproject, hist );

	//2009/4/15 2033
	sum = 0;
	for (int g=0; g<width; g++)
	{
		for (int n=0; n<height; n++)
		{
			sum += cvGetReal2D(backproject, n, g);

//			fileOut2 << cvGetReal2D(backproject, n, g) << " at (" << g << ", " << n << ")" << endl;
//			fileOut2 << (int)CV_IMAGE_ELEM(backproject, unsigned char, n, g) << " at (" << g << ", " << n << ")" << endl;
		}
	}
	//fileOut2 << "sum = " << sum <<endl;
	
	//2009/4/15 2033
	
//	cvAnd( backproject, mask, backproject, 0 );

	//2009/4/15 1705
	cvEqualizeHist(backproject, backproject);
#ifdef _DEBUG
	cvNamedWindow("initial backproject", 1);
	cvShowImage("initial backproject", backproject);
	//2009/4/15 1705
#endif	
	noMotionCnt = 0;
	losingCnt = 0;
	maxIt = 0;
	if (ThreadNr == 0)	
		addThread();

	iteration_cnt = 0;
	init_flag = true; 
	detect_flag = false;

	return true;
}

//use outer source (color sample) to construct color information
void CAMShift::initCamShiftChecking(IplImage * image, IplImage * fgMask, CvRect target )
{	
	setUpSupportingClass(image);

	cvResetImageROI( hue/*, target */);
	cvResetImageROI( mask/*, target*/ );
	cvResetImageROI( hsv/*, target */);
	cvResetImageROI( image/*, target*/ );
	cvResetImageROI( sat/*, target */);
	cvResetImageROI( val/*, target*/ );
	cvResetImageROI( fgMask/*, target*/ );
	cvResetImageROI( backproject/*, target*/);
	cvResetImageROI( edgeResponse/*, target*/);


	track_window = target;
	cvCvtColor( image, hsv, CV_BGR2HSV );
	cvInRangeS( hsv, cvScalar(0,smin,MIN(vmin,vmax),0),	cvScalar(180,256,MAX(vmin,vmax),0), mask );
	
	cvDilate( mask, mask, ck, 1 );

	cvSplit( hsv, hue, sat, val, 0 );

	
	cvAnd(sat, fgMask, sat);
	cvAnd(val, fgMask, val);

	cvAnd( mask, fgMask, mask );
	cvSetImageROI( hue, target );
	cvSetImageROI( mask, target );
	
	//改成外部給定的color sample
	//cvCalcHist( &hue, hist, 0, mask );
	if (cam_id == CAM1) 
	{
		//fileOut2 << "initCAMShift green CAM1 = " << cam_id << endl;
		sample_img = cvLoadImage("green_CAM1.bmp", 1);//in bgr space
	}
	else //cam_id == CAM2
	{
		//fileOut2 << "initCAMShift green CAM2 = " << cam_id << endl;
		sample_img = cvLoadImage("green_CAM2.bmp", 1);//This is for CAM2 setting
	}

	//........................CrCb refHist construction................................
	cvCvtColor(sample_img, sample_YCrCb, CV_BGR2YCrCb);
	cvSplit(sample_YCrCb, sample_Y, sample_Cr, sample_Cb, NULL );

	IplImage * sample_planes[] = {sample_Cr, sample_Cb};

	cvCalcHist( sample_planes, refHist, 0, 0 );

	float max_val = 0.0f;
	cvGetMinMaxHistValue( refHist, 0, &max_val, 0, 0 );
	//fileOut2 << "max_val = " << max_val <<endl;
	cvConvertScale( refHist->bins, refHist->bins, max_val ? 255. / max_val : 0., 0 );

	//要backprojection的話不可以normalized!!!!!!   //cvNormalizeHist(refHist, 1);
	
	cvGetMinMaxHistValue( refHist, 0, &max_val, 0, 0 );
	//fileOut2 << "max_val = " << max_val <<endl;
	//.........................CrCb refHist constructed...................................
	
	//改成外部給定的color sample
	//cvCalcHist( &hue, hist, 0, mask );
	if (cam_id == CAM1)
	{
		//fileOut2 << "initCAMShift hair CAM1 = " << cam_id << endl;
		sample_img2 = cvLoadImage("hair_CAM1.bmp", 1);//in bgr space
	}
	else
	{
		//fileOut2 << "initCAMShift hair CAM2 = " << cam_id << endl;
		sample_img2 = cvLoadImage("hair_CAM2.bmp", 1);//in bgr space//This is for CAM2 setting
	}
	//........................CrCb refHist construction................................
	cvCvtColor(sample_img2, sample_YCrCb2, CV_BGR2YCrCb);
	cvSplit(sample_YCrCb2, sample_Y2, sample_Cr2, sample_Cb2, NULL );

	IplImage * sample_planes2[] = {sample_Cr2, sample_Cb2};

	cvCalcHist( sample_planes2, refHairHist, 0, 0 );

	max_val = 0.0f;
	cvGetMinMaxHistValue( refHairHist, 0, &max_val, 0, 0 );
	//fileOut2 << "max_val = " << max_val <<endl;
	cvConvertScale( refHairHist->bins, refHairHist->bins, max_val ? 255. / max_val : 0., 0 );

	//要backprojection的話不可以normalized!!!!!!   //cvNormalizeHist(refHist, 1);
	
	cvGetMinMaxHistValue( refHairHist, 0, &max_val, 0, 0 );
	//fileOut2 << "max_val = " << max_val <<endl;
	//.........................CrCb refHist constructed...................................


//	cvNamedWindow("color_sample", 1);
//	cvShowImage("color_sample", sample_img);
	cvCvtColor(sample_img, sample_hsv, CV_BGR2HSV);  
	cvSplit(sample_hsv, sample_hue, sample_sat, sample_val, 0);
	cvCalcHist(&sample_hue, hist, 0, 0);
	long double sum=0;
	int count=0;
	for (int g=0; 
		g<cvGetSize(sample_sat).width; g++)
	{
		for (int n=0; 
			n<cvGetSize(sample_sat).height; n++)
		{
			sum += cvGetReal2D(sample_sat, n, g);
			count++;
		}
	}
	targetSat = sum/count;
	cvInRangeS(sat, cvScalar(targetSat-SatTh), cvScalar(targetSat+SatTh), sat);
	cvEqualizeHist(sat, sat);
#ifdef _DEBUG
	cvNamedWindow("initial sat", 1);
	cvShowImage("initial sat", sat);
#endif
	//
	max_val = 0.f;
	cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 );
	cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 );
	cvResetImageROI( hue );
	cvResetImageROI( mask );
	int bin_w = histimg->width / hdims;

	//show raw histogram
	cvZero(histimg);
	for( int i = 0; i < hdims; i++ )
	{
		int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
		CvScalar color = hsv2rgb(i*180.f/hdims);
		cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
				cvPoint((i+1)*bin_w,histimg->height - val),
				color, -1, 8, 0 );
	}
//	cvNamedWindow("target Hist(raw)", 1);
//	cvShowImage("target Hist(raw)", histimg);

	////start to purify the histogram
	//cvZero(histimg);
	//for (int i=0; i<hdims; i++)
	//{
	//	if (cvGetReal1D(hist->bins,i) < purifyFactor*255.)
	//	{
	//		*(cvGetHistValue_1D(hist, i)) = 0.;
	//	}
	//	
	//	int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
	//	CvScalar color = hsv2rgb(i*180.f/hdims);
	//	if (val != 0)
	//	cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
	//			cvPoint((i+1)*bin_w,histimg->height - val),
	//			color, -1, 8, 0 );
	//}

	//2009/4/15 1705
#ifdef _DEBUG
	cvNamedWindow("target Hist(purified)", 1);
	cvShowImage("target Hist(purified)", histimg);
	//2009/4/15 1705
#endif
	cvNormalizeHist(hist,1);
#ifdef _DEBUG
	cvNamedWindow("initial hue", 1);
	cvShowImage("initial hue", hue);
#endif
	cvZero(backproject);

	cvCalcBackProject( &hue, backproject, hist );

	//2009/4/15 2033
	sum = 0;
	for (int g=0; g<width; g++)
	{
		for (int n=0; n<height; n++)
		{
			sum += cvGetReal2D(backproject, n, g);

//			fileOut2 << cvGetReal2D(backproject, n, g) << " at (" << g << ", " << n << ")" << endl;
//			fileOut2 << (int)CV_IMAGE_ELEM(backproject, unsigned char, n, g) << " at (" << g << ", " << n << ")" << endl;
		}
	}
	//fileOut2 << "sum = " << sum <<endl;
	
	//2009/4/15 2033
	
//	cvAnd( backproject, mask, backproject, 0 );

	//2009/4/15 1705
	cvEqualizeHist(backproject, backproject);
#ifdef _DEBUG
	cvNamedWindow("initial backproject", 1);
	cvShowImage("initial backproject", backproject);
	//2009/4/15 1705
#endif	
	noMotionCnt = 0;
	losingCnt = 0;
	maxIt = 0;
	if (ThreadNr == 0)	
		addThread();

	iteration_cnt = 0;
	init_flag = true; 
	detect_flag = false;


	int slot_size = (PARTICLE_NUM)/MAX_TARGET_NUM;
	
	if (cam_id == CAM1)
	{
		//fileOut2 << "initCAMShift Calibration CAM1 = " << cam_id << endl;
		Position3D opticPt(0,0,220);
		Position3D H(276,184,118);
		Position3D unbiasedH;

		for (int k=0; k<MAX_TARGET_NUM; k++)
		{
			//locate target memories
			targetList[k].ptIndex1 = slot_size*k;
			targetList[k].ptIndex2 = slot_size*(k+1);
			targetList[k].ptNum = slot_size;

			//set losingCnt
			targetLosingCnt[k] = 0;
	
			targetHeight[k] = 155;//145;
	
			targetLines[k].initParamBy3DPt(H, opticPt, H, 0.53/*, CAM1*/);
			targetLines[k].setByImgLoc(cvPoint(CAM1_2CX, CAM1_2CY));
			targetLines[k].getLinePtOnPlane(targetLines[k].refPlane, unbiasedH);
			targetLines[k].initParamBy3DPt_precise(unbiasedH, opticPt, unbiasedH, 0.53, CAM1);
		}
		PrevJointCnt = 0;
	
	}
	else //cam_id = CAM2
	{
		//fileOut2 << "initCAMShift Calibration CAM2 = " << cam_id << endl;
		Position3D opticPt(412,0,220);//(0,0,220);//This is for CAM2 setting
		Position3D G(295,256,104);//B(155,227,140);//This is for CAM2 setting
		Position3D unbiasedG;
	
		for (int k=0; k<MAX_TARGET_NUM; k++)
		{
			//locate target memories
			targetList[k].ptIndex1 = slot_size*k;
			targetList[k].ptIndex2 = slot_size*(k+1);
			targetList[k].ptNum = slot_size;
	
			//set losingCnt
			targetLosingCnt[k] = 0;	
	
			targetHeight[k] = 155;//This is for CAM2 setting
	
			targetLines[k].initParamBy3DPt(G, opticPt, G, 0.8);//(B, opticPt, B, 0.392338);//This is for CAM2 setting
			targetLines[k].setByImgLoc(cvPoint(CAM2_2CX, CAM2_2CY));
			targetLines[k].getLinePtOnPlane(targetLines[k].refPlane, unbiasedG);
			targetLines[k].initParamBy3DPt_precise(unbiasedG, opticPt, unbiasedG, 0.81, CAM2);

		}
		PrevJointCnt = 0;
	
	}



		
}

CvScalar CAMShift::hsv2rgb( float hue )
{
    int rgb[3], p, sector;
    static const int sector_data[][3]=
        {{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
    hue *= 0.033333333333333f;
    sector = cvFloor(hue);
    p = cvRound(255*(hue - sector));
    p ^= sector & 1 ? 255 : 0;

    rgb[sector_data[sector][0]] = 255;
    rgb[sector_data[sector][1]] = 0;
    rgb[sector_data[sector][2]] = p;

    return cvScalar(rgb[2], rgb[1], rgb[0],0);
}

void CAMShift::reInit()
{
	mStartFlag = true;
}


int CAMShift::reInterpret( float angle ) const
{
	//assert( (angle >= 0.0f && angle <= 360.0f) );

	while( angle < -180.0f ) angle += 360.0f;
	while( angle > 180.0f ) angle -= 360.0f;

	assert( (angle >= 0.0f && angle <= 180.0f) || (angle <= 0.0f && angle >= -180.0f) );

	float range = 45.0f;
	if( (angle >= 180-range && angle <= 180) || (angle >= 0 && angle <= range) || (angle >= -range  && angle <= 0) || (angle >= -180 && angle <= -180+range)) // 開始 (橫
	{
		return 1;
	}
	else // if( (angle >= 120 && angle <= 180) || (angle >= -180 && angle <= -120) ) // 結束
	{
		return -1;
	}
}

void CAMShift::sendControlSignal( double timeDiff, SENDING_MODE mode )
{
	switch( mode )
	{
	case DONT_CARE:
		{
			stringstream sstrX[MAX_TARGET_NUM];
			stringstream sstrY[MAX_TARGET_NUM];
			stringstream sstrR[MAX_TARGET_NUM];
			stringstream sstrTheta[MAX_TARGET_NUM];
			stringstream sstrId[MAX_TARGET_NUM];

			string XPOS, YPOS, RSCAL, THVAL, outSig;


			for (int k=0; k<MAX_TARGET_NUM; k++)
			{
				if (targetList[k].isAlive)
				{
					sstrX[k] << (int)targetList[k].tgMAP.x;
					sstrY[k] << (int)targetList[k].tgMAP.y;
					sstrR[k] << (int)(targetList[k].tgMAP.r*100.0);
					sstrTheta[k] << (int)targetList[k].tgMAP.theta;
					sstrId[k] << k;
					XPOS = sstrX[k].str();
					YPOS = sstrY[k].str();
					RSCAL = sstrR[k].str();
					THVAL = sstrTheta[k].str();
					outSig = "target No." + sstrId[k].str() + ": Position: ( " + XPOS + " " + YPOS + " )" + " Angle: " + THVAL + " Scale: " + RSCAL + "\r\n";
					//(*mPresentedControlString) += outSig;
				}
			}
			////////////int posX=-1, posY=-1, angle=-1, scale=-1;
			//////////////double currentArea = track_box.size.height*track_box.size.width;
			//////////////posX = track_box.center.x;//getPosX(static_cast<int>(track_pos.x));
			//////////////posY = track_box.center.y;//getPosY(static_cast<int>(track_pos.y));
			//////////////angle = track_box.angle;//getRotAng(track_agl);
			//////////////scale = track_box.size.width;//getScale(STANDARD_THRESHOLD, currentArea);
			////////////posX = mapX;
			////////////posY = mapY;
			////////////angle = mapTheta;
			////////////scale = mapR*100.0;

			////////////stringstream sstrX;
			////////////sstrX << posX;
			////////////string strX = sstrX.str();

			////////////stringstream sstrY;
			////////////sstrY << posY;
			////////////string strY = sstrY.str();

			////////////string sPos = "Position: ( " + strX + " " + strY + " )";

			////////////stringstream sstrAngle;
			////////////sstrAngle << angle;
			////////////string strAngle = sstrAngle.str();

			////////////string sAngle = " Angle: " + strAngle;

			////////////stringstream sstrScale;
			////////////sstrScale << scale;
			////////////string strScale = sstrScale.str();

			////////////string sScale = " Scale: " + strScale + "\r\n";

			////////////(*mPresentedControlString) += (sPos + sAngle + sScale);


			//////////////if( mPM->getCurrentSampleSource() != VISION )
			//////////////{
			//////////////	cout << "current sample source is not vision" << endl;
			//////////////	return;
			//////////////}

			////////////int angleStatus = reInterpret(angle);

			////////////// translate
			//////////////if( mPM->getCurrentPMStatus() == RECOGNIZING || mPM->getCurrentPMStatus() == TRAINING )
			//////////////{
			//////////////	mPM->getVisionTranslate()->feedSampleIntoPM(posX, posY, angleStatus);
			//////////////}
			//////////////else
			//////////////{
			//////////////	if( mClient->mbInitialized )
			//////////////	{
			//////////////		if( mPM->canSendRawData() == true )
			//////////////		{
			//////////////			// send raw data
			//////////////			mClient->mWriteInt( 0 );
			//////////////			mClient->mWriteInt( posX );
			//////////////			mClient->mWriteInt( posY );
			//////////////			mClient->mWriteInt( angle );

			//////////////			mClient->mSend();
			//////////////		}
			//////////////	}
			//////////////}
		}
		//else{} // time limit unreached
		break;
	case REGRESSION:
		//if( mClient->mbInitialized /*&& timeDiff >= 0.2*/ )
		{
			(*mPresentedControlString) += string("regression\r\n");
		}

		break;
	case DISCONNECT:
		//if( mClient->mbInitialized /*&& timeDiff >= 0.2*/ )
		{
		}
		break;

	case TEST_ALIVE:
		//if( mClient->mbInitialized /*&& timeDiff >= 0.2*/ )
		{
		}
		break;
	}
}

CamIterationReturn CAMShift::setupReturn(CvPoint &ROIcenter, int returnValue)
{
	losingCnt = 0;
	CamIterationReturn Return;
	Return.targetCenter = cvPointFrom32f(track_box.center);
	Return.val = returnValue;
	Return.targetROI = cvRect(((ROIcenter.x-static_cast<int>(0.5*ROIdim) > 0)?(ROIcenter.x-static_cast<int>(0.5*ROIdim)):0),
							  ((ROIcenter.y-static_cast<int>(0.5*ROIdim) > 0)?(ROIcenter.y-static_cast<int>(0.5*ROIdim)):0),
							  ((ROIcenter.x+static_cast<int>(0.5*ROIdim) < width)?(ROIdim):(width-ROIcenter.x+static_cast<int>(0.5*ROIdim))),
							  ((ROIcenter.y+static_cast<int>(0.5*ROIdim) < height)?(ROIdim):(height-ROIcenter.y+static_cast<int>(0.5*ROIdim))));

	if( mFlagIteration[mLosingCount] == mIterationCount )
	{
		mLosingCount++;
	}
	else
	{
		mLosingCount = 1; // discard the count before, and start a new one
		mIterationCount = 0;
	}

	//time(&mStartTimeStamp);
	return Return;
}

void CAMShift::Predict()
{
	rng = cvRNG( rand() );

	cvRandArr( &rng, X_rand, CV_RAND_NORMAL, cvScalar(0) /* mean */, cvScalar(X_DEV) /* standard deviation */ );
	cvRandArr( &rng, Y_rand, CV_RAND_NORMAL, cvScalar(0) /* mean */, cvScalar(Y_DEV) /* standard deviation */ );
	cvRandArr( &rng, R_rand, CV_RAND_NORMAL, cvScalar(0) /* mean */, cvScalar(R_DEV) /* standard deviation */ );
	cvRandArr( &rng, Theta_rand, CV_RAND_NORMAL, cvScalar(0) /* mean */, cvScalar(THETA_DEV) /* standard deviation */ );

	cvAdd( X, X_rand, X, NULL );
	cvAdd( Y, Y_rand, Y, NULL );
	cvAdd( R, R_rand, R, NULL );
	cvAdd( Theta, Theta_rand, Theta, NULL );

	//限制state的範圍，使particle灑在ROI之中
	double x, y, r, theta1;
	for( int i=0; i<PARTICLE_NUM; i++ ){
		x = cvmGet( X, 0, i );
		y = cvmGet( Y, 0, i );
		r = cvmGet( R, 0, i );
		theta1 = cvmGet( Theta, 0, i );

		srand(time(NULL));

		double rmax = /*(R_MAX - (double)(rand()%3))/MODEL_R*/1.0;
		double rmin = /*(R_MIN + (double)(rand()%3))/MODEL_R*//*0.3333*/0.6;

		r = r > 1.0/*R_MAX/MODEL_R*/ ? rmax : r < 0.6/*0.33333f*//*R_MIN/MODEL_R*/
					  ? rmin : r;
		x = (x+MODEL_R*r*ELLIPSE_RATIO) > width  ? width-MODEL_R*r*ELLIPSE_RATIO : (x-MODEL_R*r*ELLIPSE_RATIO) < 0
					       ? MODEL_R*r*ELLIPSE_RATIO : x ;
		y = (y-MODEL_R*r*ELLIPSE_RATIO) < 0 ? MODEL_R*r*ELLIPSE_RATIO : (y+MODEL_R*r*ELLIPSE_RATIO) < height 
								   ? y : height - MODEL_R*r*ELLIPSE_RATIO;
		theta1 = theta1 > 180/*90*//*CV_PI/2*/ ? -180+(theta1-180)/*-90+(theta1-90)*//*90*//*CV_PI/2*/ : theta1 < -180/*-90*//*-CV_PI/2*/
							 ? 180+(theta1+180)/*90+(theta1+90)*//*-90*//*-CV_PI/2*/ : theta1;

		cvmSet( X, 0, i, x );
		cvmSet( Y, 0, i, y );
		cvmSet( R, 0, i, r );
		cvmSet( Theta, 0, i, 90.00/*theta1*/ );
		//cout << i << " " << alpha/PI*180 << endl;
	}
	
	//if (detect_flag) return;

	////we predict add_X, add_Y, add_R, add_Theta if we didn't detect backproject 
	//rng = cvRNG( rand() );

	//cvRandArr( &rng, add_X_rand, CV_RAND_NORMAL, cvScalar(0) /* mean */, cvScalar(X_DEV) /* standard deviation */ );
	//cvRandArr( &rng, add_Y_rand, CV_RAND_NORMAL, cvScalar(0) /* mean */, cvScalar(Y_DEV) /* standard deviation */ );
	//cvRandArr( &rng, add_R_rand, CV_RAND_NORMAL, cvScalar(0) /* mean */, cvScalar(R_DEV) /* standard deviation */ );
	//cvRandArr( &rng, add_Theta_rand, CV_RAND_NORMAL, cvScalar(0) /* mean */, cvScalar(THETA_DEV) /* standard deviation */ );

	//cvAdd( add_X, add_X_rand, add_X, NULL );
	//cvAdd( add_Y, add_Y_rand, add_Y, NULL );
	//cvAdd( add_R, add_R_rand, add_R, NULL );
	//cvAdd( add_Theta, add_Theta_rand, add_Theta, NULL );

	////限制state的範圍，使particle灑在ROI之中
	//
	//for( int i=0; i<ADD_PARTICLE_NUM; i++ ){
	//	x = cvmGet( add_X, 0, i );
	//	y = cvmGet( add_Y, 0, i );
	//	r = cvmGet( add_R, 0, i );
	//	theta1 = cvmGet( add_Theta, 0, i );

	//	srand(time(NULL));

	//	double rmax = /*(R_MAX - (double)(rand()%3))/MODEL_R*/1.3333;
	//	double rmin = /*(R_MIN + (double)(rand()%3))/MODEL_R*/0.3333;

	//	r = r > 1.33333/*R_MAX/MODEL_R*/ ? rmax : r < 0.33333f/*R_MIN/MODEL_R*/
	//				  ? rmin : r;
	//	x = (x+MODEL_R*r*ELLIPSE_RATIO) > width  ? width-MODEL_R*r*ELLIPSE_RATIO : (x-MODEL_R*r*ELLIPSE_RATIO) < 0
	//				       ? MODEL_R*r*ELLIPSE_RATIO : x ;
	//	y = (y-MODEL_R*r*ELLIPSE_RATIO) < 0 ? MODEL_R*r*ELLIPSE_RATIO : (y+MODEL_R*r*ELLIPSE_RATIO) < height 
	//							   ? y : height - MODEL_R*r*ELLIPSE_RATIO;
	//	theta1 = theta1 > 180/*90*//*CV_PI/2*/ ? -180+(theta1-180)/*-90+(theta1-90)*//*90*//*CV_PI/2*/ : theta1 < -180/*-90*//*-CV_PI/2*/
	//						 ? 180+(theta1+180)/*90+(theta1+90)*//*-90*//*-CV_PI/2*/ : theta1;

	//	cvmSet( add_X, 0, i, x );
	//	cvmSet( add_Y, 0, i, y );
	//	cvmSet( add_R, 0, i, r );
	//	cvmSet( add_Theta, 0, i, theta1 );
	//	//cout << i << " " << alpha/PI*180 << endl;
	//}

	//cvReleaseMat( & X_rand );
	//cvReleaseMat( & Y_rand );
	//cvReleaseMat( & R_rand );
	//cvReleaseMat( & Alpha_rand );
}
void CAMShift::Resampling(Target tg)
{
	int slot_size = (PARTICLE_NUM)/MAX_TARGET_NUM;
	cvGetCols( X, sub_X, (tg.tarIndex)*slot_size, (tg.tarIndex+1)*slot_size);
	cvGetCols( Y, sub_Y, (tg.tarIndex)*slot_size, (tg.tarIndex+1)*slot_size);
	cvGetCols( R, sub_R, (tg.tarIndex)*slot_size, (tg.tarIndex+1)*slot_size);
	cvGetCols( Theta, sub_Theta, (tg.tarIndex)*slot_size, (tg.tarIndex+1)*slot_size);
	cvGetCols( Weight3, sub_Weight, (tg.tarIndex)*slot_size, (tg.tarIndex+1)*slot_size);

	cvSet( sub_X, cvScalar(tg.tgMAP.x) );
	cvSet( sub_Y, cvScalar(tg.tgMAP.y) );
	cvSet( sub_R, cvScalar(tg.tgMAP.r) );
	//angle 加負號??
	cvSet( sub_Theta, cvScalar(tg.tgMAP.theta) );
	cvSet( sub_Weight, cvScalar(1.0/slot_size) );
	
	Resampling1();

	//init_flag = true;

	//if (!detect_flag)
	//{
	//	cvSet( add_X, cvScalar(ps.x) );
	//	cvSet( add_Y, cvScalar(ps.y) );
	//	cvSet( add_R, cvScalar(ps.r) );
	//	//angle 加負號??
	//	cvSet( add_Theta, cvScalar(ps.theta) );
	//}
}
void CAMShift::Resampling(CvBox2D init_box)
{
	cvSet( X, cvScalar(init_box.center.x) );
	cvSet( Y, cvScalar(init_box.center.y) );
	cvSet( R, cvScalar(0.5*min(init_box.size.width, init_box.size.height)/MODEL_R) );
	//angle 加負號??
	cvSet( Theta, cvScalar(-init_box.angle) );

	//init_flag = true;

	if (!detect_flag)
	{
		cvSet( add_X, cvScalar(init_box.center.x) );
		cvSet( add_Y, cvScalar(init_box.center.y) );
		cvSet( add_R, cvScalar(0.5*min(init_box.size.width, init_box.size.height)/MODEL_R) );
		//angle 加負號??
		cvSet( add_Theta, cvScalar(-init_box.angle) );
	}
}
void CAMShift::Resampling1()
{
	//multi-target
	double cdf = 0;
	double count = 0;
	double x0, y0, r0, a0;
	double temp_cnt;
	double slot_size = (PARTICLE_NUM)/MAX_TARGET_NUM;
	
	tmp_X = cvCloneMat( X );
	tmp_Y = cvCloneMat( Y );
	tmp_R = cvCloneMat( R );
	tmp_Theta = cvCloneMat( Theta );

	//tmp_add_X = cvCloneMat( add_X );
	//tmp_add_Y = cvCloneMat( add_Y );
	//tmp_add_R = cvCloneMat( add_R );
	//tmp_add_Theta = cvCloneMat( add_Theta );

	for (int k=0; k<MAX_TARGET_NUM; k++)
		if (targetList[k].isAlive)
		{
			cdf = 0;
			count = 0;
			temp_cnt = 0;
			for( int i=targetList[k].ptIndex1; i < targetList[k].ptIndex2; i++ )
			{
				cdf += cvmGet( Weight3, 0, i );
				//fileOut2 << "cdf: " << cdf << endl;
				if( cdf > ((double)count)/slot_size )
				{
					x0 = cvmGet( tmp_X, 0, i );
					y0 = cvmGet( tmp_Y, 0, i );
					r0 = cvmGet( tmp_R, 0, i );
					a0 = cvmGet( tmp_Theta, 0, i );
					
					temp_cnt = cvRound(cdf*slot_size);
					while( count < temp_cnt && count < slot_size)
					{
						cvmSet( X, 0, targetList[k].ptIndex1 + count, x0 );
						cvmSet( Y, 0, targetList[k].ptIndex1 + count, y0 );
						cvmSet( R, 0, targetList[k].ptIndex1 + count, r0 );
						cvmSet( Theta, 0, targetList[k].ptIndex1 + count, a0 );
						
						count ++;
						//fileOut2 << "count: " << count <<", temp_cnt = "<< temp_cnt << endl;
					}
				}
				if(count >= PARTICLE_NUM/*  + ADD_PARTICLE_NUM*/)
					break;
			}
		}	

	//get new samples from backprojection now
	if (0/*detect_flag*/)
	{
		add_X = cvCloneMat( add_new_X );
		add_Y = cvCloneMat( add_new_Y );
		add_R = cvCloneMat( add_new_R );
		add_Theta = cvCloneMat( add_new_Theta );
	}
}
void CAMShift::Resampling()
{
	double cdf = 0;
	double count = 0;
	double x0, y0, r0, a0;
	double temp_cnt;
	
	tmp_X = cvCloneMat( X );
	tmp_Y = cvCloneMat( Y );
	tmp_R = cvCloneMat( R );
	tmp_Theta = cvCloneMat( Theta );

	tmp_add_X = cvCloneMat( add_X );
	tmp_add_Y = cvCloneMat( add_Y );
	tmp_add_R = cvCloneMat( add_R );
	tmp_add_Theta = cvCloneMat( add_Theta );


	for( int i=0; i < PARTICLE_NUM + ADD_PARTICLE_NUM; i++ ){
		cdf += cvmGet( Weight3, 0, i );
		if( cdf > (double)count/(PARTICLE_NUM + ADD_PARTICLE_NUM) ){
			if (i < PARTICLE_NUM)
			{
				x0 = cvmGet( tmp_X, 0, i );
				y0 = cvmGet( tmp_Y, 0, i );
				r0 = cvmGet( tmp_R, 0, i );
				a0 = cvmGet( tmp_Theta, 0, i );
			}
			else
			{
				x0 = cvmGet( tmp_add_X, 0, i-PARTICLE_NUM );
				y0 = cvmGet( tmp_add_Y, 0, i-PARTICLE_NUM );
				r0 = cvmGet( tmp_add_R, 0, i-PARTICLE_NUM );
				a0 = cvmGet( tmp_add_Theta, 0, i-PARTICLE_NUM );
			}

			temp_cnt = cvRound(cdf*(PARTICLE_NUM + ADD_PARTICLE_NUM));
			while( count < temp_cnt && count < PARTICLE_NUM + ADD_PARTICLE_NUM){
				if (detect_flag)
				{
					cvmSet( X, 0, count*(PARTICLE_NUM/(PARTICLE_NUM + ADD_PARTICLE_NUM)), x0 );
					cvmSet( Y, 0, count*(PARTICLE_NUM/(PARTICLE_NUM + ADD_PARTICLE_NUM)), y0 );
					cvmSet( R, 0, count*(PARTICLE_NUM/(PARTICLE_NUM + ADD_PARTICLE_NUM)), r0 );
					cvmSet( Theta, 0, count*(PARTICLE_NUM/(PARTICLE_NUM + ADD_PARTICLE_NUM)), a0 );
				}
				else
				{
					if (count<PARTICLE_NUM)
					{
						cvmSet( X, 0, count, x0 );
						cvmSet( Y, 0, count, y0 );
						cvmSet( R, 0, count, r0 );
						cvmSet( Theta, 0, count, a0 );
					}
					else
					{
						cvmSet( add_X, 0, count-PARTICLE_NUM, x0 );
						cvmSet( add_Y, 0, count-PARTICLE_NUM, y0 );
						cvmSet( add_R, 0, count-PARTICLE_NUM, r0 );
						cvmSet( add_Theta, 0, count-PARTICLE_NUM, a0 );
					}
				}
				count ++;
//				fileOut2 << "particle_info: " << x0+y0+r0+a0 << endl;
			}
		}
		if(count >= PARTICLE_NUM  + ADD_PARTICLE_NUM)
			break;
	}

	//get new samples from backprojection now
	if (detect_flag)
	{
		add_X = cvCloneMat( add_new_X );
		add_Y = cvCloneMat( add_new_Y );
		add_R = cvCloneMat( add_new_R );
		add_Theta = cvCloneMat( add_new_Theta );
	}
}

ParticleStatus CAMShift::MAP()
{
	ParticleStatus best;
	double w, max_weight=0;
	for (int i=0; i<PARTICLE_NUM; i++)
	{
		w = cvmGet(Weight1, 0, i);
		if (w >= max_weight)
		{
			max_weight = w;
			best.index = i;
		}
	}
	best.x = cvmGet(X, 0, best.index);
	best.y = cvmGet(Y, 0, best.index);
	best.r = cvmGet(R, 0, best.index);
	best.theta = cvmGet(Theta, 0, best.index);
	return best;
}
void CAMShift::JointSetWeight(double weight, int leaf_cnt, Target* self_ptr, int t)
{
	//set weight for all corresponding targets in the same joint group
	
	cvmSet(Weight3, 0, self_ptr->ptIndex1 + t, weight);
	//for (int i=0; i<leaf_cnt; i++)
	//{
	//	cvmSet(Weight3, 0, ptr[i]->ptIndex1 + t, weight);
	//}
}
double CAMShift::JointAngleCts(int leaf_cnt, Target* self_ptr, int t)
{
	//soft limiter for joint angles
	double dist = (abs(/*prev_angle*/self_ptr->tgMAP.theta - cvmGet(Theta, 0, (self_ptr->ptIndex1 + t))) > 100.0) ? 
		1.0 : abs(/*prev_angle*/self_ptr->tgMAP.theta - cvmGet(Theta, 0, (self_ptr->ptIndex1 + t)))/100.0;
	for (int i=0; i<leaf_cnt; i++)
	{
		dist += (abs(/*prev_angle*/ptr[i]->tgMAP.theta - cvmGet(Theta, 0, (ptr[i]->ptIndex1 + t))) > 100.0) ? 
		1.0 : abs(/*prev_angle*/ptr[i]->tgMAP.theta - cvmGet(Theta, 0, (ptr[i]->ptIndex1 + t)))/100.0;
	}
	return dist/(leaf_cnt+1);
}
double CAMShift::JointScaleCts(int leaf_cnt, Target* self_ptr, int t)
{
	//soft limiter for scale
	double dist = (abs(/*prev_scale*/self_ptr->tgMAP.r - cvmGet(R, 0, (self_ptr->ptIndex1 + t))) > 0.6) ? 
		1.0 : abs(/*prev_scale*/self_ptr->tgMAP.r - cvmGet(R, 0, (self_ptr->ptIndex1 + t)))/0.6;
	for (int i=0; i<leaf_cnt; i++)
	{
		dist += (abs(/*prev_scale*/ptr[i]->tgMAP.r - cvmGet(R, 0, (ptr[i]->ptIndex1 + t))) > 0.6) ? 
		1.0 : abs(/*prev_scale*/ptr[i]->tgMAP.r - cvmGet(R, 0, (ptr[i]->ptIndex1 + t)))/0.6;
	}
	return dist/(leaf_cnt+1);
}
double CAMShift::JointPosXCts(int leaf_cnt, Target* self_ptr, int t)
{
	double dist = (abs(/*prev_scale*/self_ptr->tgMAP.x - cvmGet(X, 0, (self_ptr->ptIndex1 + t))) > 20) ? 
		1.0 : abs(/*prev_scale*/self_ptr->tgMAP.x - cvmGet(X, 0, (self_ptr->ptIndex1 + t)))/20;
	for (int i=0; i<leaf_cnt; i++)
	{
		dist += (abs(/*prev_scale*/ptr[i]->tgMAP.x - cvmGet(X, 0, (ptr[i]->ptIndex1 + t))) > 20) ? 
		1.0 : abs(/*prev_scale*/ptr[i]->tgMAP.x - cvmGet(X, 0, (ptr[i]->ptIndex1 + t)))/20;
	}
	return dist/(leaf_cnt+1);

}
double CAMShift::JointDistMAX(int leaf_cnt, Target* self_ptr, int t)
{
	//find the sum of distances of all pairs
	double sum = 0;
	int max = (leaf_cnt +1)*leaf_cnt/2;
	for (int g=0; g<leaf_cnt; g++)
	{
		sum += ((abs(cvmGet(X, 0,(self_ptr->ptIndex1 + t))-cvmGet(X, 0,(ptr[g]->ptIndex1 + t))) < 10 
			&& abs(cvmGet(Y, 0,(self_ptr->ptIndex1 + t))-cvmGet(Y, 0,(ptr[g]->ptIndex1 + t))) < 10)  ? 1.0 : 0.0);
	}
	for (int f=0; f<leaf_cnt; f++)
	{
		for (int p=f+1; p<leaf_cnt; p++)
			sum += ((abs(cvmGet(X, 0,(ptr[f]->ptIndex1 + t))-cvmGet(X, 0,(ptr[p]->ptIndex1 + t))) < 10 
				&& abs(cvmGet(Y, 0,(ptr[f]->ptIndex1 + t))-cvmGet(Y, 0,(ptr[p]->ptIndex1 + t))) < 10)  ? 1.0 : 0.0);
	}
	return sum/max;
	
}
double CAMShift::JointPosYCts(int leaf_cnt, Target* self_ptr, int t)
{
	double dist = (abs(/*prev_scale*/self_ptr->tgMAP.y - cvmGet(Y, 0, (self_ptr->ptIndex1 + t))) > 15) ? 
		1.0 : abs(/*prev_scale*/self_ptr->tgMAP.y - cvmGet(Y, 0, (self_ptr->ptIndex1 + t)))/15;
	for (int i=0; i<leaf_cnt; i++)
	{
		dist += (abs(/*prev_scale*/ptr[i]->tgMAP.y - cvmGet(Y, 0, (ptr[i]->ptIndex1 + t))) > 15) ? 
		1.0 : abs(/*prev_scale*/ptr[i]->tgMAP.y - cvmGet(Y, 0, (ptr[i]->ptIndex1 + t)))/15;
	}
	return dist/(leaf_cnt+1);

}
double CAMShift::JointEdgeMeasure(IplImage* src, IplImage* sb, int leaf_cnt, Target* self_ptr, int t)
{
	double dist = 0;
	dist += LH->MeasureEllipseEdge/*2*/( cvmGet( X, 0, self_ptr->ptIndex1 + t ), cvmGet( Y, 0, self_ptr->ptIndex1 + t ), cvmGet( R, 0, self_ptr->ptIndex1 + t ), cvmGet(Theta, 0, self_ptr->ptIndex1 + t), sb, src );
	for (int k=0; k<leaf_cnt; k++)
	{
		dist += LH->MeasureEllipseEdge/*2*/( cvmGet( X, 0, ptr[k]->ptIndex1 + t ), cvmGet( Y, 0, ptr[k]->ptIndex1 + t ), cvmGet( R, 0, ptr[k]->ptIndex1 + t ), cvmGet(Theta, 0, ptr[k]->ptIndex1 + t), sb, src );
	}
	return dist/(leaf_cnt+1);
}

double CAMShift::JointColorMeasure(IplImage* src_Cr, IplImage* src_Cb, int leaf_cnt, Target* self_ptr, int t)
{
	cvZero(hist_mask);
	CvBox2D Jbox;

	//first do the inner mask of Joint targets
	double theta = cvmGet(Theta, 0, (self_ptr->ptIndex1 + t));
	Jbox.angle = /*-*/(theta > 180) ? theta-180 : (theta < -180) ? theta+180 : theta;
	Jbox.center.x = cvmGet(X, 0, (self_ptr->ptIndex1 + t));
	Jbox.center.y = cvmGet(Y, 0, (self_ptr->ptIndex1 + t));
	Jbox.size.width = 2*MODEL_R*cvmGet(R, 0, (self_ptr->ptIndex1 + t));
	Jbox.size.height = Jbox.size.width*ELLIPSE_RATIO;
	cvEllipseBox(hist_mask, Jbox, cvScalar(255), -1);

	for (int i=0; i<leaf_cnt; i++)
	{
		theta = cvmGet(Theta, 0, (ptr[i]->ptIndex1 + t));
		Jbox.angle = /*-*/(theta > 180) ? theta-180 : (theta < -180) ? theta+180 : theta;
		Jbox.center.x = cvmGet(X, 0, (ptr[i]->ptIndex1 + t));
		Jbox.center.y = cvmGet(Y, 0, (ptr[i]->ptIndex1 + t));
		Jbox.size.width = 2*MODEL_R*cvmGet(R, 0, (ptr[i]->ptIndex1 + t));
		Jbox.size.height = Jbox.size.width*ELLIPSE_RATIO;
		cvEllipseBox(hist_mask, Jbox, cvScalar(255), -1);
	}
	//now we got inner mask as hist_mask
	IplImage * planes[] = { src_Cr, src_Cb };
	cvCalcHist( planes, JointHist, 0, hist_mask);
	float max_val = 0.0f;
	cvGetMinMaxHistValue( JointHist, 0, &max_val, 0, 0 );
//	fileOut3 << "max_val = " << max_val << endl;
	cvConvertScale( JointHist->bins, JointHist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????
	// Normalize
	//cvNormalizeHist( ref_hist, 1 );
	cvNormalizeHist( JointHist, 1 );
//	cvGetMinMaxHistValue( current_hist, 0, &max_val, 0, 0 );
//	fileOut3 << "max_val = " << max_val << endl;
	// Note: the method CV_COMP_BHATTACHARYYA only works with normalized histograms.
	// cvNormalizeHist normalizes the histogram bins by scaling them, such that the sum of the bins becomes equal to factor.
	// double cvCompareHist( const CvHistogram* hist1, const CvHistogram* hist2, int method );
	//cvCalcBackProjectPatch
	// Compare
	double inner_distance = cvCompareHist( refHist, JointHist, CV_COMP_BHATTACHARYYA );

	
	//then do the outer mask of Joint targets
	cvZero(hist_mask);
	theta = cvmGet(Theta, 0, (self_ptr->ptIndex1 + t));
	Jbox.angle = /*-*/(theta > 180) ? theta-180 : (theta < -180) ? theta+180 : theta;
	Jbox.center.x = cvmGet(X, 0, (self_ptr->ptIndex1 + t));
	Jbox.center.y = cvmGet(Y, 0, (self_ptr->ptIndex1 + t));
	Jbox.size.width = 2*MODEL_R*cvmGet(R, 0, (self_ptr->ptIndex1 + t))*sqrt(2.0f);
	Jbox.size.height = Jbox.size.width*ELLIPSE_RATIO;
	cvEllipseBox(hist_mask, Jbox, cvScalar(255), -1);

	for (int i=0; i<leaf_cnt; i++)
	{
		theta = cvmGet(Theta, 0, (ptr[i]->ptIndex1 + t));
		Jbox.angle = /*-*/(theta > 180) ? theta-180 : (theta < -180) ? theta+180 : theta;
		Jbox.center.x = cvmGet(X, 0, (ptr[i]->ptIndex1 + t));
		Jbox.center.y = cvmGet(Y, 0, (ptr[i]->ptIndex1 + t));
		Jbox.size.width = 2*MODEL_R*cvmGet(R, 0, (ptr[i]->ptIndex1 + t))*sqrt(2.0f);
		Jbox.size.height = Jbox.size.width*ELLIPSE_RATIO;
		cvEllipseBox(hist_mask, Jbox, cvScalar(255), -1);
	}
	//now we got outer white mask as hist_mask


	//then we will cut off center part of those white ellipse blobs
	theta = cvmGet(Theta, 0, (self_ptr->ptIndex1 + t));
	Jbox.angle = /*-*/(theta > 180) ? theta-180 : (theta < -180) ? theta+180 : theta;
	Jbox.center.x = cvmGet(X, 0, (self_ptr->ptIndex1 + t));
	Jbox.center.y = cvmGet(Y, 0, (self_ptr->ptIndex1 + t));
	Jbox.size.width = 2*MODEL_R*cvmGet(R, 0, (self_ptr->ptIndex1 + t));
	Jbox.size.height = Jbox.size.width*ELLIPSE_RATIO;
	cvEllipseBox(hist_mask, Jbox, cvScalar(0), -1);

	for (int i=0; i<leaf_cnt; i++)
	{
		theta = cvmGet(Theta, 0, (ptr[i]->ptIndex1 + t));
		Jbox.angle = /*-*/(theta > 180) ? theta-180 : (theta < -180) ? theta+180 : theta;
		Jbox.center.x = cvmGet(X, 0, (ptr[i]->ptIndex1 + t));
		Jbox.center.y = cvmGet(Y, 0, (ptr[i]->ptIndex1 + t));
		Jbox.size.width = 2*MODEL_R*cvmGet(R, 0, (ptr[i]->ptIndex1 + t));
		Jbox.size.height = Jbox.size.width*ELLIPSE_RATIO;
		cvEllipseBox(hist_mask, Jbox, cvScalar(0), -1);
	}
	//now outer stripe mask is ready in hist_mask

	cvCalcHist( planes, JointHist, 0, hist_mask);
	max_val = 0.0f;
	cvGetMinMaxHistValue( JointHist, 0, &max_val, 0, 0 );
	cvConvertScale( JointHist->bins, JointHist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????
	// Normalize
	//cvNormalizeHist( ref_hist, 1 );
	cvNormalizeHist( JointHist, 1 );
	// Note: the method CV_COMP_BHATTACHARYYA only works with normalized histograms.
	// cvNormalizeHist normalizes the histogram bins by scaling them, such that the sum of the bins becomes equal to factor.
	// double cvCompareHist( const CvHistogram* hist1, const CvHistogram* hist2, int method );
	//cvCalcBackProjectPatch
	// Compare
	double outer_distance = cvCompareHist( refHist, JointHist, CV_COMP_BHATTACHARYYA );

	//return inner_distance - outer_distance + 1;
	return inner_distance - outer_distance + 1;

}

void CAMShift::Measure1(IplImage *Cr, IplImage *Cb, IplImage* src)
{
	double color_dist = 10000, texture_dist = 10000, edge_dist = 10000, angle_dist = 10000, 
		scale_dist = 10000, positionX_dist = 10000, positionY_dist = 10000, JointDistSum = 10000;
	double w_temp, maxWeight[MAX_TARGET_NUM];
	
	for (int g=0; g<MAX_TARGET_NUM; g++)
		maxWeight[g] = 0;

	//ParticleStatus ps[MAX_TARGET_NUM];
	int slot_size = (PARTICLE_NUM)/MAX_TARGET_NUM;
	int leaf_cnt=0;
	
	for (int h=0; h<MAX_TARGET_NUM; h++)
	{
		if (!targetList[h].isAlive) continue;
		if (MeasureIsDone[h]) continue;
	
		for (int d=0; d<MAX_TARGET_NUM; d++) ptr[d] = NULL;

		leaf_cnt = 0;

		for (int f=h+1; f<MAX_TARGET_NUM; f++)
		{
			Target* tmp_ptr = targetList[h].JointLeaves[f];
			if (tmp_ptr)
			{
				leaf_cnt++;
				ptr[leaf_cnt-1] = tmp_ptr;
			}
		}

		if (leaf_cnt != 0)
		{
			for (int t=0; t<targetList[h].ptNum; t++ )
			{
				//don't forget to normalize score
				color_dist = JointColorMeasure(Cr, Cb, leaf_cnt, &targetList[h], t);
				edge_dist = JointEdgeMeasure(src, sobel, leaf_cnt, &targetList[h], t);

				angle_dist = JointAngleCts(leaf_cnt, &targetList[h], t);
				scale_dist = JointScaleCts(leaf_cnt, &targetList[h], t);
				positionX_dist = JointPosXCts(leaf_cnt, &targetList[h], t);
				positionY_dist = JointPosYCts(leaf_cnt, &targetList[h], t);
				JointDistSum = JointDistMAX(leaf_cnt, &targetList[h], t);
		
				color_dist = - COLOR_WEIGHT * color_dist;
				edge_dist = - EDGE_WEIGHT * edge_dist;
				//texture_dist = - TEXTURE_WEIGHT * texture_dist;// / template_width / template_height / 128;
				angle_dist = -ANGLE_WEIGHT * angle_dist;
				scale_dist = -SCALE_WEIGHT * scale_dist;
				positionX_dist = -POSX_WEIGHT * positionX_dist;
				positionY_dist = -POSY_WEIGHT * positionY_dist;
				JointDistSum = -JOINTDIST_WEIGHT * JointDistSum;
				

				w_temp = exp( color_dist + edge_dist  /*+ texture_dist */+ angle_dist + scale_dist + positionX_dist + positionY_dist + JointDistSum);
				//fileOut2 << targetList[h].ptIndex1 + t << "-th Joint SIR: color_dist = " << color_dist << ", edge_dist = " << edge_dist << ", weight = " << w_temp << endl;
				
				if (w_temp > maxWeight[h])  //only record weight in the slot of root target, also maxWeight
				{
					maxWeight[h] = w_temp;
					targetList[h].tgMAP.index = targetList[h].ptIndex1 + t;//global index
				}
				
				//cvmSet( Weight3, 0, i, w_temp );
				JointSetWeight(w_temp, leaf_cnt, &targetList[h], t);

			}

			//label all corresponding targets as "MeasureIsDone = true"
			MeasureIsDone[h] = true;
			for (int i=0; i<leaf_cnt; i++)
			{
				MeasureIsDone[ptr[i]->tarIndex] = true;
			}
		}
		else //no leaf, h is an independent target
		{
			for( int i=targetList[h].ptIndex1; i<targetList[h].ptIndex2; i++ )
			{
				color_dist = LH->MeasureColor1/*2*/( Cr, Cb, cvmGet( X, 0, i ), cvmGet( Y, 0, i ), cvmGet( R, 0, i ), cvmGet(Theta, 0, i), refHist , src, hist_mask);
				edge_dist = LH->MeasureEllipseEdge/*2*/( cvmGet( X, 0, i ), cvmGet( Y, 0, i ), cvmGet( R, 0, i ), cvmGet(Theta, 0, i), sobel, src );
				//edge_dist += LH->MeasureRectEdge( cvmGet( CenterX, 0, i), cvmGet( CenterY, 0, i ), cvmGet( CenterR, 0, i ), cvmGet( CenterAlpha, 0, i ), edge );
				//texture_dist = LH->MeasureTexture( cvmGet( X, 0, i ), cvmGet( Y, 0, i ), cvmGet( R, 0, i ), cvmGet(Theta, 0, i), gray );
				//soft limiter for angle
				angle_dist = (abs(/*prev_angle*/targetList[h].tgMAP.theta - cvmGet(Theta, 0, i)) > 100.0) ? 
					1.0 : abs(/*prev_angle*/targetList[h].tgMAP.theta - cvmGet(Theta, 0, i))/100.0;
				//soft limiter for scale
				scale_dist = (abs(/*prev_scale*/targetList[h].tgMAP.r - cvmGet(R, 0, i)) > 0.6) ? 
					1.0 : abs(/*prev_scale*/targetList[h].tgMAP.r - cvmGet(R, 0, i))/0.6;
				positionX_dist = (abs(/*prev_scale*/targetList[h].tgMAP.x - cvmGet(X, 0, i)) > 20) ? 
					1.0 : abs(/*prev_scale*/targetList[h].tgMAP.x - cvmGet(X, 0, i))/20;
				positionY_dist = (abs(/*prev_scale*/targetList[h].tgMAP.y - cvmGet(Y, 0, i)) > 15) ? 
					1.0 : abs(/*prev_scale*/targetList[h].tgMAP.y - cvmGet(Y, 0, i))/15;

				color_dist = - COLOR_WEIGHT * color_dist;
				edge_dist = - EDGE_WEIGHT * edge_dist;
				//texture_dist = - TEXTURE_WEIGHT * texture_dist;// / template_width / template_height / 128;
				angle_dist = -ANGLE_WEIGHT * angle_dist;
				scale_dist = -SCALE_WEIGHT * scale_dist;
				positionX_dist = -POSX_WEIGHT * positionX_dist;
				positionY_dist = -POSY_WEIGHT * positionY_dist;

				w_temp = exp( color_dist + edge_dist  /*+ texture_dist */+ angle_dist + scale_dist + positionX_dist + positionY_dist);
				//fileOut2 << i << "-th single SIR: color_dist = " << color_dist << ", edge_dist = " << edge_dist << ", weight = " << w_temp << endl;
				if (w_temp > maxWeight[h]) 
				{
					maxWeight[h] = w_temp;
					targetList[h].tgMAP.index = i;//global index
				}

		
				cvmSet( Weight3, 0, i, w_temp );

			}
			//label this target as "MeasureIsDone = true"
			MeasureIsDone[h] = true;
		}
	}

	bool checkAllDone = true;
	for (int s=0; s<MAX_TARGET_NUM; s++)
		if (targetList[s].isAlive)
			checkAllDone = checkAllDone && MeasureIsDone[s];
	
	if (!checkAllDone) fileOut2 << "Measure is not all DONE! CHECK..." << endl;
		
	int global_id = -1;
	int local_id = -1;
	int tmp_id = -1;
	for (int j=0; j<MAX_TARGET_NUM; j++)
	{
		if (targetList[j].isAlive)
		{
			if (targetList[j].JointRoot == NULL) // is a root
			{
				global_id = targetList[j].tgMAP.index;
				targetList[j].tgMAP.x = cvmGet(X, 0, global_id);
				targetList[j].tgMAP.y = cvmGet(Y, 0, global_id);
				targetList[j].tgMAP.r = cvmGet(R, 0, global_id);
				targetList[j].tgMAP.theta = cvmGet(Theta, 0, global_id);

				//locate leaves' tgMAP information
				local_id = global_id - targetList[j].ptIndex1;
				for (int p=j+1; p<MAX_TARGET_NUM; p++)
				{
					Target* lfptr = targetList[j].JointLeaves[p];
					if (lfptr != NULL)
					{
						tmp_id = lfptr->tgMAP.index = lfptr->ptIndex1 + local_id;//global index
						lfptr->tgMAP.x = cvmGet(X, 0, tmp_id);
						lfptr->tgMAP.y = cvmGet(Y, 0, tmp_id);
						lfptr->tgMAP.r = cvmGet(R, 0, tmp_id);
						lfptr->tgMAP.theta = cvmGet(Theta, 0, tmp_id);
					}

				}
			}
		}
	}

}


void CAMShift::Measure(IplImage *Cr, IplImage *Cb, IplImage* src)
{
	double color_dist = 10000, haircolor_dist = 10000, texture_dist = 10000, edge_dist = 10000, angle_dist = 10000, 
		scale_dist = 10000, positionX_dist = 10000, positionY_dist = 10000;
	double w_temp, maxWeight[MAX_TARGET_NUM];
	
	for (int g=0; g<MAX_TARGET_NUM; g++)
		maxWeight[g] = 0;

	int slot_size = (PARTICLE_NUM)/MAX_TARGET_NUM;

	for( int i=0; i<PARTICLE_NUM; i++ )
	{
		int target_id = i/slot_size;

		if (!targetList[target_id].isAlive) continue;

		color_dist = LH->MeasureColor1/*2*/( Cr, Cb, cvmGet( X, 0, i ), cvmGet( Y, 0, i ), cvmGet( R, 0, i ), cvmGet(Theta, 0, i), refHist , src, hist_mask);
		haircolor_dist = LH->MeasureHairColor( Cr, Cb, cvmGet( X, 0, i ), cvmGet( Y, 0, i ), cvmGet( R, 0, i ), refHairHist, src, hist_mask);
		edge_dist = edge4Segment[i].total = LH->MeasureEllipseEdge4( cvmGet( X, 0, i ), cvmGet( Y, 0, i ), cvmGet( R, 0, i ), cvmGet(Theta, 0, i), sobel, src, 
			edge4Segment[i].upperright, edge4Segment[i].upperleft, edge4Segment[i].downright, edge4Segment[i].downleft, cam_id);
		//edge_dist += LH->MeasureRectEdge( cvmGet( CenterX, 0, i), cvmGet( CenterY, 0, i ), cvmGet( CenterR, 0, i ), cvmGet( CenterAlpha, 0, i ), edge );
		texture_dist = LH->MeasureTexture( cvmGet( X, 0, i ), cvmGet( Y, 0, i ), cvmGet( R, 0, i ), cvmGet(Theta, 0, i), gray, target_id );
		//soft limiter for angle
		angle_dist = (abs(/*prev_angle*/targetList[target_id].tgMAP.theta - cvmGet(Theta, 0, i)) > 100.0) ? 
			1.0 : abs(/*prev_angle*/targetList[target_id].tgMAP.theta - cvmGet(Theta, 0, i))/100.0;
		//soft limiter for scale
		scale_dist = (abs(/*prev_scale*/targetList[target_id].tgMAP.r - cvmGet(R, 0, i)) > 0.6) ? 
			1.0 : abs(/*prev_scale*/targetList[target_id].tgMAP.r - cvmGet(R, 0, i))/0.6;
		positionX_dist = (abs(/*prev_scale*/targetList[target_id].tgMAP.x - cvmGet(X, 0, i)) > 100) ? 100.0 : 
			(abs(/*prev_scale*/targetList[target_id].tgMAP.x - cvmGet(X, 0, i)) > 20) ? 1.0 : 
			abs(/*prev_scale*/targetList[target_id].tgMAP.x - cvmGet(X, 0, i))/20;
		positionY_dist = (abs(/*prev_scale*/targetList[target_id].tgMAP.y - cvmGet(Y, 0, i)) > 66) ? 100.0 : 
			(abs(/*prev_scale*/targetList[target_id].tgMAP.y - cvmGet(Y, 0, i)) > 15) ? 1.0 : 
			abs(/*prev_scale*/targetList[target_id].tgMAP.y - cvmGet(Y, 0, i))/15;
		
		if (positionX_dist == 100.0 || positionY_dist == 100.0)
			fileOut2 << "problem index = " << i << endl; 

		color_dist = - COLOR_WEIGHT * color_dist;
		haircolor_dist = - HAIR_WEIGHT * haircolor_dist;
		edge_dist = - EDGE_WEIGHT * edge_dist;
		texture_dist = - TEXTURE_WEIGHT * texture_dist;// / template_width / template_height / 128;
		
		if (targetList[target_id].isInGroup)
		{
			angle_dist = -OCCLUSION_CTS_FACTOR * ANGLE_WEIGHT * angle_dist;
			scale_dist = -OCCLUSION_CTS_FACTOR * SCALE_WEIGHT * scale_dist;
			positionX_dist = -OCCLUSION_CTS_FACTOR * POSX_WEIGHT * positionX_dist;
			positionY_dist = -OCCLUSION_CTS_FACTOR * POSY_WEIGHT * positionY_dist;
		}
		else
		{
			angle_dist = -ANGLE_WEIGHT * angle_dist;
			scale_dist = -SCALE_WEIGHT * scale_dist;
			positionX_dist = -POSX_WEIGHT * positionX_dist;
			positionY_dist = -POSY_WEIGHT * positionY_dist;
		}

		w_temp = exp( color_dist + haircolor_dist + edge_dist  + texture_dist + angle_dist + scale_dist + positionX_dist + positionY_dist);
		//fileOut2 << i << "-th SIR: color_dist = " << color_dist << ", edge_dist = " << edge_dist << ", weight = " << w_temp << endl;
		if (w_temp > maxWeight[target_id]) 
		{
			maxWeight[target_id] = w_temp;
			targetList[target_id].tgMAP.index = i;
		}

		
		cvmSet( Weight3, 0, i, w_temp );

	}

//	if (0/*detect_flag*/)
//	for( int i=0; i<ADD_PARTICLE_NUM; i++ ){
//		color_dist = LH->MeasureColor2( Cr, Cb, cvmGet( add_X, 0, i ), cvmGet( add_Y, 0, i ), cvmGet( add_R, 0, i ), cvmGet(add_Theta, 0, i), refHist , src, hist_mask);
//		edge_dist = LH->MeasureEllipseEdge2( cvmGet( add_X, 0, i ), cvmGet( add_Y, 0, i ), cvmGet( add_R, 0, i ), cvmGet(add_Theta, 0, i), sobel, src );
//		//edge_dist += LH->MeasureRectEdge( cvmGet( CenterX, 0, i), cvmGet( CenterY, 0, i ), cvmGet( CenterR, 0, i ), cvmGet( CenterAlpha, 0, i ), edge );
//		//texture_dist = LH->MeasureTexture( cvmGet( add_X, 0, i ), cvmGet( add_Y, 0, i ), cvmGet( add_R, 0, i ), cvmGet(add_Theta, 0, i), gray );
//		//soft limiter for angle
//		angle_dist = (abs(prev_angle - cvmGet(add_Theta, 0, i)) > 100.0) ? 1.0 : abs(prev_angle - cvmGet(add_Theta, 0, i))/100.0;//1.0f - exp(-abs(prev_angle - cvmGet(Theta, 0, i))*0.05f);
//		//soft limiter for scale
//		scale_dist = (abs(prev_scale - cvmGet(add_R, 0, i)) > 0.6) ? 1.0 : abs(prev_scale - cvmGet(add_R, 0, i))/0.6;
//
//
//		color_dist = - COLOR_WEIGHT * color_dist;
//		edge_dist = - EDGE_WEIGHT * edge_dist;
//		//texture_dist = - TEXTURE_WEIGHT * texture_dist;// / template_width / template_height / 128;
//		angle_dist = -ANGLE_WEIGHT * angle_dist;
//		scale_dist = -SCALE_WEIGHT * scale_dist;
//
//		w_temp = exp( color_dist + edge_dist  /*+ texture_dist */ + angle_dist + scale_dist);
////		fileOut2 << "SIS: color_dist = " << color_dist << ", edge_dist = " << edge_dist << ", weight = " << w_temp << endl;
//		if (w_temp > maxWeight) 
//		{
//			maxWeight = w_temp;
//			ps.index = i+PARTICLE_NUM;
//		}
//
//		
//		cvmSet( Weight3, 0, i+PARTICLE_NUM, w_temp );
//
//	}

//	fileOut2 << "MAP index = " << ps.index << endl;

	int tmp_id = -1;
	for (int j=0; j<MAX_TARGET_NUM; j++)
		if (targetList[j].isAlive)
		{
			tmp_id = targetList[j].tgMAP.index;
			targetList[j].tgMAP.x = cvmGet(X, 0, tmp_id);
			targetList[j].tgMAP.y = cvmGet(Y, 0, tmp_id);
			targetList[j].tgMAP.r = cvmGet(R, 0, tmp_id);
			targetList[j].tgMAP.theta = cvmGet(Theta, 0, tmp_id);
		}

	//else
	//{
	//	//in Weight2
	//	ps.x = cvmGet(add_X, 0, ps.index-PARTICLE_NUM);
	//	ps.y = cvmGet(add_Y, 0, ps.index-PARTICLE_NUM);
	//	ps.r = cvmGet(add_R, 0, ps.index-PARTICLE_NUM);
	//	ps.theta = cvmGet(add_Theta, 0, ps.index-PARTICLE_NUM);

	//}
	return;

}

void CAMShift::SobelEdge1()
{
	// get Sobel edge = (dx^2 + dy^2)^.5;
	// out: sobel
	// in: gray

	cvSmooth( gray, sobel, CV_GAUSSIAN, 5, 5 );
	cvSobel( sobel, dx, 1, 0, 3 );
	cvSobel( sobel, dy, 0, 1, 3 );
	cvMul( dx, dx, dx, 1 );		//cvPow( dx, dx, 2 );
	cvMul( dy, dy, dy, 1 );		//cvPow( dy, dy, 2 );
	cvAdd( dx, dy, edgetemp );
	cvPow( edgetemp, edgetemp, 0.5 );

//	cvMinMaxLoc( edgeImage, &min_val, &max_val, &min_pt, &max_pt, NULL );

	cvConvertScale( edgetemp, edgetemp, 0.005, 0 );	// scale & threshold	0.005
	cvThreshold( edgetemp, edgetemp, 0.25, 255, CV_THRESH_TOZERO );
	cvCvtScale( edgetemp, sobel, 255 );
	cvThreshold( sobel, sobel, 50, 255, CV_THRESH_TOZERO );
#ifdef _DEBUG
	cvNamedWindow("prior_sobel",1);
	cvShowImage("prior_sobel", sobel);
#endif

	cvAbsDiff(gray, p_gray, moving_blob);
	cvThreshold(moving_blob, moving_blob, 10, 255, CV_THRESH_BINARY);
	cvErode(moving_blob, moving_blob, smlErosion);
	cvDilate(moving_blob, moving_blob, bigErosion);
	//cvOr(s_mv_blob, prev_moving_blob, s_mv_blob);//???
	cvRectangle( moving_blob, cvPoint(0,0), cvPoint(gray->width,gray->height), CV_RGB(0, 0, 0), 2, 8, 0 );
#ifdef _DEBUG
	cvNamedWindow("moving_blob",1);
	cvShowImage("moving_blob",moving_blob);
#endif

	cvZero(temp_sobel);
	cvCopy(sobel, temp_sobel, moving_blob);

#ifdef _DEBUG
	cvNamedWindow("temp_sobel",1);
	cvShowImage("temp_sobel",temp_sobel);
#endif
	cvAddWeighted(sobel, 0.5, temp_sobel, 1.0, 0, sobel);

//#ifdef _DEBUG
	cvNamedWindow("Mixed Sobel", 1);
	cvShowImage("Mixed Sobel", sobel);
//#endif
	cvCopy(gray, p_gray);
//	cvCopy(moving_blob, prev_moving_blob);  //?	

	//cvReleaseImage( & graysmooth );
	//cvReleaseImage( & dx );
	//cvReleaseImage( & dy );
	//cvReleaseImage( & edgetemp );
}

void CAMShift::SobelEdge()
{

	// get Sobel edge = (dx^2 + dy^2)^.5;
	// out: sobel
	// in: gray

	cvResize(gray, sml_sobel,CV_INTER_LINEAR);
	cvResize(gray, sml_gray,CV_INTER_LINEAR);
	cvSmooth( sml_gray, sml_sobel, CV_GAUSSIAN, 5, 5 );
	cvSobel( sml_sobel/*graysmooth*/, s_dx, 1, 0, 3 );
	cvSobel( sml_sobel/*graysmooth*/, s_dy, 0, 1, 3 );
	cvMul( s_dx, s_dx, s_dx, 1 );		//cvPow( dx, dx, 2 );
	cvMul( s_dy, s_dy, s_dy, 1 );		//cvPow( dy, dy, 2 );
	cvAdd( s_dx, s_dy, sml_temp );
	cvPow( sml_temp, sml_temp, 0.5 );

//	cvMinMaxLoc( edgeImage, &min_val, &max_val, &min_pt, &max_pt, NULL );

	cvConvertScale( sml_temp, sml_temp, 0.005, 0 );	// scale & threshold	0.005
	cvThreshold( sml_temp, sml_temp, 0.25, 255, CV_THRESH_TOZERO );
	cvCvtScale( sml_temp, sml_sobel, 255 );
	cvThreshold( sml_sobel, sml_sobel, 50, 255, CV_THRESH_TOZERO );
#ifdef _DEBUG
	cvNamedWindow("sml_sobel",1);
	cvShowImage("sml_sobel",sml_sobel);
#endif

	cvAbsDiff(sml_gray, prev_gray, s_mv_blob);

#ifdef _DEBUG
	cvNamedWindow("diff image",1);
	cvShowImage("diff image",s_mv_blob);
#endif
	cvThreshold(s_mv_blob, s_mv_blob, 10, 255, CV_THRESH_BINARY);
	cvErode(s_mv_blob, s_mv_blob, smlErosion);
	cvDilate(s_mv_blob, s_mv_blob, bigErosion);
	//cvOr(s_mv_blob, prev_moving_blob, s_mv_blob);//???
	cvRectangle( s_mv_blob, cvPoint(0,0), cvPoint(sml_gray->width,sml_gray->height), CV_RGB(0, 0, 0), 2, 8, 0 );
#ifdef _DEBUG
	cvNamedWindow("s_mv_blob",1);
	cvShowImage("s_mv_blob",s_mv_blob);
#endif
	cvZero(s_temp_sobel);
	cvCopy(sml_sobel, s_temp_sobel, s_mv_blob);
#ifdef _DEBUG
	cvNamedWindow("s_temp_sobel",1);
	cvShowImage("s_temp_sobel",s_temp_sobel);
#endif
	cvAddWeighted(sml_sobel, 0.5, s_temp_sobel, 1.0, 0, sml_sobel);
#ifdef _DEBUG
	cvNamedWindow("Mixed Sobel", 1);
	cvShowImage("Mixed Sobel", sml_sobel);
#endif
	cvResize(sml_sobel, sobel, CV_INTER_LINEAR);
#ifdef _DEBUG
	cvNamedWindow("Sobel", 1);
	cvShowImage("Sobel", sobel);
#endif
	cvCopy(sml_gray, prev_gray);
	cvCopy(s_mv_blob, prev_moving_blob);	

	//cvReleaseImage( & graysmooth );
	//cvReleaseImage( & dx );
	//cvReleaseImage( & dy );
	//cvReleaseImage( & edgetemp );
}
void CAMShift::drawModelPattern(IplImage * src, double x, double y, double r, double theta, CvScalar color, int thickness)
{
	int i_index = cvRound( theta/360*circle_edge_sample_number );
	for (int i=0; i<circle_edge_sample_number; i++)
	{
		if (i < circle_edge_sample_number-1)
			cvLine(src, cvPoint(x + LH->getRotatedX(LH->R[i]*r, 0, i+i_index-10),y /*+*/- LH->getRotatedY(LH->R[i]*r, 0, i+i_index-10)),
				cvPoint(x + LH->getRotatedX(LH->R[i+1]*r, 0, i+i_index-10+1),y /*+*/- LH->getRotatedY(LH->R[i+1]*r, 0, i+i_index-10+1)), color, thickness);
		else // i = circle_edge_sample_number-1, last one
			cvLine(src, cvPoint(x + LH->getRotatedX(LH->R[i]*r, 0, i+i_index-10),y /*+*/- LH->getRotatedY(LH->R[i]*r, 0, i+i_index-10)),
				cvPoint(x + LH->getRotatedX(LH->R[0]*r, 0, 0+i_index-10),y /*+*/- LH->getRotatedY(LH->R[0]*r, 0, 0+i_index-10)), color, thickness);
	}
}
void CAMShift::drawMAPModelPattern(IplImage * src, double x, double y, double r, double theta, CvScalar color, int thickness)
{
	int i_index = cvRound( theta/360*circle_edge_sample_number*10 );
	for (int i=0; i<circle_edge_sample_number*10; i++)
	{
		if (i < circle_edge_sample_number*10-1)
			cvLine(src, cvPoint(x + LH->MAPgetRotatedX(LH->MAP_R[i]*r, 0, i+i_index-10*10),y /*+*/- LH->MAPgetRotatedY(LH->MAP_R[i]*r, 0, i+i_index-10*10)),
				cvPoint(x + LH->MAPgetRotatedX(LH->MAP_R[i+1]*r, 0, i+i_index-10*10+1),y /*+*/- LH->MAPgetRotatedY(LH->MAP_R[i+1]*r, 0, i+i_index-10*10+1)), color, thickness);
		else // i = circle_edge_sample_number*10-1, last one
			cvLine(src, cvPoint(x + LH->MAPgetRotatedX(LH->MAP_R[i]*r, 0, i+i_index-10*10),y /*+*/- LH->MAPgetRotatedY(LH->MAP_R[i]*r, 0, i+i_index-10*10)),
				cvPoint(x + LH->MAPgetRotatedX(LH->MAP_R[0]*r, 0, 0+i_index-10*10),y /*+*/- LH->MAPgetRotatedY(LH->MAP_R[0]*r, 0, 0+i_index-10*10)), color, thickness);
	}
}
void CAMShift::reverseAngleSelector(ParticleStatus ps)
{
	//fileOut2 << "............................................................\n";
	//fileOut2 << "prev map ps.theta = " << ps.theta << endl;
	//for (int k=0; k<PARTICLE_NUM; k++)
	//	fileOut2 << "original angle = " << cvmGet(Theta, 0, k) << endl;
	//if (detect_flag)
	//for (int k=0; k<ADD_PARTICLE_NUM; k++)
	//	fileOut2 << "original (add)angle = " << cvmGet(add_Theta, 0, k) << endl;


	double angle;
	for (int i=0; i<PARTICLE_NUM; i++)
	{
		angle = cvmGet(Theta, 0, i);
		for (; angle - ps.theta > 180.0; )
			angle = angle - 180.0;
		for (; ps.theta - angle > 180.0; )
			angle = angle + 180.0;
		for (; angle < -180.0; angle+=180.0)
			;
		for (; angle > 180.0; angle-=180.0)
			;
		cvmSet(Theta, 0, i, angle);

		//if (ps.theta < 90.0 && ps.theta > -90.0)
		//{
		//	if (angle - ps.theta > 90.0) 
		//		cvmSet(Theta, 0, i, angle-180.0);
		//	else if (ps.theta - angle > 90) 
		//		cvmSet(Theta, 0, i, angle+180.0);
		//	else ;
		//}
		//else if (ps.theta > 90.0)
		//{
		//	if (ps.theta - angle > 90.0) 
		//		if (angle > 0) cvmSet(Theta, 0, i, angle-180.0);
		//		else if (angle < 0 && angle > -90.0) cvmSet(Theta, 0, i, angle+180.0);
		//		else if (angle < -90.0)
		//			if (ps.theta - (angle+180.0) < 90.0) cvmSet(Theta, 0, i, angle+180.0);
		//			else ;
		//}
		//else if (ps.theta < -90.0) 
		//{
		//	if (angle - ps.theta > 90.0)
		//		if (angle < 0) cvmSet(Theta, 0, i, angle+180.0);
		//		else if (angle > 0 && angle < 90.0) cvmSet(Theta, 0, i, angle-180.0);
		//		else if (angle > 90.0)
		//			if ((angle-180.0) - ps.theta < 90.0) cvmSet(Theta, 0, i, angle-180.0);
		//			else ;
		//}
		//else ;
	} 

	//if (detect_flag)
	//for (int i=0; i<ADD_PARTICLE_NUM; i++)
	//{
	//	angle = cvmGet(add_Theta, 0, i);
	//	for (; angle - ps.theta > 180.0; )
	//		angle = angle - 180.0;
	//	for (; ps.theta - angle > 180.0; )
	//		angle = angle + 180.0;
	//	for (; angle < -180.0; angle+=180.0)
	//		;
	//	for (; angle > 180.0; angle-=180.0)
	//		;
	//	cvmSet(add_Theta, 0, i, angle);

	//} 
 


	for (int i=0; i<PARTICLE_NUM; i++)
	{
		angle = cvmGet(Theta, 0, i);
		if (ps.theta < 90.0 && ps.theta > -90.0)
		{
			if (angle - ps.theta > 90.0) 
				cvmSet(Theta, 0, i, angle-180.0);
			else if (ps.theta - angle > 90) 
				cvmSet(Theta, 0, i, angle+180.0);
			else ;
		}
		else if (ps.theta > 90.0)
		{
			if (ps.theta - angle > 90.0) 
				if (angle > 0) cvmSet(Theta, 0, i, angle-180.0);
				else if (angle < 0 && angle > -90.0) cvmSet(Theta, 0, i, angle+180.0);
				else if (angle < -90.0)
					if (ps.theta - (angle+180.0) < 90.0) cvmSet(Theta, 0, i, angle+180.0);
					else ;
		}
		else if (ps.theta < -90.0) 
		{
			if (angle - ps.theta > 90.0)
				if (angle < 0) cvmSet(Theta, 0, i, angle+180.0);
				else if (angle > 0 && angle < 90.0) cvmSet(Theta, 0, i, angle-180.0);
				else if (angle > 90.0)
					if ((angle-180.0) - ps.theta < 90.0) cvmSet(Theta, 0, i, angle-180.0);
					else ;
		}
		else ;
	}

	//if (detect_flag)
	//for (int i=0; i<ADD_PARTICLE_NUM; i++)
	//{
	//	angle = cvmGet(add_Theta, 0, i);
	//	if (ps.theta < 90.0 && ps.theta > -90.0)
	//	{
	//		if (angle - ps.theta > 90.0) 
	//			cvmSet(add_Theta, 0, i, angle-180.0);
	//		else if (ps.theta - angle > 90) 
	//			cvmSet(add_Theta, 0, i, angle+180.0);
	//		else ;
	//	}
	//	else if (ps.theta > 90.0)
	//	{
	//		if (ps.theta - angle > 90.0) 
	//			if (angle > 0) cvmSet(add_Theta, 0, i, angle-180.0);
	//			else if (angle < 0 && angle > -90.0) cvmSet(add_Theta, 0, i, angle+180.0);
	//			else if (angle < -90.0)
	//				if (ps.theta - (angle+180.0) < 90.0) cvmSet(add_Theta, 0, i, angle+180.0);
	//				else ;
	//	}
	//	else if (ps.theta < -90.0) 
	//	{
	//		if (angle - ps.theta > 90.0)
	//			if (angle < 0) cvmSet(add_Theta, 0, i, angle+180.0);
	//			else if (angle > 0 && angle < 90.0) cvmSet(add_Theta, 0, i, angle-180.0);
	//			else if (angle > 90.0)
	//				if ((angle-180.0) - ps.theta < 90.0) cvmSet(add_Theta, 0, i, angle-180.0);
	//				else ;
	//	}
	//	else ;
	//}

}
void CAMShift::DrawResult(/*ParticleStatus ps, */IplImage* image)
{
//	int id = ps.index;
	

	CvBox2D box;
	double slot_size = (PARTICLE_NUM)/MAX_TARGET_NUM;
	//wrong!!->//畫畫之前角度加負號
	for (int i=0; i<PARTICLE_NUM; i++)
	{
		int id = i/slot_size;
		if (!targetList[id].isAlive) continue;
		box.angle = 90.0;/*-*///cvmGet(Theta, 0, i);
		box.center.x = cvmGet(X, 0, i);
		box.center.y = cvmGet(Y, 0, i);
		box.size.width = 2*MODEL_R*cvmGet(R, 0, i);//2*cvmGet(R, 0, i);
		box.size.height = box.size.width*ELLIPSE_RATIO;//box.size.width*LH->getEllipse_ratio();
		//cvEllipseBox(image, box, CV_RGB(100, 200, 100)); 

//		drawModelPattern(image, cvmGet(X, 0, i), cvmGet(Y, 0, i), cvmGet(R, 0, i), cvmGet(Theta, 0, i), CV_RGB(100, 200, 100), 2);
	}

	if (detect_flag)
	for (int i=0; i<ADD_PARTICLE_NUM; i++)
	{
		//box.angle = /*-*/cvmGet(Theta, 0, i);
		//box.center.x = cvmGet(X, 0, i);
		//box.center.y = cvmGet(Y, 0, i);
		//box.size.width = 2*cvmGet(R, 0, i);
		//box.size.height = box.size.width*LH->getEllipse_ratio();
		//cvEllipseBox(image, box, CV_RGB(100, 200, 100)); 

//		drawModelPattern(image, cvmGet(add_X, 0, i), cvmGet(add_Y, 0, i), cvmGet(add_R, 0, i), cvmGet(add_Theta, 0, i), CV_RGB(50, 100, 200), 2);
	}

	//track_box.angle = /*-*/ps.theta;//cvmGet(Theta, 0, id);
	//track_box.center.x = ps.x;//cvmGet(X, 0, id);
	//track_box.center.y = ps.y;//cvmGet(Y, 0, id);
	//track_box.size.width = 2*ps.r;//cvmGet(R, 0, id);
	//track_box.size.height = track_box.size.width*LH->getEllipse_ratio();
	//cvEllipseBox( image, track_box, CV_RGB(255,50,50), 5, 8, 0 );



	//MAP drawing
	for (int j=0; j<MAX_TARGET_NUM; j++)
		if (targetList[j].isAlive)
		{
			int map_i = targetList[j].tgMAP.index;
			double cur_X = 0.6*targetList[j].tgMAP.x + 0.3*targetList[j].preStatus[0].x + 0.1*targetList[j].preStatus[1].x;//cvmGet(X, 0, map_i);
			double cur_Y = 0.6*targetList[j].tgMAP.y + 0.3*targetList[j].preStatus[0].y + 0.1*targetList[j].preStatus[1].y;//cvmGet(Y, 0, map_i);
			double cur_R = 0.6*targetList[j].tgMAP.r + 0.3*targetList[j].preStatus[0].r + 0.1*targetList[j].preStatus[1].r;//cvmGet(R, 0, map_i);
			double cur_Theta = 0.6*targetList[j].tgMAP.theta + 0.3*targetList[j].preStatus[0].theta + 0.1*targetList[j].preStatus[1].theta;//cvmGet(Theta, 0, map_i);	
	

			CvBox2D ell_box;
			ell_box.angle = cur_Theta;
			ell_box.center.x = cur_X;
			ell_box.center.y = cur_Y;
			ell_box.size.height = 2*MODEL_R*ELLIPSE_RATIO*cur_R;
			ell_box.size.width = 2*MODEL_R*cur_R;
			//if (iteration_cnt > NOM_FOR_INIT_STATE_USING_CAMSHIFT+3)
				//drawMAPModelPattern(image, mapX, mapY, 0.9*mapR, mapTheta, CV_RGB(255, 50, 50), 8);
				cvEllipseBox(image, ell_box, CV_RGB(255, 50+j*50, 50), 3);	

				targetLines[j].setByImgLoc_precise(cvPoint(cur_X, ((cur_Y < 60) && (cam_id == CAM1)) ? cur_Y - 12 : cur_Y));
			
			//////////bool show3DPos = targetLines[j].getPosFromZ(targetHeight[j], targetPos[j]);
			//////////targetInfo[j].LocateAndMeasureInfo(&targetLines[j], targetGndLines[j], true, targetHeight[j], image_Cr, image_Cb, hist_mask);
			//////////if (show3DPos)
			//////////{
			//////////	char number[20];
			//////////	sprintf(number, "(%d,%d,%d)", (int)targetPos[j].x, (int)targetPos[j].y, (int)targetPos[j].z);
			//////////	cvPutText(image, number, cvPoint(targetList[j].tgMAP.x - 20, targetList[j].tgMAP.y - 40), &orderFont, CV_RGB(0,250,250));
			//////////	sprintf(number, "(%d,%d)", targetGndLines[j].imageLoc.x, targetGndLines[j].imageLoc.y);
			//////////	cvPutText(image, number, cvPoint(targetList[j].tgMAP.x - 10, targetList[j].tgMAP.y + 40), &orderFont, CV_RGB(50,250,100));
			//////////	for (int f=0; f<NUM_OF_BLOCKS; f++)
			//////////	{
			//////////		if (!targetInfo[j].IsBodySeen[f]) continue;

			//////////		cvRectangle(image, cvPoint(targetInfo[j].Blocks[f].x, targetInfo[j].Blocks[f].y), 
			//////////			cvPoint(targetInfo[j].Blocks[f].x+targetInfo[j].Blocks[f].width, targetInfo[j].Blocks[f].y+targetInfo[j].Blocks[f].height),
			//////////			CV_RGB(100,200,20), 2);
			//////////	}

			//////////}


			////////////bool show3DPos = targetLines[j].getPosFromZ(targetHeight[j], targetPos[j]);
			////////////targetInfo[j].LocateAndMeasureInfo(&targetLines[j], targetGndLines[j], true, targetHeight[j], image_Cr, image_Cb, hist_mask);
			////////////if (show3DPos)
			////////////{
			////////////	char number[20];
			////////////	sprintf(number, "(%d,%d,%d)", (int)targetPos[j].x, (int)targetPos[j].y, (int)targetPos[j].z);

			////////////	if (cam_id == CAM1)
			////////////		cvPutText(image, number, cvPoint(targetList[j].tgMAP.x - 20, targetList[j].tgMAP.y - 40), &orderFont, CV_RGB(50,150,50));
			////////////	else //cam_id == CAM2
			////////////		cvPutText(image, number, cvPoint(targetList[j].tgMAP.x - 20, targetList[j].tgMAP.y + 50/*- 40*/ //This is for CAM2 setting
			////////////		), &orderFont, CV_RGB(50,150,50));

			////////////	sprintf(number, "(%d,%d)", targetGndLines[j].imageLoc.x, targetGndLines[j].imageLoc.y);

			////////////	if (cam_id == CAM1)
			////////////		cvPutText(image, number, cvPoint(targetList[j].tgMAP.x - 10, targetList[j].tgMAP.y + 40), &orderFont, CV_RGB(150,250,0));
			////////////	else //cam_id == CAM2
			////////////		cvPutText(image, number, cvPoint(targetList[j].tgMAP.x - 10, targetList[j].tgMAP.y + 40), &orderFont, CV_RGB(150,250,0));
			////////////	
			////////////	for (int f=0; f<NUM_OF_BLOCKS; f++)
			////////////	{
			////////////		if (!targetInfo[j].IsBodySeen[f]) continue;

			////////////		cvRectangle(image, cvPoint(targetInfo[j].Blocks[f].x, targetInfo[j].Blocks[f].y), 
			////////////			cvPoint(targetInfo[j].Blocks[f].x+targetInfo[j].Blocks[f].width, targetInfo[j].Blocks[f].y+targetInfo[j].Blocks[f].height),
			////////////			CV_RGB(100,20,20), 1);
			////////////	}

			////////////}

		}
		for (int g=0; g<MAX_GROUP_NUM; g++)
		{
			if (!order[g][0]) continue;
			for (int w=0; w<leafctr[g]+1; w++)
			{
				char number[4];
				sprintf(number,"%d", w+1);
				cvPutText( image, number, cvPoint(order[g][w]->tgMAP.x, order[g][w]->tgMAP.y), &orderFont, CV_RGB(0,0,250) );
			}
		}




//	else
//	{
//		double cur_X = cvmGet(add_X, 0, id-PARTICLE_NUM);
//		double cur_Y = cvmGet(add_Y, 0, id-PARTICLE_NUM);
//		double cur_R = cvmGet(add_R, 0, id-PARTICLE_NUM);
//		double cur_Theta = cvmGet(add_Theta, 0, id-PARTICLE_NUM);
//
//		mapX = cur_X*0.6 + Pre_Status[1].x*0.3 + Pre_Status[0].x*0.1;
//		if (fabs(mapX - pr_mapX) > 10)
//			;
//		else mapX = pr_mapX;
//
//		pr_mapX = mapX;
//
//		mapY = cur_Y*0.6 + Pre_Status[1].y*0.3 + Pre_Status[0].y*0.1;
//		if (fabs(mapY - pr_mapY) > 10)
//			;
//		else mapY = pr_mapY;
//
//		pr_mapY = mapY;
//		
//		mapR = cur_R*0.6 + Pre_Status[1].r*0.3 + Pre_Status[0].r*0.1;
//		if (fabs(mapR - pr_mapR) > 0.1)
//			;
//		else mapR = pr_mapR;
//
//		pr_mapR = mapR;
//		
//		//if (iteration_cnt > NOM_FOR_INIT_STATE_USING_CAMSHIFT+3)
//		{
//			while (cur_Theta - mapTheta > 180) cur_Theta = cur_Theta - 360.0;
//			while (cur_Theta - mapTheta < -180) cur_Theta = cur_Theta + 360.0;
//		}
//		mapTheta = cur_Theta*0.6 + Pre_Status[1].theta*0.3 + Pre_Status[0].theta*0.1;
//		if (fabs(mapTheta - pr_mapTheta) > 2)
//			;
//		else mapTheta = pr_mapTheta;
//
//		pr_mapTheta = mapTheta;
//
//		//fileOut2 << "cur_Theta = " << cur_Theta << endl;
//		//fileOut2 << "Pre_Status[1].theta = " << Pre_Status[1].theta << endl;
//		//fileOut2 << "Pre_Status[0].theta = " << Pre_Status[0].theta << endl;
//		CvBox2D ell_box;
//		ell_box.angle = mapTheta;
//		ell_box.center.x = mapX;
//		ell_box.center.y = mapY;
//		ell_box.size.height = 2*MODEL_R*ELLIPSE_RATIO;
//		ell_box.size.width = 2*MODEL_R;
//		//if (iteration_cnt > NOM_FOR_INIT_STATE_USING_CAMSHIFT+3)
//			//drawMAPModelPattern(image, mapX, mapY, 0.9*mapR, mapTheta, CV_RGB(255, 50, 50), 8);
//			cvEllipseBox(image, ell_box, CV_RGB(255, 50, 50), 3);
//
////		mPM->getVisionTranslate()->showCurrentParticleOnImage( image );
//
//		Pre_Status[0] = Pre_Status[1];
//		Pre_Status[1].x = cur_X;
//		Pre_Status[1].y = cur_Y;
//		Pre_Status[1].r = cur_R;
//		Pre_Status[1].theta = cur_Theta;
//
////		drawMAPModelPattern(image, cvmGet(add_X, 0, id-PARTICLE_NUM), cvmGet(add_Y, 0, id-PARTICLE_NUM), cvmGet(add_R, 0, id-PARTICLE_NUM), cvmGet(add_Theta, 0, id-PARTICLE_NUM), CV_RGB(255, 50, 50), 8);
////drawModelPattern(image, cvmGet(add_X, 0, id-PARTICLE_NUM), cvmGet(add_Y, 0, id-PARTICLE_NUM), cvmGet(add_R, 0, id-PARTICLE_NUM), cvmGet(add_Theta, 0, id-PARTICLE_NUM), CV_RGB(255, 50, 50), 8);
//	}
	iteration_cnt++;

}
void CAMShift::DrawIntegratedObservation(int obs_Id, IplImage* image, int TargetID, int level)
{
	char LvChar;
	if (level == 4) LvChar = 'S';
	else if (level == 3) LvChar = 'H';
	else if (level == 2) LvChar = 'M';
	else if (level == 1) LvChar = 'L';
	else 
		LvChar = 'N';

	bool show3DPos = targetLines[obs_Id].getPosFromZ(targetHeight[obs_Id], targetPos[obs_Id]);
	targetInfo[obs_Id].LocateAndMeasureInfo(&targetLines[obs_Id], targetGndLines[obs_Id], true, targetHeight[obs_Id], image_Cr, image_Cb, hist_mask);


	if (cam_id == CAM1)
	{
		char number[20];
		sprintf(number, "%c", LvChar);
		sprintf(number+1, "(%d,%d,%d)", (int)targetPos[obs_Id].x, (int)targetPos[obs_Id].y, (int)targetPos[obs_Id].z);
		cvPutText(image, number, cvPoint(targetList[obs_Id].tgMAP.x - 20, targetList[obs_Id].tgMAP.y + 50/*- 40*/), &orderFont, CV_RGB(0,250,250));

		//////sprintf(number, "(%d,%d)", targetGndLines[obs_Id].imageLoc.x, targetGndLines[obs_Id].imageLoc.y);
		//////cvPutText(image, number, cvPoint(targetList[obs_Id].tgMAP.x - 10, targetList[obs_Id].tgMAP.y + 40), &orderFont, CV_RGB(50,250,100));

		for (int f=0; f<NUM_OF_BLOCKS; f++)
		{
			if (!targetInfo[obs_Id].IsBodySeen[f]) continue;
			
			if (!isBlockOcc[obs_Id][f])
				cvRectangle(image, cvPoint(targetInfo[obs_Id].Blocks[f].x, targetInfo[obs_Id].Blocks[f].y), 
					cvPoint(targetInfo[obs_Id].Blocks[f].x+targetInfo[obs_Id].Blocks[f].width, targetInfo[obs_Id].Blocks[f].y+targetInfo[obs_Id].Blocks[f].height),
					CV_RGB((128+70*TargetID)%255, (254*3-70*TargetID)%255, (70*TargetID)%255), 2);
			else
				cvRectangle(image, cvPoint(targetInfo[obs_Id].Blocks[f].x, targetInfo[obs_Id].Blocks[f].y), 
					cvPoint(targetInfo[obs_Id].Blocks[f].x+targetInfo[obs_Id].Blocks[f].width, targetInfo[obs_Id].Blocks[f].y+targetInfo[obs_Id].Blocks[f].height),
					CV_RGB(((128+70*TargetID)%255)/2, ((254*3-70*TargetID)%255)/2, ((70*TargetID)%255)/2), 2);

		}
	}
	else if (cam_id == CAM2)
	{
		char number[20];
		sprintf(number, "%c", LvChar);
		sprintf(number+1, "(%d,%d,%d)", (int)targetPos[obs_Id].x, (int)targetPos[obs_Id].y, (int)targetPos[obs_Id].z);
		cvPutText(image, number, cvPoint(targetList[obs_Id].tgMAP.x - 20, targetList[obs_Id].tgMAP.y + 50), &orderFont, CV_RGB(0,250,250));

		//////sprintf(number, "(%d,%d)", targetGndLines[obs_Id].imageLoc.x, targetGndLines[obs_Id].imageLoc.y);
		//////cvPutText(image, number, cvPoint(targetList[obs_Id].tgMAP.x - 10, targetList[obs_Id].tgMAP.y + 40), &orderFont, CV_RGB(50,250,100));

		for (int f=0; f<NUM_OF_BLOCKS; f++)
		{
			if (!targetInfo[obs_Id].IsBodySeen[f]) continue;
			
			if (!isBlockOcc[obs_Id][f])
				cvRectangle(image, cvPoint(targetInfo[obs_Id].Blocks[f].x, targetInfo[obs_Id].Blocks[f].y), 
					cvPoint(targetInfo[obs_Id].Blocks[f].x+targetInfo[obs_Id].Blocks[f].width, targetInfo[obs_Id].Blocks[f].y+targetInfo[obs_Id].Blocks[f].height),
					CV_RGB((128+70*TargetID)%255, (254*3-70*TargetID)%255, (70*TargetID)%255), 2);
			else
				cvRectangle(image, cvPoint(targetInfo[obs_Id].Blocks[f].x, targetInfo[obs_Id].Blocks[f].y), 
					cvPoint(targetInfo[obs_Id].Blocks[f].x+targetInfo[obs_Id].Blocks[f].width, targetInfo[obs_Id].Blocks[f].y+targetInfo[obs_Id].Blocks[f].height),
					CV_RGB(((128+70*TargetID)%255)/2, ((254*3-70*TargetID)%255)/2, ((70*TargetID)%255)/2), 2);
		}
	}
	else if (cam_id == CAM3)
	{
		char number[20];
		sprintf(number, "%c", LvChar);
		sprintf(number+1, "(%d,%d,%d)", (int)targetPos[obs_Id].x, (int)targetPos[obs_Id].y, (int)targetPos[obs_Id].z);
		cvPutText(image, number, cvPoint(targetList[obs_Id].tgMAP.x - 20, targetList[obs_Id].tgMAP.y - 40), &orderFont, CV_RGB(0,250,250));

		//////sprintf(number, "(%d,%d)", targetGndLines[obs_Id].imageLoc.x, targetGndLines[obs_Id].imageLoc.y);
		//////cvPutText(image, number, cvPoint(targetList[obs_Id].tgMAP.x - 10, targetList[obs_Id].tgMAP.y + 40), &orderFont, CV_RGB(50,250,100));

		for (int f=0; f<NUM_OF_BLOCKS; f++)
		{
			if (!targetInfo[obs_Id].IsBodySeen[f]) continue;
			
			if (!isBlockOcc[obs_Id][f])
				cvRectangle(image, cvPoint(targetInfo[obs_Id].Blocks[f].x, targetInfo[obs_Id].Blocks[f].y), 
					cvPoint(targetInfo[obs_Id].Blocks[f].x+targetInfo[obs_Id].Blocks[f].width, targetInfo[obs_Id].Blocks[f].y+targetInfo[obs_Id].Blocks[f].height),
					CV_RGB((128+70*TargetID)%255, (254*3-70*TargetID)%255, (70*TargetID)%255), 2);
			else
				cvRectangle(image, cvPoint(targetInfo[obs_Id].Blocks[f].x, targetInfo[obs_Id].Blocks[f].y), 
					cvPoint(targetInfo[obs_Id].Blocks[f].x+targetInfo[obs_Id].Blocks[f].width, targetInfo[obs_Id].Blocks[f].y+targetInfo[obs_Id].Blocks[f].height),
					CV_RGB(((128+70*TargetID)%255)/2, ((254*3-70*TargetID)%255)/2, ((70*TargetID)%255)/2), 2);
		}
	}
	else
		;//nothing to do
}

void CAMShift::DrawSingleObservation(int obs_Id, IplImage* image)
{
			bool show3DPos = targetLines[obs_Id].getPosFromZ(targetHeight[obs_Id], targetPos[obs_Id]);
			targetInfo[obs_Id].LocateAndMeasureInfo(&targetLines[obs_Id], targetGndLines[obs_Id], true, targetHeight[obs_Id], image_Cr, image_Cb, hist_mask);
			if (show3DPos)
			{
				char number[20];
				sprintf(number, "(%d,%d,%d)", (int)targetPos[obs_Id].x, (int)targetPos[obs_Id].y, (int)targetPos[obs_Id].z);

				if (cam_id == CAM1)
					cvPutText(image, number, cvPoint(targetList[obs_Id].tgMAP.x - 20, targetList[obs_Id].tgMAP.y + 50/*- 40*/), &orderFont, CV_RGB(50,150,50));
				else //cam_id == CAM2
					cvPutText(image, number, cvPoint(targetList[obs_Id].tgMAP.x - 20, targetList[obs_Id].tgMAP.y + 50/*- 40*/ //This is for CAM2 setting
					), &orderFont, CV_RGB(50,150,50));

				//////sprintf(number, "(%d,%d)", targetGndLines[obs_Id].imageLoc.x, targetGndLines[obs_Id].imageLoc.y);

				//////if (cam_id == CAM1)
				//////	cvPutText(image, number, cvPoint(targetList[obs_Id].tgMAP.x - 10, targetList[obs_Id].tgMAP.y + 40), &orderFont, CV_RGB(150,250,0));
				//////else //cam_id == CAM2
				//////	cvPutText(image, number, cvPoint(targetList[obs_Id].tgMAP.x - 10, targetList[obs_Id].tgMAP.y + 40), &orderFont, CV_RGB(150,250,0));
				
				for (int f=0; f<NUM_OF_BLOCKS; f++)
				{
					if (!targetInfo[obs_Id].IsBodySeen[f]) continue;

					if (isBlockOcc[obs_Id][f])
						cvRectangle(image, cvPoint(targetInfo[obs_Id].Blocks[f].x, targetInfo[obs_Id].Blocks[f].y), 
						cvPoint(targetInfo[obs_Id].Blocks[f].x+targetInfo[obs_Id].Blocks[f].width, targetInfo[obs_Id].Blocks[f].y+targetInfo[obs_Id].Blocks[f].height),
						CV_RGB(100,100,100), 2);
					else
						cvRectangle(image, cvPoint(targetInfo[obs_Id].Blocks[f].x, targetInfo[obs_Id].Blocks[f].y), 
						cvPoint(targetInfo[obs_Id].Blocks[f].x+targetInfo[obs_Id].Blocks[f].width, targetInfo[obs_Id].Blocks[f].y+targetInfo[obs_Id].Blocks[f].height),
						CV_RGB(100,20,20), 1);

				}

			}

}


void CAMShift::setUpSupportingClass(IplImage *image)
{
	//LH 創立於此時
	cvCvtColor(image, temp_sobel, CV_BGR2GRAY);
	cvResize(temp_sobel, prev_gray, CV_INTER_LINEAR);
	if (LH != NULL) 
	{
		delete LH;
		LH = NULL;
	}
	LH = new likelihood(max(track_box.size.height/track_box.size.width, track_box.size.width/track_box.size.height));
	//LH->setEllipse_ratio(max(track_box.size.height/track_box.size.width, track_box.size.width/track_box.size.height));
	LH->setTexture_cnt(0);

	if (frontalFaceTest != NULL)
	{
		delete frontalFaceTest;
		frontalFaceTest = NULL;
	}
	frontalFaceTest = new face_detect(0, cam_id);

	if (profileFaceTest != NULL)
	{
		delete profileFaceTest;
		profileFaceTest = NULL;
	}
	profileFaceTest = new face_detect(1, cam_id);
	
	if (!frontalFaceTest->loadCascadeOk)
		fileOut2 << "frontalFace is not loaded" << endl;
	if (!profileFaceTest->loadCascadeOk)
		fileOut2 << "profileFace is not loaded" << endl;

}
void CAMShift::normalizeWeight()
{
	double slot_size = (PARTICLE_NUM)/MAX_TARGET_NUM, sum=0, avg=0, tmp_w=0, min=1, max=0, tmp=0;

	for (int h=0; h<MAX_TARGET_NUM; h++)
	{
		if (targetList[h].isAlive)
		{

			//dst(i,j)=(src(i,j)-min(src))*(b'-a')/(max(src)-min(src)) + a'


			max = 0;
			min = 1;
			for (int g=targetList[h].ptIndex1; g<targetList[h].ptIndex2; g++)
			{
				tmp = cvmGet(Weight3, 0, g);
				if (tmp > max) max = tmp;
				if (tmp < min) min = tmp;
			}
			
			if (min != max)
				for (int k=targetList[h].ptIndex1; k<targetList[h].ptIndex2; k++)
				{
					tmp_w = (cvmGet(Weight3, 0, k) - min)*1.0/(max-min) + 0.0;
					
					tmp_w = (tmp_w > 0.5/*0.7*/) ? tmp_w : 0;

					cvmSet(Weight3, 0, k, tmp_w);
				}

			sum = 0;
			for (int g=targetList[h].ptIndex1; g<targetList[h].ptIndex2; g++)
			{
				tmp = cvmGet(Weight3, 0, g);
				sum += tmp;
			}
			avg = sum/slot_size;

			//fileOut2 << "sum = " << sum << ", avg = " << avg << endl;

			for (int k=targetList[h].ptIndex1; k<targetList[h].ptIndex2; k++)
			{
				tmp_w = cvmGet(Weight3, 0, k)/sum;
				cvmSet(Weight3, 0, k, tmp_w);
			}

		}
	}
	for (int j=0; j<PARTICLE_NUM; j++)
		;//fileOut2 << "normalized weight(" << j << ") = " << cvmGet(Weight3, 0, j) << endl;
	return;



////////////////past code//////////////////
	double min_val, max_val;
	CvPoint min_pt, max_pt;


	cvMinMaxLoc( Weight3, &min_val, &max_val, &min_pt, &max_pt, NULL );

	if ( min_val != max_val )
	{
		// CV_MINMAX can not process all equivalent weights
		cvNormalize( Weight3, Weight3, 1, 0, CV_MINMAX, NULL );		// normalize to [0,1]

		// remove small weight < weight_bios
		cvSubS( Weight3, cvScalar(cvAvg( Weight3 ).val[0]*1.5), Weight3 );

		cvMaxS( Weight3, 0, Weight3 );

	}

	cvNormalize( Weight3, Weight3, 1, 0, CV_L1, NULL );		// sum = 1

	//for (int j=0; j<PARTICLE_NUM + ADD_PARTICLE_NUM; j++)
	//fileOut2 << "normalized weight(" << j << ") = " << cvmGet(Weight3, 0, j) << endl;
}


void CAMShift::deleteTarget(int index)
{
	//for leaf target
	while (targetList[index].JointRoot != NULL)
	{
		targetList[index].JointRoot->JointLeaves[index] = NULL;
		targetList[index].JointRoot = targetList[index].JointRoot->JointRoot;
	}

	//for root target
	for (int h=index+1; h<MAX_TARGET_NUM; h++)
	{
		while (targetList[index].JointLeaves[h] != NULL)
		{
			targetList[index].JointLeaves[h]->JointRoot = NULL;
			targetList[index].JointLeaves[h] = NULL;
		}
	}

	targetList[index].delTarget();
}
bool CAMShift::targetAllAlive(int k)
{
	if (!targetList[k].isAlive) 
		return false; 
	else if (k == MAX_TARGET_NUM-1) 
		return targetList[k].isAlive; 
	else 
		return targetAllAlive(k+1);
}
bool CAMShift::targetAllDead(int k)
{
	if (targetList[k].isAlive) 
		return false; 
	else if (k == MAX_TARGET_NUM-1) 
		return !targetList[k].isAlive; 
	else 
		return targetAllDead(k+1);
}
int CAMShift::targeAliveNum(int k)
{
	if (k == MAX_TARGET_NUM-1)
		return (targetList[k].isAlive ? 1 : 0);
	else   
		return (targetList[k].isAlive ? 1+targeAliveNum(k+1) : targeAliveNum(k+1)); 
}

int orderCompare_multiFeature (const void * a, const void * b)
{
	int totalOrderScore;
	int indexA = (*(CAMShift::Target**)a)->tgMAP.index;
	int indexB = (*(CAMShift::Target**)b)->tgMAP.index;
	int idA = (*(CAMShift::Target**)a)->tarIndex;
	int idB = (*(CAMShift::Target**)b)->tarIndex;
	CAMShift* mom = (*(CAMShift::Target**)a)->MomSelf;
	IplImage* texA = mom->LH->ref_texture[idA];
	IplImage* prevtexA = mom->LH->prev_ref_texture[idA];
	IplImage* texB = mom->LH->ref_texture[idB];
	IplImage* prevtexB = mom->LH->prev_ref_texture[idB];
	cvAbsDiff(texA, prevtexA, sad);
	CvScalar sum1 = cvSum(sad);
	cvAbsDiff(texB, prevtexB, sad);
	CvScalar sum2 = cvSum(sad);
	double tex_diff = sum1.val[0]/TEMPLATE_WIDTH/TEMPLATE_HEIGHT - sum2.val[0]/TEMPLATE_WIDTH/TEMPLATE_HEIGHT;
	totalOrderScore = (tex_diff > 0) ? 1 : -1;
	totalOrderScore = (abs(tex_diff) > 10) ? totalOrderScore*3 : totalOrderScore*2;
	totalOrderScore += ((mom->edge4Segment[indexA].total > mom->edge4Segment[indexB].total) ? 1 : -1);
	Position3D groundA, groundB;
	mom->targetGndLines[idA].getPosFromZ(0, groundA);
	mom->targetGndLines[idB].getPosFromZ(0, groundB);
	int camNo = mom->cam_id;
	if (camNo == CAM1) totalOrderScore += (MAGNITUDE((groundA-position(0, 0, 220))) > MAGNITUDE((groundB-position(0,0,220))) ? 5 : -5);
	if (camNo == CAM2) totalOrderScore += (MAGNITUDE((groundA-position(412, 0, 220))) > MAGNITUDE((groundB-position(412,0,220))) ? 5 : -5);

	if (abs(totalOrderScore) >= 8) orderFix = true;
	return totalOrderScore;
}
void CAMShift::DepthOrderCheck()
{
	if (orderFix) return;


	for (int d=0; d<MAX_GROUP_NUM; d++)
	{
		leafctr[d] = 0;
		for (int h=0; h<MAX_TARGET_NUM; h++)
			order[d][h] = NULL;
	}

	int groupctr=0;
	for (int k=0; k<MAX_TARGET_NUM; k++)
	{
		if (!targetList[k].isAlive) continue;
		
		if (targetList[k].JointRoot == NULL) //a root
		{
			leafctr[groupctr]=0;
			for (int j=k+1; j<MAX_TARGET_NUM; j++)
			{
				if (targetList[k].JointLeaves[j] != NULL) // j is a leaf
				{
					leafctr[groupctr]++;
					if (leafctr[groupctr] == 1)
					{
						order[groupctr][0] = &targetList[k];
						order[groupctr][1] = &targetList[j];
					}
					else
					{
						order[groupctr][leafctr[groupctr]] = &targetList[j];
					}
					
				}
			}
			if (leafctr[groupctr] != 0) // one group is constructed
			{
				groupctr++;
			}

		}
	}
	//now we have the group list

	for (int t=0; t<MAX_GROUP_NUM; t++)
	{
		if (order[t][0])
			qsort(order[t], leafctr[t]+1, sizeof(CAMShift::Target*), orderCompare_multiFeature);
	}
}
void CAMShift::BodyOccCheck()
{
	for (int d=0; d<MAX_GROUP_NUM; d++)
	{
		if (!order[d][0]) continue;
		//fileOut2 << "Group " << d << ":" << endl << "order ";
		for (int h=0; h<leafctr[d]+1; h++)
		{
			//fileOut2 << order[d][h]->tarIndex << ", ";
			if (h == 0)
			{
				continue;
			}
			else if (h == 1)
			{			
				double bk_h = targetInfo[order[d][h]->tarIndex].Blocks[0].height;
				double gndY = targetGndLines[(order[d][0]->tarIndex)].imageLoc.y;
				double hdY = targetLines[(order[d][0]->tarIndex)].imageLoc.y - 30;
				for (int s=0; s<NUM_OF_BLOCKS; s++) 
				{
					int bk_center_y = targetInfo[order[d][h]->tarIndex].Blocks[s].y + 0.5*bk_h;
					if ((bk_center_y > hdY) && (bk_center_y < gndY)) isBlockOcc[order[d][h]->tarIndex][s] = true;
				}
			}
			else 
			{
				for (int s=0; s<NUM_OF_BLOCKS; s++) 
				{
					isBlockOcc[order[d][h]->tarIndex][s] = true;
				}
			}
		
		}
		//fileOut2 << endl;
	}
	


}

bool CAMShift::JointStatusChange()
{
	//must be very simple and quick
	int JointCnt = 0;
	for (int k=0; k<MAX_TARGET_NUM; k++)
	{
		if (!targetList[k].isAlive) continue;

		for (int g=k+1; g<MAX_TARGET_NUM; g++)
		{
			if (!targetList[g].isAlive) continue;

			if (abs(targetList[k].tgMAP.x - targetList[g].tgMAP.x) < JOINT_X_DIST && abs(targetList[k].tgMAP.y - targetList[g].tgMAP.y) < JOINT_Y_DIST)
			{
				JointCnt++;
			}
		}
	}

	if (JointCnt == PrevJointCnt)
		return false;
	else
	{
		PrevJointCnt = JointCnt;
		return true;
	}

}
void CAMShift::JointStatusCheck()
{
//沒run separation! root ptr link都不會斷 難怪會越追越不準，因為最後大家都會joint在一起了 這樣子particle數目就會太少，結果不穩
//兩個解決辦法：
//             1.每次都run separation check，把脫離的target的相關link都斷光光
//               (應該很慢，因為跑一大堆ptr應該不是什麼快的事)
//             2.每次都run initialization 把所有link切光，反正connection check都會幫我把該接的全部link接好
//      
//             以上兩種方法都可以加上簡單的geometry判斷來filter掉過度頻繁的check，例如target沒動根本不用來check什麼 

	for (int k=0; k<MAX_TARGET_NUM; k++)
	{
		if (!targetList[k].isAlive) continue;

		for (int h=0; h<NUM_OF_BLOCKS; h++)
		{
			isBlockOcc[k][h] = false;
		}
	}


	if (JointStatusChange())
	{

		orderFix = false;
		for (int k=0; k<MAX_TARGET_NUM; k++)
		{
			if (!targetList[k].isAlive) continue;

			targetList[k].isInGroup = false;
			targetList[k].JointRoot = NULL;
			for (int h=0; h<MAX_TARGET_NUM; h++)
			{
				targetList[k].JointLeaves[h] = NULL;
			}
		}


		//Connection Check
		for (int k=0; k<MAX_TARGET_NUM; k++)
		{
			if (!targetList[k].isAlive) continue;

			for (int j=k+1; j<MAX_TARGET_NUM; j++)
			{
				if (!targetList[j].isAlive) continue;

				if (abs(targetList[k].tgMAP.x - targetList[j].tgMAP.x) < JOINT_X_DIST && abs(targetList[k].tgMAP.y - targetList[j].tgMAP.y) < JOINT_Y_DIST)
				{
					targetList[k].isInGroup = targetList[j].isInGroup = true;
					if (targetList[k].JointRoot != NULL)  //k has a root, k is a leaf.
					{
						targetList[k].JointRoot->JointLeaves[j] = &targetList[j];
						targetList[j].JointRoot = targetList[k].JointRoot;
					}
					else //k is the root
					{
						targetList[k].JointLeaves[j] = &targetList[j];
						targetList[j].JointRoot = &targetList[k];
					}
				
				}

			}
		}
	}
	//////////}

	////////Separation Check
	//////for (int k=0; k<MAX_TARGET_NUM; k++)
	//////{
	//////	//for each node run the separation check over the root and all the leaves
	//////	if (targetList[k].)
	//////	for (int j=k+1; j<MAX_TARGET_NUM; j++)
	//////	{
	//////		if (abs(targetList[k].tgMAP.x - targetList[j].tgMAP.x) < 60 && abs(targetList[k].tgMAP.y - targetList[j].tgMAP.y) < 80)
	//////		{
	//////			if (targetList[k].JointRoot != NULL)  //k has a root, k is a leaf.
	//////			{
	//////				targetList[k].JointRoot->JointLeaves[j] = &targetList[j];
	//////				targetList[j].JointRoot = targetList[k].JointRoot;
	//////			}
	//////			else //k is the root
	//////			{
	//////				targetList[k].JointLeaves[j] = &targetList[j];
	//////				targetList[j].JointRoot = &targetList[k];
	//////			}
	//////			
	//////		}

	//////	}
	//////}

	//prepare the work list for Measure process 
	for (int g=0; g<MAX_TARGET_NUM; g++)
	{
		MeasureIsDone[g] = false;
		DepthOrderDone[g] = false;
	}
}


CamIterationReturn CAMShift::iteration(IplImage *image, IplImage* &fgmask, CvRect ROI)
{

		cvCvtColor(image, image_YCrCb, CV_BGR2YCrCb);
		cvSplit(image_YCrCb, image_Y, image_Cr, image_Cb, NULL );
		if (init_flag) 
		{
			//setUpSupportingClass(image);
			init_flag = false;
		}

		//detect objects
		ParticleStatus detectResult = detectObject_importanceSampling(image);
		int newTargetId = NOT_DETECTED;

		if (detectResult.index == DETECT_NEW)
		{
			for (int k=0; k<MAX_TARGET_NUM; k++)
			{
				if (!targetList[k].isAlive)
				{
					targetList[k] = Target(detectResult, k);
					newTargetId = k;
					break;
				}
			}
		}
		
		if (targetAllDead(0))
		{
			CamIterationReturn Return;
			Return.val = 0;
			return Return;
		}
		

		//if (iteration_cnt < NOM_FOR_INIT_STATE_USING_CAMSHIFT)
		//{
		//	iteration_cnt++;
		//	CamIterationReturn result = CAMShift::iteration_init(image, fgmask, ROI);
		//	

		//	return result;
		//}

		//reject cases with too small area
		//if (track_box.size.height + track_box.size.width < 50)
		//{
		//	CamIterationReturn result;
		//	result.val = -1;
		//	return result;
		//}
		



		cvCvtColor(image, gray, CV_BGR2GRAY);
		SobelEdge1();
//		cvCvtColor(image, image_YCrCb, CV_BGR2YCrCb);
//		cvSplit(image_YCrCb, image_Y, image_Cr, image_Cb, NULL );
		//cvNamedWindow("input_Cr", 1);
		//cvShowImage("input_Cr", image_Cr);
		//cvNamedWindow("input_Cb", 1);
		//cvShowImage("input_Cb", image_Cb);
#ifdef _DEBUG
		cvNamedWindow("gray", 1);
		cvShowImage("gray", gray);
#endif

		if( mLosingCount > 0 ) // start counting iteration
			mIterationCount++;

		if (newTargetId != NOT_DETECTED) //detect new target
		{
			//LH->Backproj1(image_Cr, image_Cb, add_X, add_Y, add_R, add_Theta, image, refHist, detect_flag, cvPoint(prev_best.x,prev_best.y));
			targetList[newTargetId].SIS_flag = false;
			LH->ref_tex_cnt[newTargetId] = 0;
			targetList[newTargetId].MomSelf = this;
			Resampling(targetList[newTargetId]);
			targetList[newTargetId].tgMAP.index = TRACKED;
			
			if (cam_id == CAM1) targetHeight[newTargetId] = 155;//145;//170;//150;
			else //cam_id == CAM2
				targetHeight[newTargetId] = 155;
		}
		else 
		{
			//LH->Backproj1(image_Cr, image_Cb, add_new_X, add_new_Y, add_new_R ,add_new_Theta ,image ,refHist , detect_flag, cvPoint(prev_best.x, prev_best.y));
			detect_flag = false;
			Resampling1();
		}
		

		Predict();
	
		JointStatusCheck();

		//reverseAngleSelector(prev_best);
		for (int h=0; h<MAX_TARGET_NUM; h++)
		{
			if (!targetList[h].isAlive) continue;

			LH->UpdateRefTexture(targetList[h].tgMAP.x, targetList[h].tgMAP.y, targetList[h].tgMAP.r, gray, h);
		}

		Measure(image_Cr, image_Cb, image);

		DepthOrderCheck();
		BodyOccCheck();
		
		for (int h=0; h<MAX_TARGET_NUM; h++)
		{
			if (!targetList[h].isAlive) continue;

			targetList[h].preStatus[1] = targetList[h].preStatus[0];
			targetList[h].preStatus[0] = targetList[h].tgMAP;
		}
		
		//double color_quality = 0, edge_quality = 0, hair_quality = 0;
		for (int h=0; h<MAX_TARGET_NUM; h++)
		{
			if (targetList[h].isAlive)
			{
				//////color_quality = LH->MeasureColor1/*2*/(image_Cr, image_Cb, targetList[h].tgMAP.x, 
				//////	targetList[h].tgMAP.y, targetList[h].tgMAP.r, targetList[h].tgMAP.theta, refHist, image, hist_mask);
				//////edge_quality = LH->MeasureEllipseEdge(targetList[h].tgMAP.x, targetList[h].tgMAP.y, targetList[h].tgMAP.r, 
				//////	targetList[h].tgMAP.theta, sobel, image); 
				//////hair_quality = LH->MeasureHairColor( image_Cr, image_Cb, targetList[h].tgMAP.x, targetList[h].tgMAP.y, targetList[h].tgMAP.r, refHairHist, image, hist_mask);

				//////fileOut2 << "CAM" << cam_id+1 << " target" << h << " : color_quality = " << color_quality << ", edge_quality = " << edge_quality << ", hair_quality = " << hair_quality << endl; 
				//////if (((cam_id == CAM1) && (1.55 < color_quality) && (0.55 < edge_quality) && (hair_quality > 0.65))
				//////	|| ((cam_id == CAM2) && (1.1 < color_quality) && (0.5 < edge_quality) && (hair_quality > 0.5)) || (1.8 < color_quality))
				//////{
				//////	targetLosingCnt[h]++;
				//////	if (targetLosingCnt[h] > 30)
				//////	{
				//////		//targetList[h].isAlive = false;
				//////		fileOut2 << "delete!" << endl;
				//////		deleteTarget(h);
				//////		targetLosingCnt[h] = 0;

				//////	}
				//////}
				//////else
				//////	targetLosingCnt[h] = 0;
				
				
				//losingTrackDetector(h, image);
				losingTrackDetectorByFaceDetector(h, image);

			}
		}
		
		DrawResult(image);
		normalizeWeight();


		//sendControlSignal(timeDiff);	
		
		CamIterationReturn Return;
		Return.targetCenter = cvPointFrom32f(track_box.center);
		Return.val = 0;

		return Return;
}

void CAMShift::losingTrackDetector(int h, IplImage* image)
{
	double color_quality = 0, edge_quality = 0, hair_quality = 0;		
	color_quality = LH->MeasureColor1/*2*/(image_Cr, image_Cb, targetList[h].tgMAP.x, 
		targetList[h].tgMAP.y, targetList[h].tgMAP.r, targetList[h].tgMAP.theta, refHist, image, hist_mask);
	edge_quality = LH->MeasureEllipseEdge(targetList[h].tgMAP.x, targetList[h].tgMAP.y, targetList[h].tgMAP.r, 
		targetList[h].tgMAP.theta, sobel, image); 
	hair_quality = LH->MeasureHairColor( image_Cr, image_Cb, targetList[h].tgMAP.x, targetList[h].tgMAP.y, targetList[h].tgMAP.r, refHairHist, image, hist_mask);

	//fileOut2 << "CAM" << cam_id+1 << " target" << h << " : color_quality = " << color_quality << ", edge_quality = " << edge_quality << ", hair_quality = " << hair_quality << endl; 
	if (((cam_id == CAM1) && (1.55 < color_quality) && (0.55 < edge_quality) && (hair_quality > 0.65))
		|| ((cam_id == CAM2) && (1.1 < color_quality) && (0.5 < edge_quality) && (hair_quality > 0.5)) || (1.8 < color_quality))
	{
		targetLosingCnt[h]++;
		if (targetLosingCnt[h] > 30/*80*/)
		{
			//targetList[h].isAlive = false;
			//fileOut2 << "delete!" << endl;
			deleteTarget(h);
			targetLosingCnt[h] = 0;
		}
	}
	else
		targetLosingCnt[h] = 0;
}

bool CAMShift::faceDetector(int x, int y, IplImage* image)
{
	//fileOut2 << "bdd box test ok" << endl;

	CvRect faceTestRgn;
	faceTestRgn.x = x - FACETEST_WIDTH/2 < 0 ? 0 : x - FACETEST_WIDTH/2;
	faceTestRgn.y = y - 2*FACETEST_HEIGHT/3 < 0 ? 0 : y - 2*FACETEST_HEIGHT/3;
	faceTestRgn.width = faceTestRgn.x + FACETEST_WIDTH > width ? width - faceTestRgn.x : FACETEST_WIDTH;
	faceTestRgn.height = faceTestRgn.y + FACETEST_HEIGHT > height ? height - faceTestRgn.y : FACETEST_HEIGHT;
	cvSetImageROI(image, faceTestRgn);
	IplImage* subImg = cvCreateImage(cvSize(faceTestRgn.width, faceTestRgn.height), 8, 3);
	cvCopy(image, subImg, 0);
	cvNamedWindow("subImg", 1);
	cvShowImage("subImg", subImg);
	cvResetImageROI(image);//important!! don't forget.
	frontalFaceTest->detect_and_draw(subImg, -1, cam_id);
	profileFaceTest->detect_and_draw(subImg, -1, cam_id);

	if (frontalFaceTest->detect || profileFaceTest->detect)
	{
		//fileOut2 << "face test ok" << endl;
		cvReleaseImage(&subImg);//important!! don't forget.
		return true;
	}
	
	cvReleaseImage(&subImg);//important!! don't forget.
	return false;
}

void CAMShift::losingTrackDetectorByFaceDetector(int h, IplImage* image)
{
	double color_quality = 0, edge_quality = 0, hair_quality = 0;		
	color_quality = LH->MeasureColor1/*2*/(image_Cr, image_Cb, targetList[h].tgMAP.x, 
		targetList[h].tgMAP.y, targetList[h].tgMAP.r, targetList[h].tgMAP.theta, refHist, image, hist_mask);
	edge_quality = LH->MeasureEllipseEdge(targetList[h].tgMAP.x, targetList[h].tgMAP.y, targetList[h].tgMAP.r, 
		targetList[h].tgMAP.theta, sobel, image); 
	hair_quality = LH->MeasureHairColor( image_Cr, image_Cb, targetList[h].tgMAP.x, targetList[h].tgMAP.y, targetList[h].tgMAP.r, refHairHist, image, hist_mask);

	//fileOut2 << "CAM" << cam_id+1 << " target" << h << " : color_quality = " << color_quality << ", edge_quality = " << edge_quality << ", hair_quality = " << hair_quality << endl; 
	if (((cam_id == CAM1) && (1.55 < color_quality) && (0.55 < edge_quality) && (hair_quality > 0.65))
		|| ((cam_id == CAM2) && (1.1 < color_quality) && (0.5 < edge_quality) && (hair_quality > 0.5)) || (1.8 < color_quality))
	{
		targetLosingCnt[h]++;

		if (targetLosingCnt[h] > 25)
		{
			char msg[30];
			sprintf(msg, "LTD by FD. TID: %d, times: %d \n", h, targetLosingCnt[h]); 
			(*mPresentedControlString) += string(msg);

			if(faceDetector(targetList[h].tgMAP.x + rand()%10 - 5 , targetList[h].tgMAP.y + rand()%10 - 5 , image))
			{
				targetLosingCnt[h] = 0;
				//set MAP as face detection result
			}
		}

		if (targetLosingCnt[h] > 35/*80*/)
		{
			//targetList[h].isAlive = false;
			//fileOut2 << "delete!" << endl;
			deleteTarget(h);
			targetLosingCnt[h] = 0;
		}
	}
	else
		targetLosingCnt[h] = 0;
}

ParticleStatus CAMShift::detectObject_importanceSampling(IplImage * image)
{
	IplImage * planes[] = {image_Cr, image_Cb};
	cvCalcBackProject( planes, extend_mask, refHist );
	cvEqualizeHist(extend_mask, extend_mask);
	cvThreshold(extend_mask, extend_mask, SKIN_COLOR_THD-25/*CAM2 setting*/, 255, CV_THRESH_TOZERO);//CAM2 setting
	cvNamedWindow("skin color detect");
	cvShowImage("skin color detect", extend_mask);

	cvResize(extend_mask, sml_backproj, CV_INTER_LINEAR);
	cvErode(sml_backproj, sml_backproj, smlErosion, 1);
	cvDilate(sml_backproj, sml_backproj, CAM1_detect_dilation/*CAM2 setting*/, 1);
	cvNamedWindow("skin color blob");
	cvShowImage("skin color blob", sml_backproj);

	int c, count ;
	int value, r_sum, c_sum;

	cvClearMemStorage(detect_storage);
    cvFindContours( sml_backproj, detect_storage, &detect_contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ) ;
    CvSeq* first_contour = detect_contour;
    //count = 255 ;

	double bestArea = 0, curArea = 0, maxScore = -10000, curScore = 0;
	CvPoint bestLoc = cvPoint(10000,10000);
	CvPoint pt;
	//search_contours = NULL;
	bool find_new = true;

	int contour_num = 0;
	for( ; detect_contour != 0 ; detect_contour = detect_contour->h_next )
    {
		find_new = true;
		contour_num++;
		curArea = abs(cvContourArea(detect_contour));
		
		if (curArea < 170/*SKIN_BLOB_AREA_THD*/) continue;

		int sumx = 0, sumy = 0;
		for (int k=0; k < 8; k++)
		{
			pt = *(CvPoint*)cvGetSeqElem(detect_contour, k*(detect_contour->total/8));
			sumx += pt.x;
			sumy += pt.y;
		}
		sumx/=8;
		sumy/=8;
		pt = cvPoint(sumx, sumy);
		
		for (int k=0; k<MAX_TARGET_NUM; k++ )
		{
			if (targetList[k].isAlive == true)
			{
				if ( abs(targetList[k].tgMAP.x-pt.x*2) > 80/*100*/ || abs(targetList[k].tgMAP.y-pt.y*2) > 60/*80*/ )
					find_new = find_new ? true : false;
				else
				{
					find_new = false;
					//targetList[k].tgMAP.x = pt.x*2;
					//targetList[k].tgMAP.y = pt.y*2;
				}
				if (pt.y > 100) find_new = false;
			}
		}
		if (find_new)
		{
			//fileOut2 << "find_new" << endl;
			ParticleStatus detect_obj;
			CvScalar color = cvScalar(rand()%150+100);
			cvZero(sml_backproj);
			//cvDrawContours( sml_CntCmp, detect_contour, color, color, -1, CV_FILLED, 8 ) ;
			cvDrawContours( sml_backproj, detect_contour, color, color, -1, CV_FILLED, 8 ) ;

			double r = sqrt(curArea*4.0/(CV_PI*ELLIPSE_RATIO))/MODEL_R;
	
			CvMoments m;
		
			cvMoments( sml_backproj, &m, 0/*1*/ ) ;
			cvGetSpatialMoment( &m, 1, 0);
			cvGetSpatialMoment( &m, 0, 1);
			int centerX = m.m10/m.m00;
			int centerY = m.m01/m.m00;

			cvGetCentralMoment( &m, 1, 1 ) ;
			cvGetCentralMoment( &m, 2, 0 ) ;
			cvGetCentralMoment( &m, 0, 2 ) ;

			double devX = sqrt(m.mu20/m.m00);
			double devY = sqrt(m.mu02/m.m00);

			if ((m.mu20 > /*0.8*/0.5*m.mu02) && (m.mu02 > 0.8*m.mu20) && (devX/sqrt(curArea) < 0.3) && (devY/sqrt(curArea) < 0.4))
			{
				//fileOut2 << "bdd box test ok" << endl;
				rng = cvRNG( rand() );
				cvRandArr( &rng, add_X_rand, CV_RAND_NORMAL, cvScalar(centerX) /* mean */, cvScalar(devX) /* standard deviation */ );
				cvRandArr( &rng, add_Y_rand, CV_RAND_NORMAL, cvScalar(centerY) /* mean */, cvScalar(devY) /* standard deviation */ );
				for (int i=0; (i<(curArea/170))&&(i<ADD_PARTICLE_NUM); i++)
				{
					int samplingX = cvmGet( add_X_rand, 0, i );
					int samplingY = cvmGet( add_Y_rand, 0, i );
					if (!((samplingX > centerX - 2*devX) && (samplingX < centerX + 2*devX) && (samplingY > centerY - 2*devY) && (samplingY < centerY + 2*devY))) continue;
					CvRect faceTestRgn;
					faceTestRgn.x = samplingX*2 - FACETEST_WIDTH/2 < 0 ? 0 : samplingX*2 - FACETEST_WIDTH/2;
					faceTestRgn.y = samplingY*2 - 2*FACETEST_HEIGHT/3 < 0 ? 0 : samplingY*2 - 2*FACETEST_HEIGHT/3;
					faceTestRgn.width = faceTestRgn.x + FACETEST_WIDTH > width ? width - faceTestRgn.x : FACETEST_WIDTH;
					faceTestRgn.height = faceTestRgn.y + FACETEST_HEIGHT > height ? height - faceTestRgn.y : FACETEST_HEIGHT;
					cvSetImageROI(image, faceTestRgn);
					IplImage* subImg = cvCreateImage(cvSize(faceTestRgn.width, faceTestRgn.height), 8, 3);
					cvCopy(image, subImg, 0);
					cvNamedWindow("subImg", 1);
					cvShowImage("subImg", subImg);
					cvResetImageROI(image);//important!! don't forget.
					frontalFaceTest->detect_and_draw(subImg, -1, cam_id);
					profileFaceTest->detect_and_draw(subImg, -1, cam_id);		

					if (frontalFaceTest->detect || profileFaceTest->detect)
					{
						//fileOut2 << "face test ok" << endl;
						cvReleaseImage(&subImg);//important!! don't forget.
						double thita = 180 * ( atan2( 2*m.mu11 , m.mu20 - m.mu02 ) / (2*3.1415926) ) ;
						double stheta = (thita >= 0 ? 90-thita : -(90+thita) );
				////////fileOut3 << "......................................\n" << endl;
				////////fileOut3 << "(x,y) = " << "(" << samplingX*2.0 << "," << samplingY*2.0 << ")" << endl;
				////////fileOut3 << "scale = " << r << endl;
				////////fileOut3 << "angle = " << stheta + 90.0 << endl;		

						cvCircle(sml_backproj, cvPoint(samplingX, samplingY), 3, cvScalar(125), -1);
						prtPline(samplingY, samplingX, (stheta/180)*3.1415926, sml_backproj);
#ifdef _DEBUG
				////reserve
						cvNamedWindow("sml_backproject",1);
						cvShowImage("sml_backproject",sml_backproj);
						cvNamedWindow("sml_CntCmp",1);
						cvShowImage("sml_CntCmp",sml_CntCmp);
#endif
						//CvRNG rng = cvRNG( rand() );	

						//cvRandArr( &rng, mx, CV_RAND_NORMAL, cvScalar(samplingX*2.0) /* mean */, cvScalar(X_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Blob.MinX()) , cvScalar(Blob.MaxX()) );
						//cvRandArr( &rng, my, CV_RAND_NORMAL, cvScalar(samplingY*2.0) /* mean */, cvScalar(Y_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Blob.MinY()) , cvScalar(Blob.MaxY()) );
						//cvRandArr( &rng, mr, CV_RAND_NORMAL, cvScalar(0.9*r/*0.5*min(temp.size.width, temp.size.height)/MODEL_R*/) /* mean */, cvScalar(R_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );
						//cvRandArr( &rng, mtheta, CV_RAND_NORMAL, cvScalar(stheta + 90.0) /* mean */, cvScalar(THETA_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );
						if (cam_id == CAM1)
						{
							//fileOut2 << "detectObject CAM1 = " << cam_id << endl;
							detect_obj.r = 1.0;//0.9*r;
							detect_obj.x = samplingX*2.0;
							detect_obj.y = samplingY*2.0;
							detect_obj.theta = stheta + 90.0;
							detect_obj.index = DETECT_NEW;
						}
						else //cam_id == CAM2
						{
							//fileOut2 << "detectObject CAM2 = " << cam_id << endl;
							detect_obj.r = 0.3;//This is for CAM2 setting//1.0;//0.9*r;
							detect_obj.x = samplingX*2.0;
							detect_obj.y = samplingY*2.0 - 10;//This is for CAM2 setting
							detect_obj.theta = stheta + 90.0;
							detect_obj.index = DETECT_NEW;
						}

						for (; first_contour->h_next != NULL; ) 
						{
							CvSeq* current = first_contour;
							first_contour = first_contour->h_next;
							cvClearSeq(current);
						}
						cvClearSeq(first_contour);

						return detect_obj;
					}
					cvReleaseImage(&subImg);//important!! don't forget.
				}
			}
		}
	}
	for (int k=0; k<MAX_TARGET_NUM; k++ )
		if (targetList[k].isAlive)
			;//cvCircle(image, cvPoint(targetList[k].tgMAP.x,targetList[k].tgMAP.y),k*2,CV_RGB(255, 255-50*k, 50*k),2);

	//no new object is detected
	ParticleStatus no_object;
	no_object.index = NOT_DETECTED;
	
	if (first_contour != NULL)
	{
		for (; first_contour->h_next != NULL; ) 
		{
			CvSeq* current = first_contour;
			first_contour = first_contour->h_next;
			cvClearSeq(current);
		}
		cvClearSeq(first_contour);
	}
	return no_object;

}


ParticleStatus CAMShift::detectObject(IplImage *image)
{
	IplImage * planes[] = {image_Cr, image_Cb};
	cvCalcBackProject( planes, extend_mask, refHist );
	cvEqualizeHist(extend_mask, extend_mask);
	cvThreshold(extend_mask, extend_mask, SKIN_COLOR_THD-25/*CAM1 setting*/, 255, CV_THRESH_TOZERO);
	cvNamedWindow("skin color detect");
	cvShowImage("skin color detect", extend_mask);

	cvResize(extend_mask, sml_backproj, CV_INTER_LINEAR);
	cvErode(sml_backproj, sml_backproj, smlErosion, 1);
	cvDilate(sml_backproj, sml_backproj, CAM1_detect_dilation/*CAM1 setting*/, 1);
	cvNamedWindow("skin color blob");
	cvShowImage("skin color blob", sml_backproj);

	int c, count ;
	int value, r_sum, c_sum;

	cvClearMemStorage(detect_storage);
    cvFindContours( sml_backproj, detect_storage, &detect_contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ) ;
    CvSeq* first_contour = detect_contour;
    //count = 255 ;

	double bestArea = 0, curArea = 0, maxScore = -10000, curScore = 0;
	CvPoint bestLoc = cvPoint(10000,10000);
	CvPoint pt;
	//search_contours = NULL;
	bool find_new = true;

	int contour_num = 0;
	for( ; detect_contour != 0 ; detect_contour = detect_contour->h_next )
    {
		find_new = true;
		contour_num++;
		curArea = abs(cvContourArea(detect_contour));
		
		if (curArea < SKIN_BLOB_AREA_THD) continue;

		int sumx = 0, sumy = 0;
		for (int k=0; k < 8; k++)
		{
			pt = *(CvPoint*)cvGetSeqElem(detect_contour, k*(detect_contour->total/8));
			sumx += pt.x;
			sumy += pt.y;
		}
		sumx/=8;
		sumy/=8;
		pt = cvPoint(sumx, sumy);
		
		for (int k=0; k<MAX_TARGET_NUM; k++ )
		{
			if (targetList[k].isAlive == true)
			{
				if ( abs(targetList[k].tgMAP.x-pt.x*2) > 80/*100*/ || abs(targetList[k].tgMAP.y-pt.y*2) > 60/*80*/ )
					find_new = find_new ? true : false;
				else
				{
					find_new = false;
					//targetList[k].tgMAP.x = pt.x*2;
					//targetList[k].tgMAP.y = pt.y*2;
				}
			}
		}
		if (find_new)
		{
			//fileOut2 << "find_new" << endl;
			ParticleStatus detect_obj;
			CvScalar color = cvScalar(rand()%150+100);
			cvZero(sml_backproj);
			//cvDrawContours( sml_CntCmp, detect_contour, color, color, -1, CV_FILLED, 8 ) ;
			cvDrawContours( sml_backproj, detect_contour, color, color, -1, CV_FILLED, 8 ) ;

			double r = sqrt(curArea*4.0/(CV_PI*ELLIPSE_RATIO))/MODEL_R;
	
			CvMoments m;
		
			cvMoments( sml_backproj, &m, 0/*1*/ ) ;
			cvGetSpatialMoment( &m, 1, 0);
			cvGetSpatialMoment( &m, 0, 1);
			int centerX = m.m10/m.m00;
			int centerY = m.m01/m.m00;

			cvGetCentralMoment( &m, 1, 1 ) ;
			cvGetCentralMoment( &m, 2, 0 ) ;
			cvGetCentralMoment( &m, 0, 2 ) ;

			if (m.mu20 > /*0.8*/0.5*m.mu02 && m.mu02 > 0.8*m.mu20)
			{
				//fileOut2 << "bdd box test ok" << endl;
				CvRect faceTestRgn;
				faceTestRgn.x = centerX*2 - FACETEST_WIDTH/2 < 0 ? 0 : centerX*2 - FACETEST_WIDTH/2;
				faceTestRgn.y = centerY*2 - 2*FACETEST_HEIGHT/3 < 0 ? 0 : centerY*2 - 2*FACETEST_HEIGHT/3;
				faceTestRgn.width = faceTestRgn.x + FACETEST_WIDTH > width ? width - faceTestRgn.x : FACETEST_WIDTH;
				faceTestRgn.height = faceTestRgn.y + FACETEST_HEIGHT > height ? height - faceTestRgn.y : FACETEST_HEIGHT;
				cvSetImageROI(image, faceTestRgn);
				IplImage* subImg = cvCreateImage(cvSize(faceTestRgn.width, faceTestRgn.height), 8, 3);
				cvCopy(image, subImg, 0);
				cvNamedWindow("subImg", 1);
				cvShowImage("subImg", subImg);
				cvResetImageROI(image);//important!! don't forget.
				frontalFaceTest->detect_and_draw(subImg, -1, cam_id);
				profileFaceTest->detect_and_draw(subImg, -1, cam_id);

				if (frontalFaceTest->detect || profileFaceTest->detect)
				{
					//fileOut2 << "face test ok" << endl;
					cvReleaseImage(&subImg);//important!! don't forget.
					double thita = 180 * ( atan2( 2*m.mu11 , m.mu20 - m.mu02 ) / (2*3.1415926) ) ;
					double stheta = (thita >= 0 ? 90-thita : -(90+thita) );
			////////fileOut3 << "......................................\n" << endl;
			////////fileOut3 << "(x,y) = " << "(" << centerX*2.0 << "," << centerY*2.0 << ")" << endl;
			////////fileOut3 << "scale = " << r << endl;
			////////fileOut3 << "angle = " << stheta + 90.0 << endl;	

					cvCircle(sml_backproj, cvPoint(centerX, centerY), 3, cvScalar(125), -1);
					prtPline(centerY, centerX, (stheta/180)*3.1415926, sml_backproj);
#ifdef _DEBUG
			////reserve
					cvNamedWindow("sml_backproject",1);
					cvShowImage("sml_backproject",sml_backproj);
					cvNamedWindow("sml_CntCmp",1);
					cvShowImage("sml_CntCmp",sml_CntCmp);
#endif
					//CvRNG rng = cvRNG( rand() );

					//cvRandArr( &rng, mx, CV_RAND_NORMAL, cvScalar(centerX*2.0) /* mean */, cvScalar(X_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Blob.MinX()) , cvScalar(Blob.MaxX()) );
					//cvRandArr( &rng, my, CV_RAND_NORMAL, cvScalar(centerY*2.0) /* mean */, cvScalar(Y_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Blob.MinY()) , cvScalar(Blob.MaxY()) );
					//cvRandArr( &rng, mr, CV_RAND_NORMAL, cvScalar(0.9*r/*0.5*min(temp.size.width, temp.size.height)/MODEL_R*/) /* mean */, cvScalar(R_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );
					//cvRandArr( &rng, mtheta, CV_RAND_NORMAL, cvScalar(stheta + 90.0) /* mean */, cvScalar(THETA_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );
					if (cam_id == CAM1)
					{
						//fileOut2 << "detectObject CAM1 = " << cam_id << endl;
						detect_obj.r = 1.0;//0.9*r;
						detect_obj.x = centerX*2.0;
						detect_obj.y = centerY*2.0;
						detect_obj.theta = stheta + 90.0;
						detect_obj.index = DETECT_NEW;
					}
					else //cam_id == CAM2
					{
						//fileOut2 << "detectObject CAM2 = " << cam_id << endl;
						detect_obj.r = 0.3;//This is for CAM2 setting//1.0;//0.9*r;
						detect_obj.x = centerX*2.0;
						detect_obj.y = centerY*2.0 - 10;//This is for CAM2 setting
						detect_obj.theta = stheta + 90.0;
						detect_obj.index = DETECT_NEW;
					}

					for (; first_contour->h_next != NULL; ) 
					{
						CvSeq* current = first_contour;
						first_contour = first_contour->h_next;
						cvClearSeq(current);
					}
					cvClearSeq(first_contour);

					return detect_obj;
				}
				cvReleaseImage(&subImg);//important!! don't forget.
			}
		}
	}
	for (int k=0; k<MAX_TARGET_NUM; k++ )
		if (targetList[k].isAlive)
			;//cvCircle(image, cvPoint(targetList[k].tgMAP.x,targetList[k].tgMAP.y),k*2,CV_RGB(255, 255-50*k, 50*k),2);

	//no new object is detected
	ParticleStatus no_object;
	no_object.index = NOT_DETECTED;
	
	if (first_contour != NULL)
	{
		for (; first_contour->h_next != NULL; ) 
		{
			CvSeq* current = first_contour;
			first_contour = first_contour->h_next;
			cvClearSeq(current);
		}
		cvClearSeq(first_contour);
	}
	return no_object;

}

CamIterationReturn CAMShift::iteration_init(IplImage *image, IplImage* &fgmask, CvRect ROI)
{
		if( mLosingCount > 0 ) // start counting iteration
			mIterationCount++;

		cvCvtColor(image, gray, CV_BGR2GRAY);
//		SobelEdge();
		SobelEdge1();
		cvCvtColor( image, hsv, CV_BGR2HSV );
		cvResetImageROI(mask);
		cvInRangeS( hsv, cvScalar(0,smin,MIN(vmin,vmax),0), cvScalar(180,256,MAX(vmin,vmax),0), mask );

		//dilate 如此可以讓saturation的門檻設高一點，卻又不讓抓物體的手指或是追蹤物體的深淺色細紋使橢圓排斥
		cvDilate( mask, mask, ck, 1 );

		cvResetImageROI(sat);
		cvSplit( hsv, hue, sat, val, 0 );
		

		//cvNamedWindow("current target Sat", 1);
		//cvShowImage("current target Sat", sat);
		cvSetImageROI(sat, ROI);
		cvInRangeS(sat, cvScalar(targetSat-SatTh), cvScalar(targetSat+SatTh), sat);
		



		cvResetImageROI(backproject);
		cvCalcBackProject( &hue, backproject, hist );

		


		cvSetImageROI(backproject, ROI);
		cvSetImageROI(mask, ROI);
		cvSetImageROI(half_backproject, ROI);
		cvAnd( backproject, mask, half_backproject, 0 );

		if (fgmask) 
		{
			cvSetImageROI(fgmask, ROI);
			cvAnd(half_backproject, fgmask, half_backproject, 0);
			cvAnd(sat, fgmask, sat, 0);
		}

		cvAnd(half_backproject, sat, half_backproject);

		cvErode(half_backproject, half_backproject, smlErosion, 1);
		cvDilate(half_backproject, half_backproject, ck, 1);
		cvErode(half_backproject,half_backproject, bigErosion, 1);
		cvDilate(half_backproject,half_backproject, ck, 1);
		cvErode(half_backproject,half_backproject, ck, 1);
		
		cvSetImageROI(half_backproject, cvGetImageROI(backproject));

		cvSetImageROI(safeMask, ROI);
		cvAnd(half_backproject, safeMask, half_backproject);

		cvResetImageROI(half_backproject);
		cvZero(ROImask);
		cvRectangle(ROImask, cvPoint(ROI.x,ROI.y), 
			cvPoint(ROI.x+ROI.width, ROI.y+ROI.height), cvScalar(255), -1);
		cvAnd(half_backproject, ROImask, half_backproject);

		//2009/4/15 1717
		cvEqualizeHist(half_backproject, half_backproject);
#ifdef _DEBUG
		cvNamedWindow("current target sat And backproject", 1);
		cvShowImage("current target sat And backproject", half_backproject);
		//2009/4/15 1717
#endif

		int it = cvCamShift( half_backproject/*backproject*/, track_window,
			        cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, /*15*/10, 1/*0.75*/ ),
				    &track_comp, &track_box );

		//if( !image->origin )
//			track_box.angle = -track_box.angle;
		
		if (iteration_cnt == NOM_FOR_INIT_STATE_USING_CAMSHIFT)
		{
			setUpSupportingClass(image);
		}


		CvPoint ROIcenter;
		ROIcenter.x=static_cast<int>(((intepolation_m+intepolation_n)*cvPointFrom32f(track_box.center).x
			-intepolation_n*(ROI.x+static_cast<int>(0.5*ROI.width)))/intepolation_m);
		ROIcenter.y=static_cast<int>(((intepolation_m+intepolation_n)*cvPointFrom32f(track_box.center).y
			-intepolation_n*(ROI.y+static_cast<int>(0.5*ROI.height)))/intepolation_m);

//		if (maxScore == 0) it=0;

		//(*mPresentedControlString) += string( "iteration!" );
		maxIt = (maxIt >= it) ? maxIt : it;
		
		//time(&mEndTimeStamp);
		if( mLosingCount == MAX_LOSING_COUNT /*|| ( mLosingCount >= 2 && difftime( mEndTimeStamp, mStartTimeStamp ) >= 2.0 )*/ ) // re-initialization
		{
			mIterationCount = 0;
			mLosingCount = 0;

			CamIterationReturn Return;
			Return.val = -1;
			return Return;
		}

		//if we got continuous 15 0's, then we restart.
		if (it == 0 && maxIt == 0)
		{
			noMotionCnt++;
			if (noMotionCnt > 15)
			{
				noMotionCnt = 0;
				CamIterationReturn Return;
				Return.targetCenter = cvPointFrom32f(track_box.center);
				Return.val = -1;
				int ROI_x = (ROIcenter.x-static_cast<int>(0.5*ROIdim) > 0)?(ROIcenter.x-static_cast<int>(0.5*ROIdim)):0;
				int ROI_y = (ROIcenter.y-static_cast<int>(0.5*ROIdim) > 0)?(ROIcenter.y-static_cast<int>(0.5*ROIdim)):0;
				int ROI_w = (ROIcenter.x+static_cast<int>(0.5*ROIdim) < width)?
					(static_cast<int>(0.5*ROIdim) + (ROIcenter.x - ROI_x)):(width-ROIcenter.x+(ROIcenter.x - ROI_x));
				int ROI_h = (ROIcenter.y+static_cast<int>(0.5*ROIdim) < height)?
					(static_cast<int>(0.5*ROIdim) + (ROIcenter.y - ROI_y)):(height-ROIcenter.y+(ROIcenter.y - ROI_y));
				/*Return.targetROI = cvRect(((ROIcenter.x-static_cast<int>(0.5*ROIdim) > 0)?(ROIcenter.x-static_cast<int>(0.5*ROIdim)):0), 
										  ((ROIcenter.y-static_cast<int>(0.5*ROIdim) > 0)?(ROIcenter.y-static_cast<int>(0.5*ROIdim)):0), 
										  ((ROIcenter.x+static_cast<int>(0.5*ROIdim) < width)?(ROIdim):(width-ROIcenter.x+static_cast<int>(0.5*ROIdim))),
										  ((ROIcenter.y+static_cast<int>(0.5*ROIdim) < height)?(ROIdim):(height-ROIcenter.y+static_cast<int>(0.5*ROIdim))));*/
				Return.targetROI = cvRect(ROI_x, ROI_y, ROI_w, ROI_h);
				return Return;
			}
		}
		else noMotionCnt = 0;

		track_window = track_comp.rect;

		////if( !image->origin )
		//	track_box.angle = -track_box.angle;
			
		//畫的時候用一個反的橢圓去畫 不改track_box值
		CvBox2D box = track_box;
		box.angle = -box.angle;
		CvPoint2D32f pt[4];
		cvBoxPoints( box,  pt );
		//cvLine( image, cvPoint( (pt[0].x+pt[1].x)/2, (pt[0].y+pt[1].y)/2 ), cvPoint( (pt[2].x+pt[3].x)/2, 
		//	(pt[2].y+pt[3].y)/2 ), scaleColor(getScale(STANDARD_THRESHOLD, box.size.height*box.size.width)), 3 );
		//cvLine( image, cvPoint( (pt[0].x+pt[3].x)/2, (pt[0].y+pt[3].y)/2 ), cvPoint( (pt[2].x+pt[1].x)/2, 
		//	(pt[2].y+pt[1].y)/2 ), scaleColor(getScale(STANDARD_THRESHOLD, box.size.height*box.size.width)), 3 );
		//cvEllipseBox( image, box, CV_RGB(255,200,200), 1, 8, 0 );
		//cvRectangle( image, cvPoint(0.5*width-0.17*width, 0.5*height-0.2*height), cvPoint(0.5*width+0.17*width, 
		//	0.5*height+0.2*height), CV_RGB(255, 255, 255), 2);
		//cvRectangle( image, cvPoint(0.5*width-0.17*width-2, 0.5*height-0.2*height-2), cvPoint(0.5*width+0.17*width+2, 
		//	0.5*height+0.2*height+2), CV_RGB(0, 0, 0), 2);
	
		//Draw current histogram
		//int left = min(min(pt[1].x, pt[2].x), min(pt[3].x, pt[0].x));
		//int right = max(max(pt[1].x, pt[2].x), max(pt[3].x, pt[0].x));
		//int top = max(max(pt[1].y, pt[2].y), max(pt[3].y, pt[0].y));
		//int bottom = min(min(pt[1].y, pt[2].y), min(pt[3].y, pt[0].y));
		//CvRect crtbox = cvRect(left, bottom, right-left, top-bottom);
		cvZero(crtMask);
		cvEllipseBox(crtMask, box, cvScalar(255), -1, 8, 0);
		//cvRectangle(crtMask, cvPoint(crtbox.x, crtbox.y), cvPoint(crtbox.x+crtbox.width, crtbox.y+crtbox.height), cvScalar(255), -1);
		cvCalcHist( &hue, crtHist, 0, crtMask );
		float max_val = 0.f;
		cvGetMinMaxHistValue( crtHist, 0, &max_val, 0, 0 );
		cvConvertScale( crtHist->bins, crtHist->bins, max_val ? 255. / max_val : 0., 0 );
		cvResetImageROI( hue );
		cvResetImageROI( crtMask );
		int bin_w = crtHistimg->width / hdims;
		cvZero(crtHistimg);
		for( int i = 0; i < hdims; i++ )
		{
			int val = cvRound( cvGetReal1D(crtHist->bins,i)*crtHistimg->height/255 );
			CvScalar color = hsv2rgb(i*180.f/hdims);
			cvRectangle( crtHistimg, cvPoint(i*bin_w,crtHistimg->height),
					cvPoint((i+1)*bin_w,crtHistimg->height - val),
					color, -1, 8, 0 );
		}
//		cvNamedWindow("current target Hist(raw)", 1);
//		cvShowImage("current target Hist(raw)", crtHistimg);	


		//if( mStartFlag == true ) // register the standard size
		//{
		//	mStandardArea = (track_box.size.height)*(track_box.size.width);
		//	mMinArea = static_cast<int>(mStandardArea * 0.2);
		//	mMaxArea = static_cast<int>(mStandardArea * 2.5);
		//	mStartFlag = false;
		//}

		int k = static_cast<int>(track_box.size.height+track_box.size.width);
		if ( /*k > mMaxArea || */k > MAX_THRESHOLD && (maxIt != 0))
		{
			losingCnt++;
			if (losingCnt > 3)
			{	
				CamIterationReturn &Return = setupReturn(ROIcenter, -2);
				return Return;
			}
		}
		else if ( /*k < mMinArea ||*/ k < MIN_THRESHOLD && (maxIt != 0))
		{
			losingCnt++;
			if (losingCnt > 3)
			{
				CamIterationReturn &Return = setupReturn(ROIcenter, -3);
				return Return;
			}
		}
		else 
			losingCnt = 0;


		
		//sendControlSignal(timeDiff);	
		
		CamIterationReturn Return;
		Return.targetCenter = cvPointFrom32f(track_box.center);
		Return.val = 0;

		int ROI_x = (ROIcenter.x-static_cast<int>(0.5*ROIdim) > 0)?(ROIcenter.x-static_cast<int>(0.5*ROIdim)):0;
		int ROI_y = (ROIcenter.y-static_cast<int>(0.5*ROIdim) > 0)?(ROIcenter.y-static_cast<int>(0.5*ROIdim)):0;
		int ROI_w = (ROIcenter.x+static_cast<int>(0.5*ROIdim) < width)?
					(static_cast<int>(0.5*ROIdim) + (ROIcenter.x - ROI_x)):(width-ROIcenter.x+(ROIcenter.x - ROI_x));
		int ROI_h = (ROIcenter.y+static_cast<int>(0.5*ROIdim) < height)?
					(static_cast<int>(0.5*ROIdim) + (ROIcenter.y - ROI_y)):(height-ROIcenter.y+(ROIcenter.y - ROI_y));
		/*Return.targetROI = cvRect(((ROIcenter.x-static_cast<int>(0.5*ROIdim) > 0)?(ROIcenter.x-static_cast<int>(0.5*ROIdim)):0), 
								  ((ROIcenter.y-static_cast<int>(0.5*ROIdim) > 0)?(ROIcenter.y-static_cast<int>(0.5*ROIdim)):0), 
								  ((ROIcenter.x+static_cast<int>(0.5*ROIdim) < width)?(ROIdim):(width-ROIcenter.x+static_cast<int>(0.5*ROIdim))),
								  ((ROIcenter.y+static_cast<int>(0.5*ROIdim) < height)?(ROIdim):(height-ROIcenter.y+static_cast<int>(0.5*ROIdim))));*/
		Return.targetROI = cvRect(ROI_x, ROI_y, ROI_w, ROI_h);

		return Return;
}


int CAMShift::getRotAng(float agl)
{
	if( agl >= 0 && agl <= 40 )
		return 2;
	else if( agl > 40 && agl <= 70 )
		return 1;
	else if( agl < 0 && agl >= -40 )
		return -2;
	else if( agl < -40 && agl >= -70 )
		return -1;
	else
		return 0;
}

int CAMShift::getPosX(int x)
{
	if (-0.17*width <= x-width/2 && x-width/2 <= 0.17*width)
		return 0;
	else if (-(0.125*width)*2 <= x-width/2 && x-width/2 < -0.17*width)
		return 1;
	else if ( (0.17*width) < x-width/2 && x-width/2 <= (0.125*width)*2)
		return -1;
	else if ( -(0.125*width)*3 <= x-width/2 && x-width/2 < -(0.125*width)*2)
		return 2;
	else if ( (0.125*width)*2 < x-width/2  && x-width/2 <= (0.125*width)*3)
		return -2;
	else if ( x-width/2 < -(0.125*width)*3)
		return 3;
	else if ( (0.125*width)*3 < x-width/2)
		return -3;
}

int CAMShift::getPosY(int y)
{
	if (-0.2*height <= y-height/2 && y-height/2 <= 0.2*height)
		return 0;
	else if (-(0.16667*height)*2 <= y-height/2 && y-height/2 < -0.2*height)
		return -1;
	else if ( (0.2*height) < y-height/2 && y-height/2 <= (0.16667*height)*2)
		return 1;
	else if ( y-height/2 < -(0.16667*height)*2)
		return -2;
	else if ( (0.16667*height)*2 < y-height/2 )
		return 2;
}


int CAMShift::getScale(double org, double crt)
{
	//if ( org > crt) // smaller
	//{
	//	if (org/crt < 1.15)
	//		return 0;
	//	else if (1.15 <= org/crt && org/crt <= 1.8)
	//		return -1;
	//	else if (1.8 < org/crt)
	//		return -2;
	//}
	//else  // larger
	//{
	//	if (crt/org < 1.5)
	//		return 0;
	//	else if (1.5 <= crt/org && crt/org <= 2.5)
	//		return 1;
	//	else if (2.5 < crt/org)
	//		return 2;
	//}

	if ( org > crt) // smaller
	{
		if (org/crt < 1.4)
			return 0;
		else if (1.4 <= org/crt && org/crt <= 1.65)
			return -1;
		else if (1.65 < org/crt)
			return -2;
	}
	else  // larger
	{
		if (crt/org < 1.3)
			return 0;
		else if (1.3 <= crt/org && crt/org <= 1.45)
			return 1;
		else if (1.45 < crt/org)
			return 2;
	}
}

CvScalar CAMShift::scaleColor(const int scale)
{
	if (scale == 0)
		return CV_RGB(255,255,255);
	else if (scale == 1)
		return CV_RGB(255,120,120);
	else if (scale == 2)
		return CV_RGB(255, 0, 0);
	else if (scale == -1)
		return CV_RGB(170,80,80);
	else if (scale == -2)
		return CV_RGB(100, 0, 0);
}