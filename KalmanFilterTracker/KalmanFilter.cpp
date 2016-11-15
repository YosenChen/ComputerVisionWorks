/*************************************ReadMe of Kalman Filter Visual Tracker***************************
//
// This is a basic demo environment for visual tracking 
// Author: Yu-Sheng (Yosen) Chen
//
*******************************************************************************************************/

/*************************************Version Upate Note***********************************************
Date: 2013/1/6, 14:02
Update Part: (this is the first note, earlier updates cannot be traced...)
	0. add comments
		//global variables
		//observation measurement tool
		//Kalman filtering position estimator
		And change variable name: center -> obsPos
	1. implement position(state) estimator
		create functions:
			CvPoint trivialPosEstimator(CvPoint obsPos) //for original version
			CvPoint kalmanFilterPosEstimator(CvPoint obsPos) //for kalman filter version
	2. change to function pointer versions
		including observation measurement tool, position(state) estimator
	3. 

Date: 2013/6/25
Update Part:
	1. Implement 2x2 Kalman filter position estimator.
	2. Use design pattern:  to model this tracking system framework.
	3. Provide document (slides, webpage) to describe this code.
	4. Upload this code version as the first version of "Object-Tracking" on GitHub.


*******************************************************************************************************/
#include <stdio.h>
#include <iostream>
using namespace std;
#include <conio.h>
#include <tchar.h>
#include <windows.h>
#include <time.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvcam.h>

#define IMG_WIDTH 320
#define IMG_HEIGHT 240
#define SEARCH_RANGE 50
#define ML_SEARCH_RANGE 100
#define ML_SEARCH_GRID 5
#define BACKPROJECT_TH 128 /*80*/
#define VAL_RANGE 80 /*40*/
#define RATIO_THD_BACKPROJ_NOISE 0.5/*0.1*/

//global variables
CvVideoWriter *writer;
CvCapture *videoCapture;
IplImage *frame, *frame_RGB, *frame_HSV, *frame_H, *frame_S, *frame_V, *backProject, /*new add*/*backProject_temp, *colorSample_RGB, *colorSample_HSV, *CS_ch0, *CS_ch1, *CS_ch2, /*new add*/*ValMsk;
CvHistogram* hist;

//observation measurement tool
CvPoint windowMeanShift(IplImage* bp, CvPoint obsPos);
CvPoint maxLikelihood(IplImage* bp, CvPoint obsPos);
CvPoint sampleMean(IplImage* bp, CvPoint obsPos);

//importance mapping tool for observation measurement tool
void impMap_HBack_fixTH_valMsk(IplImage* frame_RGB, int avgVal);
void impMap_HBack_fixTH(IplImage* frame_RGB, int THD);
void impMap_HBack_adaptiveTH(IplImage* frame_RGB);

//position(state) estimator
CvPoint trivialPosEstimator(CvPoint obsPos);
CvPoint kalmanFilterPosEstimator(CvPoint obsPos);
CvPoint kalmanFilterPosEstimator2x2(CvPoint obsPos);


CvScalar hsv2rgb( float hue )
{
    int rgb[3], p, sector;
    static const int sector_data[][3]=
        {{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
    hue *= 0.033333333333333333333333333333333f;
    sector = cvFloor(hue);
    p = cvRound(255*(hue - sector));
    p ^= sector & 1 ? 255 : 0;

    rgb[sector_data[sector][0]] = 255;
    rgb[sector_data[sector][1]] = 0;
    rgb[sector_data[sector][2]] = p;

    return cvScalar(rgb[2], rgb[1], rgb[0],0);
}

int main (void)
{
	printf("//simple test");
	printf("size of unsigned char = %d byte(s)\n", sizeof(unsigned char));
	printf("size of char = %d byte(s)\n", sizeof(char));
	printf("\n");
	printf("size of int = %d byte(s)\n", sizeof(int));
	printf("size of unsigned int = %d byte(s)\n", sizeof(unsigned int));
	printf("size of long int = %d byte(s)\n", sizeof(long int));
	printf("size of short = %d byte(s)\n", sizeof(short));
	printf("\n");
	printf("size of double = %d byte(s)\n", sizeof(double));
	printf("size of long double = %d byte(s)\n", sizeof(long double));
	printf("\n");
	printf("size of float = %d byte(s)\n", sizeof(float));
	printf("\n");
	printf("size of bool = %d byte(s)\n", sizeof(bool));
	printf("\n");
	printf("===============================================================\n");

	float hranges_arr[] = {0,180};
    float* hranges = hranges_arr;
	int hdims = 16;
	hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	writer = cvCreateVideoWriter("TrackingResult[1].avi",-1,22,cvSize(320,240),1);
//	videoCapture = cvCaptureFromAVI("MVI_0216_cvtd.AVI");
	videoCapture = cvCaptureFromAVI("MVI_0206_cvtd.AVI");
	colorSample_RGB = cvLoadImage("colorSample_purpleBall_noise.jpg");
//	colorSample_RGB = cvLoadImage("blackDog.jpg");
	CS_ch0 = cvCreateImage(cvGetSize(colorSample_RGB), 8, 1);
	CS_ch1 = cvCreateImage(cvGetSize(colorSample_RGB), 8, 1);
	CS_ch2 = cvCreateImage(cvGetSize(colorSample_RGB), 8, 1);
	colorSample_HSV = cvCreateImage(cvGetSize(colorSample_RGB), 8, 3);
	backProject = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 1);
	backProject_temp = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 1);
	frame_RGB = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 3);
	frame_H = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 1);
	frame_S = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 1);
	frame_V = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 1);
	frame_HSV = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 3);
	ValMsk = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), 8, 1);

	cvCvtColor(colorSample_RGB, colorSample_HSV, CV_BGR2HSV);
	cvSplit(colorSample_HSV, CS_ch0, CS_ch1, CS_ch2, 0);

	cvCalcHist(&CS_ch0, hist, 0, 0);
	float max_val = 0.0f;
	cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 );
	cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 );


	long int sumVal = 0;
	for (int j=0; j<cvGetSize(colorSample_RGB).width; j++)
	{
		for (int k=0; k<cvGetSize(colorSample_RGB).height; k++)
		{
			sumVal += CV_IMAGE_ELEM( CS_ch2, unsigned char, k, j );
		}
	}
	int avgVal = sumVal/(cvGetSize(colorSample_RGB).width*cvGetSize(colorSample_RGB).height);
	


//	torso_color = cvCreateHist(1,&hdims,CV_HIST_ARRAY, &hranges, 1);
//	IplImage* skin = 0, *hsv = 0, *hue = 0;
	IplImage* histimg = cvCreateImage(cvSize(320,200),8,3);
//	if (cam_id == CAM1)
//	{
//		fileOut4 << "fd green CAM1 = " << cam_id << endl;
//		skin = cvLoadImage("green_CAM1.bmp"/*"Skin.jpg"*/,1);
//	}
//	else //cam_id == CAM2
//	{
//		fileOut4 << "fd green CAM2 = " << cam_id << endl;
//		skin = cvLoadImage("green_CAM2.bmp"/*"Skin.jpg"*/,1);
//	}
//	hsv = cvCreateImage(cvGetSize(skin),8,3);
//	hue = cvCreateImage(cvGetSize(skin),8,1);
//	cvCvtColor(skin,hsv,CV_BGR2HSV);
//	cvSplit(hsv,hue,0,0,0);
//	cvCalcHist(&hue,ref_hist,0,0);
//    float max_val = 0.0f;
	cvZero(histimg);
//	cvGetMinMaxHistValue( ref_hist, 0, &max_val, 0, 0 );
//    cvConvertScale( ref_hist->bins, ref_hist->bins, max_val ? 255. / max_val : 0., 0 );
	int bin_w = histimg->width / hdims;
    for( int i = 0; i < hdims; i++ )
    {
		int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
        CvScalar color = hsv2rgb(i*180.f/hdims);
        cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
                     cvPoint((i+1)*bin_w,histimg->height - val),
                     color, -1, 8, 0 );
	}
	cvNamedWindow("Reference Histogram");
	cvShowImage("Reference Histogram",histimg);





	cvNamedWindow("colorSample");
	cvNamedWindow("source");
	cvNamedWindow("backProject");
	cvNamedWindow("backProject_TH");
	cvNamedWindow("frame_H");
	cvNamedWindow("frame_S");
	cvNamedWindow("frame_V");


	CvPoint obsPos;
	
	//initialize tracking position (state)
	int useDef;
	cout << "Enter\n"
		"1: use default initial position\n"
		"Others: use new initial position\n";
	cin >> useDef;
	if (useDef == 1)
	{
		obsPos.x = IMG_WIDTH/2;
		obsPos.y = IMG_HEIGHT/2;
	}
	else
	{
		cout << "input initial position: \n";
		cout << "x = ";
		cin >> obsPos.x;
		cout << "y = ";
		cin >> obsPos.y;
	}


	//select tracking mode
	int mode;
	printf("\nSelect observation measurement tool: \n"
		"1: WindowMeanShift\n"
		"2: MaxLikelihood\n"
		"3: SampleMean\n");
	cin >> mode;

	//select importance map type
	int impMapType;
	printf("\nSelect importance map type (for observation measurement): \n"
		"1: impMap_HBack_fixTH_valMsk\n"
		"2: impMap_HBack_fixTH\n"
		"3: impMap_HBack_adaptiveTH\n");
	cin >> impMapType;

	//select position(state) estimator
	int stateEstType;
	printf("\nSelect position(state) estimator: \n"
		"1: trivial estimator: current state estimation result = current observation \n"
		"2: Kalman filtering estimator\n"
		"3: Kalman filtering estimator (2x2 model)\n");
	cin >> stateEstType;

	
	while(frame = cvQueryFrame(videoCapture))
	{
		//if( frame->origin == IPL_ORIGIN_TL )
		//	cvFlip(frame, frame_RGB, 0);
		//else
		//	cvCopy(frame, frame_RGB, 0 );

		cvFlip(frame, frame_RGB, 0);

		switch (impMapType)
		{
		case 1:
			impMap_HBack_fixTH_valMsk(frame_RGB, avgVal);
			break;
		case 2:
			impMap_HBack_fixTH(frame_RGB, BACKPROJECT_TH);
			break;
		case 3:
			impMap_HBack_adaptiveTH(frame_RGB);
			break;
		default:
			cout << "impMapType error\n";
			break;
		}
		
		CvPoint (*pObsMeasure)(IplImage*, CvPoint);
		switch (mode)
		{
		case 1:
			pObsMeasure = &windowMeanShift;
			break;
		case 2:
			pObsMeasure = maxLikelihood;
			break;
		case 3: 
			pObsMeasure = sampleMean;
			break;
		default:
			cout << "mode error\n";
			break;
		}
		//apply observation model
		obsPos = (*pObsMeasure)(backProject_temp, obsPos);

		//Show observation measurement result
		cvRectangle(frame_RGB, cvPoint(obsPos.x-SEARCH_RANGE, obsPos.y-SEARCH_RANGE), cvPoint(obsPos.x+SEARCH_RANGE, obsPos.y+SEARCH_RANGE), CV_RGB(255, 255, 255), 1, 4);
		//cvCircle(frame_RGB, obsPos, 4, CV_RGB(255, 0, 0), 2);


		CvPoint (*pStateEstimator)(CvPoint obsPos);
		switch (stateEstType)
		{
		case 1:
			pStateEstimator = &trivialPosEstimator;
			break;
		case 2:
			pStateEstimator = &kalmanFilterPosEstimator;
			break;
		case 3: 
			pStateEstimator = &kalmanFilterPosEstimator2x2;
			break;
		default:
			printf("stateEstType error\n");
		}
		//Apply state estimation
		CvPoint state = (*pStateEstimator)(obsPos);

		//Show state estimation result
		cvRectangle(frame_RGB, cvPoint(state.x-SEARCH_RANGE, state.y-SEARCH_RANGE), cvPoint(state.x+SEARCH_RANGE, state.y+SEARCH_RANGE), CV_RGB(255, 0, 0), 1, CV_AA);
		//cvCircle(frame_RGB, state, 8, CV_RGB(255, 255, 0), 2);
		cvLine(frame_RGB, cvPoint(state.x-SEARCH_RANGE/6, state.y), cvPoint(state.x+SEARCH_RANGE/6, state.y), CV_RGB(255,0,0), 2, CV_AA);
		cvLine(frame_RGB, cvPoint(state.x, state.y-SEARCH_RANGE/6), cvPoint(state.x, state.y+SEARCH_RANGE/6), CV_RGB(255,0,0), 2, CV_AA);

		cvShowImage("source", frame_RGB);
		cvShowImage("colorSample", colorSample_RGB);
		cvShowImage("frame_H", frame_H);
		cvShowImage("frame_S", frame_S);
		cvShowImage("frame_V", frame_V);
		cvShowImage("backProject_TH", backProject_temp);

		cvWriteFrame(writer, frame_RGB);

		if (mode != 2) cvWaitKey(33);
		else cvWaitKey(5);
	}

	system("PAUSE");

	cvReleaseVideoWriter(&writer);


	return 0;
}

CvPoint trivialPosEstimator(CvPoint obsPos)
{
	// trivial case: 
	// transition model(A) and input model(B,u) are don't care, 
	// measurement model(H) = 1,
	// process noise covariance(Q) = infinity
	// measurement noise covariance(R) << Q
	// thus, Kalman gain(K) = 1, updated state x (by measurement z) = z
	// thus updated P = 0.
	return obsPos; 
}

inline CvPoint stateSubtract(CvPoint A, CvPoint B){return cvPoint(A.x-B.x, A.y-B.y);} //for state operation: A-B
inline CvPoint stateAdd(CvPoint A, CvPoint B){return cvPoint(A.x+B.x, A.y+B.y);} //for state operation: A+B
inline CvPoint stateScale(CvPoint X, float a){return cvPoint((int)(a*(float)(X.x)), (int)(a*(float)(X.y)));} //for state scaling: aX

#define PRE_DIST_ARR_LENGTH 10
// measurement noise variance(R) estimation
float obsVarR_Est(CvPoint obsPos)
{
	// here we implement Mean-Square observation distance (to previous observation) 
	static int obsCnt = 0; // this line only goes once, initialization
	static float preDistSqArr[PRE_DIST_ARR_LENGTH] = {0.0};
	static CvPoint preObsPos;


	//if (obsCnt < PRE_DIST_ARR_LENGTH) // 0,...,PRE_DIST_ARR_LENGTH-1
	// now, it's OK for all cases
	{
		if (obsCnt >= PRE_DIST_ARR_LENGTH) obsCnt = PRE_DIST_ARR_LENGTH-1; // full storage

		//update buffer array
		float curDistSq;
		int i = 0;
		for (; i<obsCnt; i++)
		{
			preDistSqArr[obsCnt-i] = preDistSqArr[obsCnt-1-i];
		}
		preDistSqArr[0] = (obsCnt == 0) ? 0/*first call*/ : curDistSq = pow((double)(obsPos.x-preObsPos.x), 2) + pow((double)(obsPos.y-preObsPos.y), 2);
		
		//calculate obsVarR
		float obsVarR = 0; 
		for (i=0; i<obsCnt+1; i++)
		{
			obsVarR+=preDistSqArr[i];
		}
		obsVarR = obsVarR/(obsCnt+1);
		obsVarR = (obsCnt == 0) ? 10.0 : obsVarR;

		printf("curDistSq = %.3f, obsVarR = %.3f ", (obsCnt == 0) ? 0.0 : curDistSq, obsVarR);

		preObsPos = obsPos;
		++obsCnt;
		return obsVarR;
	}

}

#define varQ 50.0 //unit: pixel^2

CvPoint kalmanFilterPosEstimator(CvPoint obsPos)
{
	// tranistion model(A) = 1,
	// input model(B,u) = 0,
	// measurement model(H) = 1,
	// 
	static CvPoint curState = cvPoint(IMG_WIDTH/2, IMG_HEIGHT/2); // this line only goes once, initialization
	static CvPoint preState = cvPoint(IMG_WIDTH/2, IMG_HEIGHT/2); // this line only goes once, initialization
	static float kalmanGainK = 0; // this line only goes once, initialization
	static float errVarianceP = 0; // this line only goes once, initialization

	//Project the state ahead
	curState = preState;
	//Project the error covariance ahead
	errVarianceP = errVarianceP + varQ;

	//calculate kalmanGainK
	kalmanGainK = errVarianceP/(errVarianceP+obsVarR_Est(obsPos));
	// Update state with measurement
	curState = stateAdd(preState, stateScale(stateSubtract(obsPos,preState), kalmanGainK/*0.5*/));
	// Update error covariance
	errVarianceP = (1.0 - kalmanGainK)*errVarianceP;

	//print detail status
	printf("kalmanGainK = %.3f ", kalmanGainK);
	printf("errVarianceP = %.3f\n", errVarianceP);

	preState = curState; // for next iteration

	return curState;
}

struct Float2x2; //forward declaration

struct Float2x1
{
	float x11, x21;
	Float2x1():x11(0),x21(0) {}
	Float2x1(float y11, float y21):x11(y11),x21(y21) {}
	Float2x1&	operator=(Float2x1& rhs)	{x11=rhs.x11; x21=rhs.x21; return *this;}
	Float2x1	operator+(Float2x1& rhs)	
	{
		Float2x1 ret; 
		ret.x11=x11+rhs.x11; 
		ret.x21=x21+rhs.x21; 
		return ret;
	}
	Float2x1	operator-(Float2x1& rhs)	
	{
		Float2x1 ret; 
		ret.x11=x11-rhs.x11; 
		ret.x21=x21-rhs.x21; 
		return ret;
	}
};

struct Float2x2
{
	float x11, x12, x21, x22;
	Float2x2():x11(0),x12(0),x21(0),x22(0) {}
	Float2x2(float y11, float y12, float y21, float y22):x11(y11),x12(y12),x21(y21),x22(y22) {}
	Float2x2&	operator=(Float2x2& rhs)	{x11=rhs.x11; x12=rhs.x12; x21=rhs.x21; x22=rhs.x22; return *this;}
	Float2x2	operator+(Float2x2& rhs)	
	{
		Float2x2 ret; 
		ret.x11=x11+rhs.x11; 
		ret.x12=x12+rhs.x12; 
		ret.x21=x21+rhs.x21; 
		ret.x22=x22+rhs.x22; 
		return ret;
	}
	Float2x2	operator-(Float2x2& rhs)	
	{
		Float2x2 ret; 
		ret.x11=x11-rhs.x11; 
		ret.x12=x12-rhs.x12; 
		ret.x21=x21-rhs.x21; 
		ret.x22=x22-rhs.x22; 
		return ret;
	}
	Float2x2	operator*(Float2x2& rhs)	//matrix*matrix
	{
		Float2x2 ret; 
		ret.x11 = x11*rhs.x11+x12*rhs.x21;
		ret.x12 = x11*rhs.x12+x12*rhs.x22;
		ret.x21 = x21*rhs.x11+x22*rhs.x21;
		ret.x22 = x21*rhs.x12+x22*rhs.x22;
		return ret;
	}
	Float2x1	operator*(Float2x1& rhs)	//matrix*array
	{
		Float2x1 ret; 
		ret.x11 = x11*rhs.x11+x12*rhs.x21;
		ret.x21 = x21*rhs.x11+x22*rhs.x21;
		return ret;
	}
	void		scale(float coeff)			{x11=x11*coeff; x12=x12*coeff; x21=x21*coeff; x22=x22*coeff;}//change itself
	float		det()						{return (x11*x22-x21*x12);}//won't change itself
	Float2x2	inv()						//won't change itself
	{ 
		if(abs((double)det()) < 0.00001)
		{Float2x2 ret; return ret;} 
		Float2x2 ret(x22, -x12, -x21, x11);
		ret.scale(1.0/det());
		return ret;
	}
	Float2x2	transp()					//won't change itself
	{
		Float2x2 ret(x11,x21,x12,x22);
		return ret;
	}
};

Float2x2 obsVarR_Est2x2(CvPoint obsPos)
{
	// here we implement Mean-Square observation distance (to previous observation) 
	static int obsCnt = 0; // this line only goes once, initialization
	static Float2x1 preDistSqArr[PRE_DIST_ARR_LENGTH]; //init as all 0
	static CvPoint preObsPos = obsPos; // this line only goes once, initialization


	//if (obsCnt < PRE_DIST_ARR_LENGTH) // 0,...,PRE_DIST_ARR_LENGTH-1
	// now, it's OK for all cases
	{
		if (obsCnt >= PRE_DIST_ARR_LENGTH) obsCnt = PRE_DIST_ARR_LENGTH-1; // full storage

		//update buffer array
		Float2x1 curDistSq;
		int i = 0;
		for (; i<obsCnt; i++)
		{
			preDistSqArr[obsCnt-i] = preDistSqArr[obsCnt-1-i];
		}
		if (obsCnt != 0)
		{
			//not the first call
			curDistSq.x11 = pow((double)(obsPos.x-preObsPos.x), 2);
			curDistSq.x21 = pow((double)(obsPos.y-preObsPos.y), 2);
			preDistSqArr[0] = curDistSq;
		}
		else
			;//keep preDistSqArr[0] and curDistSq as 0
		
		//calculate obsVarR
		Float2x1 obsVarR; 
		for (i=0; i<obsCnt+1; i++)
		{
			obsVarR = obsVarR + preDistSqArr[i];
		}
		obsVarR.x11 = obsVarR.x11/(obsCnt+1);
		obsVarR.x21 = obsVarR.x21/(obsCnt+1);
		if (obsCnt == 0) 
		{ obsVarR.x11 = obsVarR.x21 = 10.0;}

		printf("Dist^2=(%.1f,%.1f),ObsVar=(%.1f,%.1f),", curDistSq.x11, curDistSq.x21, obsVarR.x11, obsVarR.x21);

		preObsPos = obsPos;
		++obsCnt;
		//return obsVarR;
		Float2x2 ret(obsVarR.x11,0,0,obsVarR.x21);
		return ret;
	}

}

Float2x2 varQ2x2(10, 0, 0, 10);

CvPoint kalmanFilterPosEstimator2x2(CvPoint obsPos)
{
	Float2x1 obs(obsPos.x, obsPos.y);
	// transition model(A) = I2x2,
	Float2x2 tranA(1,0,0,1);
	// input model(B,u) = 0,
	// measurement model(H) = I2x2,
	Float2x2 measH(1,0,0,1);
	Float2x2 I2x2(1,0,0,1);
	// 
	static Float2x1 curState(IMG_WIDTH/2, IMG_HEIGHT/2); // this line only goes once, initialization
	static Float2x2 kalmanGainK; // this line only goes once, initialization as O2x2
	static Float2x2 errVarianceP; // this line only goes once, initialization as O2x2

	//Project the state ahead
	curState = tranA*curState;
	//Project the error covariance ahead
	errVarianceP = tranA*errVarianceP*tranA.transp() + varQ2x2;

	//calculate kalmanGainK
	kalmanGainK = errVarianceP*measH.transp()*(measH*errVarianceP*measH.transp() + obsVarR_Est2x2(obsPos)).inv();
	// Update state with measurement
	curState = curState + kalmanGainK*(obs - measH*curState);
	// Update error covariance
	errVarianceP = (I2x2 - kalmanGainK*measH)*errVarianceP;

	//print detail status
	printf("GainK=(%.1f,%.1f),", kalmanGainK.x11, kalmanGainK.x22);
	printf("ErrVar=(%.1f,%.1f)\n", errVarianceP.x11, errVarianceP.x22);

	return cvPoint(curState.x11, curState.x21);
}


CvPoint windowMeanShift(IplImage* bp, CvPoint ctr)
{
	double value;
	int sumX = 0, sumY = 0, pointCounter = 0;
	for (int i=max(0, ctr.x-SEARCH_RANGE); i<min(IMG_WIDTH, ctr.x+SEARCH_RANGE); i++)
	{
		for (int j=max(0, ctr.y-SEARCH_RANGE); j<min(IMG_HEIGHT, ctr.y+SEARCH_RANGE); j++)
		{
			value = CV_IMAGE_ELEM( bp, unsigned char, j, i );
			if (value > 0) 
			{
				sumX += i;
				sumY += j;
				pointCounter++;
			}
		}
	}
	CvPoint meanLoc;
	meanLoc.x = sumX/(pointCounter ? pointCounter : 1);
	meanLoc.y = sumY/(pointCounter ? pointCounter : 1);
	return meanLoc;
}
CvPoint maxLikelihood(IplImage* bp, CvPoint ctr)
{
	double value;
	int pointCounter=0, maxCnt = 0;
	CvPoint bestLoc, tmpCtr;

	for (int x=max(0, ctr.x-ML_SEARCH_RANGE); x<min(IMG_WIDTH, ctr.x+ML_SEARCH_RANGE); x+=ML_SEARCH_GRID)
	{
		for (int y=max(0, ctr.y-ML_SEARCH_RANGE); y<min(IMG_HEIGHT, ctr.y+ML_SEARCH_RANGE); y+=ML_SEARCH_GRID)
		{
			tmpCtr = cvPoint(x, y);
			pointCounter = 0;

			for (int i=max(0, tmpCtr.x-SEARCH_RANGE); i<min(IMG_WIDTH, tmpCtr.x+SEARCH_RANGE); i++)
			{
				for (int j=max(0, tmpCtr.y-SEARCH_RANGE); j<min(IMG_HEIGHT, tmpCtr.y+SEARCH_RANGE); j++)
				{
					value = CV_IMAGE_ELEM( bp, unsigned char, j, i );
					if (value > 0) 
					{
						pointCounter++;
					}
				}
			}
			if (pointCounter > maxCnt)
			{
				maxCnt = pointCounter;
				bestLoc = tmpCtr;
			}
		}
	}

	return bestLoc;
}
CvPoint sampleMean(IplImage* bp, CvPoint ctr)
{
	double value;
	long int sumX = 0, sumY = 0, pointCounter = 0;
	for (int i=0; i<IMG_WIDTH; i++)
	{
		for (int j=0; j<IMG_HEIGHT; j++)
		{
			value = CV_IMAGE_ELEM( bp, unsigned char, j, i );
			if (value > 0) 
			{
				sumX += i;
				sumY += j;
				pointCounter++;
			}
		}
	}
	CvPoint meanLoc;
	meanLoc.x = sumX/pointCounter;
	meanLoc.y = sumY/pointCounter;
	return meanLoc;
}



// IplImage *frame_HSV, *frame_H, *frame_S, *frame_V, *backProject, *ValMsk, *backProject_temp are all allocated & available to use
void impMap_HBack_fixTH_valMsk(IplImage* frame_RGB, int avgVal)
{
		cvCvtColor(frame_RGB, frame_HSV, CV_BGR2HSV);
		cvSplit(frame_HSV, frame_H, frame_S, frame_V, 0);

		cvCalcBackProject(&frame_H, backProject, hist);
		cvShowImage("backProject", backProject);

		cvThreshold(backProject, backProject, BACKPROJECT_TH, 255, CV_THRESH_BINARY);

		cvZero(ValMsk);
		cvInRangeS(frame_V, cvScalar(avgVal - VAL_RANGE), cvScalar(avgVal + VAL_RANGE), ValMsk);
		cvZero(backProject_temp);
		cvCopy(backProject, backProject_temp, ValMsk);
}

void impMap_HBack_fixTH(IplImage* frame_RGB, int THD)
{
		cvCvtColor(frame_RGB, frame_HSV, CV_BGR2HSV);
		cvSplit(frame_HSV, frame_H, frame_S, frame_V, 0);

		cvCalcBackProject(&frame_H, backProject, hist);
		cvShowImage("backProject", backProject);

		cvThreshold(backProject, backProject_temp, THD, 255, CV_THRESH_BINARY);

}

void impMap_HBack_adaptiveTH(IplImage* frame_RGB)
{
		unsigned int sumX = 0;
		double sumXsq = 0;
		unsigned int imgVal = 0;
		int i=0, j=0;
		int BackProj_point_cnt = 0;
		int EX, EXsq, total_EX, total_EXsq, VarX, total_VarX;
	
		cvCvtColor(frame_RGB, frame_HSV, CV_BGR2HSV);
		cvSplit(frame_HSV, frame_H, frame_S, frame_V, 0);

		cvCalcBackProject(&frame_H, backProject, hist);
		cvShowImage("backProject", backProject);

		for (i=0; i<cvGetSize(frame_RGB).width; i++)
		{
			for (j=0; j<cvGetSize(frame_RGB).height; j++)
			{
				imgVal = CV_IMAGE_ELEM( backProject, unsigned char, j, i );
				imgVal = (imgVal > 255*RATIO_THD_BACKPROJ_NOISE) ? imgVal : 0; 
				if (imgVal)
				{
					sumX += imgVal;
					sumXsq += imgVal*imgVal;
					BackProj_point_cnt += 1;
				}
			}
		}
		EX = sumX/BackProj_point_cnt;
		EXsq = sumXsq/BackProj_point_cnt;
		total_EX = sumX/(i*j);
		total_EXsq = sumXsq/(i*j);

		VarX = EXsq - EX*EX;
		total_VarX = total_EXsq - total_EX*total_EX;

		cvThreshold(backProject, backProject_temp, EX-sqrt((double)VarX), 255, CV_THRESH_BINARY);
}
