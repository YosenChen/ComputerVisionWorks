#ifndef __H__OPTICALFLOW__
#define __H__OPTICALFLOW__

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <ctime>

#include "CAMShift.h"
#include <direct.h>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;

//static fstream fileOut;

#define tolDis 100
#define tar_square 80


//////////////////////////////////////////////////error while deleting this line of code////////////////////////////////////
class ProcessState;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ClassOpenCVMat;
class C_WinSocketClient;
class CAMShift;
//class C_PM;
class VisionTranslate;

class OpticalFlow
{
private:
	ClassOpenCVMat             *mMatColorDisplayImage;
	ClassOpenCVMat             *mMatColorDisplayResult;
	ClassOpenCVMat             *mMatTempResult;
	ClassOpenCVMat             *mMatTempImage;

	time_t mStartTimeStampReSend;
	time_t mEndTimeStampReSend;
	int mReSendCount;

	static const double RECONNECT_TIME;
	static const int RESEND_COUNT;

	void initializeImage(IplImage *frame);
	void resetAllParameters();

	vector<ProcessState*> mStates;

	OpticalFlow( /*C_PM* cPM, C_WinSocketClient* client=0,*/ int iWidth=320, int iHeight=240, int camNo=CAM1 );
    ~OpticalFlow();

	static OpticalFlow* mSingletonInstancePtr;

private:	
	int night_mode;

public:
// common
	// all
	int mWidth;
	int mHeight;
	CvRect	prevReturnROI;
	IplImage *image, *grey, *prev_grey, *pyramid, *prev_pyramid/*, *swap_temp*/;
	IplImage *blob, *result, *fgmask, *RgnRegion;
	CvVideoWriter				*srcVideo;
	CvVideoWriter               *resVideo;
	bool takeSeqPhoto;

	C_WinSocketClient *mClient;
	CAMShift *Cam;
	int top, bottom, right, left; // preCheckingState, checkingState;
	CvPoint center; // checkingState, preCheckingState
	CvFont testfont; // initialize, preCheckingState
	CvFont Tfont; // initialize, &&??
	CvFont statefont;
	string mNormalMessage;
	string mControlMessage;
	string img_type, image_name;
	string prefixRes;
	string prefixSrc;
	int imageSamplingCtr;

	void initialize( 
		ClassOpenCVMat  *matColorDisplayImage,
		ClassOpenCVMat  *matColorDisplayResult );

	void startProcess( IplImage* imageSource );

	void showImages();

	void activateMotionDetection();
	void activateDeletingPoints();
	void activateNightMode();

	string* getNormalMessage()
	{
		return &mNormalMessage;
	}

	string* getControlMessage()
	{
		return &mControlMessage;
	}

	static OpticalFlow* getSingletonPtr( /*C_PM* cPM=0,*/ /*C_WinSocketClient* client=0,*/ int width=320, int height=240, int camNo=CAM1 )
	{
		//fileOut.open("error report.txt",ios::out);

		if( mSingletonInstancePtr == 0 )
			mSingletonInstancePtr = new OpticalFlow(/* cPM, client,*/ width, height, camNo );

        return mSingletonInstancePtr;
	}

	static void	destroyInstance()    
	{  
		delete   mSingletonInstancePtr;  
		mSingletonInstancePtr=0;  
	}   

	void changeState(ProcessState* state );
	void pushState(ProcessState* state);
	void popState();
};

#endif
