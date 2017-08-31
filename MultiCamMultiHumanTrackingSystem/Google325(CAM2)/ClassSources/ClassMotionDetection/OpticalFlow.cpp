#include "OpticalFlow.h"
#include "ClassOpenCVMat.h"
#include "PreCheckingState.h"
#include <sstream>
#include "C_WinSocket.h"

OpticalFlow* OpticalFlow::mSingletonInstancePtr(0);
const double OpticalFlow::RECONNECT_TIME = 3.0;
const int OpticalFlow::RESEND_COUNT = 3;

OpticalFlow::OpticalFlow( /*C_PM* cPM, C_WinSocketClient* client,*/ int iWidth, int iHeight, int camNo ) : 
	//image(0), 
	grey(0),
	RgnRegion(0),
	prev_grey(0), 
	pyramid(0), 
	prev_pyramid(0), 
	blob(0), 
	//result[CAM1](0),
	//result[CAM2](0),
	night_mode(0),	
	fgmask(0),
	mReSendCount(0)/*,
	mClient(client)*/
{
	image = result = NULL;
	mWidth = iWidth;
	mHeight = iHeight;
	CAMShift* tempptr = CAMShift::getSingletonPtr( /*cPM,*/ mWidth, mHeight, camNo );
	Cam = tempptr;
	prevReturnROI.x = prevReturnROI.y = prevReturnROI.height = prevReturnROI.width = 0;
	//mClient = new C_WinSocketClient();
	img_type = ".jpg";
	prefixRes = (camNo == CAM1) ? "[CAM1]Res" : (camNo == CAM2) ? "[CAM2]Res" : "[CAM3]Res";
	prefixSrc = (camNo == CAM1) ? "[CAM1]Src" : (camNo == CAM2) ? "[CAM2]Src" : "[CAM3]Src";
	imageSamplingCtr = 0;
	takeSeqPhoto = false;
}

OpticalFlow::~OpticalFlow()
{
	 cvReleaseImage( &image );
	 cvReleaseImage( &grey );
	 cvReleaseImage( &prev_grey );
	 cvReleaseImage( &pyramid );
	 cvReleaseImage( &RgnRegion );
	 cvReleaseImage( &prev_pyramid );

	 cvReleaseImage( &blob );
	 cvReleaseImage( &result );
	 cvReleaseImage( &fgmask );

	 cvDestroyAllWindows();

	 delete mMatTempImage;
	 delete mMatTempResult;
	 //delete mClient;

	 Cam->sendControlSignal( 0.2, DISCONNECT );
	 CAMShift::destroyInstance();
}

void OpticalFlow::initialize( 
		ClassOpenCVMat  *matColorDisplayImage,
		ClassOpenCVMat  *matColorDisplayResult )
{   
	mMatColorDisplayImage = matColorDisplayImage;
	mMatColorDisplayResult = matColorDisplayResult;

	mMatTempImage=new ClassOpenCVMat;
	mMatTempImage->mCreateRGBUCMat(mWidth,mHeight);

	mMatTempResult=new ClassOpenCVMat;
	mMatTempResult->mCreateRGBUCMat(mWidth,mHeight);

	Cam->init( mClient, &mNormalMessage, &mControlMessage );

	cvInitFont( &Tfont, CV_FONT_HERSHEY_PLAIN, 1.0, 1.0, 0, 2);
	cvInitFont( &statefont, CV_FONT_HERSHEY_PLAIN, 1.1, 1.1, 0, 2);
	cvInitFont( &testfont, CV_FONT_HERSHEY_PLAIN, 0.7, 0.7, 0, 1 );
}

void OpticalFlow::initializeImage(IplImage *frame)
{
    /* allocate all the buffers */
 ////////////////////////////////////////////////////////////////////////////////////////
	image = cvCreateImage( cvGetSize(frame), 8, 3 );
    image->origin = frame->origin;
	grey = cvCreateImage( cvGetSize(frame), 8, 1 );
    prev_grey = cvCreateImage( cvGetSize(frame), 8, 1 );
    pyramid = cvCreateImage( cvGetSize(frame), 8, 1 );
    prev_pyramid = cvCreateImage( cvGetSize(frame), 8, 1 );
	blob = cvCreateImage( cvGetSize(frame), 8, 1 );
	blob->origin = frame->origin;
	RgnRegion= cvCreateImage(cvGetSize(frame),8,1);
	//fgmask = cvCreateImage( cvGetSize(result), 8, 1 );

	result = cvCreateImage( cvGetSize(frame), 8, 3 );
	result->origin = frame->origin;
}
template <class T>
inline std::string to_string(const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}
void OpticalFlow::showImages()
{
	imageSamplingCtr++;
//	cvWriteFrame(srcVideo, image);
//	cvWriteFrame(resVideo, result);
	CvMat* pDesImage = cvGetMat( image, mMatTempImage->mpMat );
	CvMat* pDesResult = cvGetMat( result, mMatTempResult->mpMat );

	cvCopy( pDesImage, mMatColorDisplayImage->mpMat );
	cvCopy( pDesResult, mMatColorDisplayResult->mpMat );

	if (imageSamplingCtr%8 || !takeSeqPhoto) return;
	image_name.clear();
	image_name = prefixRes + to_string(imageSamplingCtr) + img_type;
	cvSaveImage(image_name.c_str(), result);
	image_name.clear();
	image_name = prefixSrc + to_string(imageSamplingCtr) + img_type;
	cvSaveImage(image_name.c_str(), image);
}

void OpticalFlow::resetAllParameters()
{ 
	night_mode= 0;	
}

void OpticalFlow::startProcess( IplImage* imageSource )
{
	if( !imageSource)
		return;

	if( !image)
	{
		initializeImage(imageSource);
	}

	cvCopy( imageSource, image, 0 );
	cvCopy( imageSource, result, 0 );
	cvCvtColor( image, grey, CV_BGR2GRAY );

	if( night_mode )
		cvZero( image );

	if( mStates.empty() == false )
		mStates.back()->execute( imageSource );

	showImages();
}

void OpticalFlow::activateMotionDetection()
{
	if( !image )
		return;

	resetAllParameters();

	changeState( PreCheckingState::getSingletonPtr() );
}

void OpticalFlow::activateDeletingPoints()
{
	if( !image )
		return;
}

void OpticalFlow::activateNightMode()
{
	if( !image )
		return;

    night_mode ^= 1;
}

//bool OpticalFlow::connect( char *ip, int port )
//{
//	mIp = ip;
//	mPort = port;
//
//	mClient->mInitialize( ip, port, 600 );
//
//	if( !mClient->mbInitialized )
//	{		
//		mNormalMessage += string( "connection fail\n" );
//		return false;
//	}
//	mNormalMessage += string( "connection succeed\n" );
//	mHasConncected = true;
//	return true;
//}


void OpticalFlow::changeState(ProcessState* state )
{
	// cleanup the current state
	if ( !mStates.empty() ) {
		mStates.back()->exit();
		mStates.pop_back();
	}

	// store and init the new state
	mStates.push_back(state);

	mStates.back()->enter( mSingletonInstancePtr );
}

void OpticalFlow::pushState(ProcessState* state)
{
	// pause current state
	if ( !mStates.empty() ) {
		mStates.back()->pause();
	}

	// store and init the new state
	mStates.push_back(state);
	mStates.back()->enter( mSingletonInstancePtr );
}

void OpticalFlow::popState()
{
	// cleanup the current state
	if ( !mStates.empty() ) {
		mStates.back()->exit();
		mStates.pop_back();
	}

	// resume previous state
	if ( !mStates.empty() ) {
		mStates.back()->resume();
	}
}