#include "CheckingState.h"
#include "TrackingState.h"
//#include "C_PM.h"

CheckingState CheckingState::mCheckingState;
double CheckingState::RECOGNIZING_TIME = 3.0;

void CheckingState::enter( OpticalFlow* SingletonInstancePtr )
{
	mSingletonInstancePtr = SingletonInstancePtr;
	reset();
	initChecking();
	
	time (&mStartTimeStamp);
//	mSingletonInstancePtr->Cam->mPM->getParticleTrack()->clearAllTracks();
}

void CheckingState::exit( void )
{
}

void CheckingState::pause( void )
{
}

void CheckingState::resume( void )
{
}

void CheckingState::update( unsigned long lTimeElapsed )
{
}


void CheckingState::initChecking()
{
	mSingletonInstancePtr->mNormalMessage += string( "Motion Detected!\n" );
	mSingletonInstancePtr->mNormalMessage += string( "please put your interacting object in the box\n" );
	mSingletonInstancePtr->center = cvPoint( (mMiddleX=(mSingletonInstancePtr->left+mSingletonInstancePtr->right)/2), (mMiddleY=(mSingletonInstancePtr->top > mSingletonInstancePtr->mHeight-0.5*tar_square) ? (mSingletonInstancePtr->mHeight-static_cast<int>(0.5*tar_square)) : ((mSingletonInstancePtr->top < static_cast<int>(0.5*tar_square)) ? static_cast<int>(0.5*tar_square) : mSingletonInstancePtr->top)) );
}

void CheckingState::drawCheckingBox()
{
	cvCircle(mSingletonInstancePtr->result, mSingletonInstancePtr->center, 1, CV_RGB(0,255,255), 3);
	cvCircle(mSingletonInstancePtr->result, mSingletonInstancePtr->center, 10, CV_RGB(255,20,200), 2);
	cvRectangle( mSingletonInstancePtr->result, cvPoint(mMiddleX-static_cast<int>(0.3*tar_square), mMiddleY-static_cast<int>(0.75*tar_square)),
		cvPoint(mMiddleX+static_cast<int>(0.3*tar_square), mMiddleY+static_cast<int>(0.75*tar_square)), CV_RGB(0,255,255), 2, 8, 0);
	cvPutText( mSingletonInstancePtr->result, "INITIAL...", cvPoint(mMiddleX-30/*mSingletonInstancePtr->left*/, mMiddleY-static_cast<int>(0.8*tar_square)), &(mSingletonInstancePtr->Tfont), CV_RGB(0,255,255) );
}

bool CheckingState::waitForChecking()
{
	time (&mEndTimeStamp);

	if( difftime( mEndTimeStamp, mStartTimeStamp ) >= RECOGNIZING_TIME ) // jump to trackingState
	{
		return false;
	}

	return true;
}

bool CheckingState::setupRecognizedImageROI()
{
	//以下建立影像的roi區域
	mTar_box.x = (mMiddleX-static_cast<int>(0.2/*0.3*/*tar_square) > 0) ? mMiddleX-static_cast<int>(0.2/*0.3*/*tar_square) : 0;
	mTar_box.y = (mMiddleY-static_cast<int>(0.5/*0.3*/*tar_square) > 0) ? mMiddleY-static_cast<int>(0.5/*0.3*/*tar_square) : 0;
	mTar_box.height = (mTar_box.y+1.0/*0.6*/*tar_square < mSingletonInstancePtr->mHeight) ? static_cast<int>(1.0/*0.6*/*tar_square) : mSingletonInstancePtr->mHeight-mTar_box.y;
	mTar_box.width = (mTar_box.x+static_cast<int>(0.4/*0.6*/*tar_square) < mSingletonInstancePtr->mWidth) ? static_cast<int>(0.4/*0.6*/*tar_square) : mSingletonInstancePtr->mWidth-mTar_box.x;
	cvZero(mSingletonInstancePtr->RgnRegion);
	cvRectangle(mSingletonInstancePtr->RgnRegion, cvPoint(mTar_box.x, mTar_box.y), cvPoint(mTar_box.x+mTar_box.width, mTar_box.y+mTar_box.height), cvScalar(255), -1);

	//將剛剛建立的roi區域拿去當camshift的初始追蹤目標
	bool checking_result = mSingletonInstancePtr->Cam->initCamShiftChecking1( mSingletonInstancePtr->result, mSingletonInstancePtr->RgnRegion, mTar_box );
//	cvRectangle( mSingletonInstancePtr->result, cvPoint(mTar_box.x, mTar_box.y), cvPoint(mTar_box.x+mTar_box.width, mTar_box.y+mTar_box.height), CV_RGB( 0, 0, 255), 2, 8, 0 );

	cvPutText( mSingletonInstancePtr->result, "CLOSER...", cvPoint(mMiddleX-30/*mSingletonInstancePtr->left*/, mMiddleY-static_cast<int>(1.0*tar_square)), &(mSingletonInstancePtr->Tfont), CV_RGB(0,0,255) );
	time (&mEndTimeStamp);


	if (mSingletonInstancePtr->fgmask) 
		cvReleaseImage(&(mSingletonInstancePtr->fgmask));
	mSingletonInstancePtr->fgmask = 0;
	return checking_result;
}

bool CheckingState::execute( IplImage* imageSource)
{
	//直接跳級
	mTar_box = cvRect(0,0, mSingletonInstancePtr->mWidth,mSingletonInstancePtr->mHeight);
	cvZero(mSingletonInstancePtr->RgnRegion);
	cvRectangle(mSingletonInstancePtr->RgnRegion, cvPoint(mTar_box.x, mTar_box.y), cvPoint(mTar_box.x+mTar_box.width, mTar_box.y+mTar_box.height), cvScalar(255), -1);
	mSingletonInstancePtr->Cam->initCamShiftChecking( mSingletonInstancePtr->result, mSingletonInstancePtr->RgnRegion, mTar_box );
	mSingletonInstancePtr->fgmask = 0;
	changeState( TrackingState::getSingletonPtr() );
	mSingletonInstancePtr->prevReturnROI = cvRect(0,0,mSingletonInstancePtr->mWidth,mSingletonInstancePtr->mHeight); // for initializing the ROI
	return false;
	//

//	drawCheckingBox();

	if( waitForChecking() == false ) // jump to trackingState
	{
		
		if (setupRecognizedImageROI() == true)
		{
			changeState( TrackingState::getSingletonPtr() );
			mSingletonInstancePtr->prevReturnROI = mTar_box; // for initializing the ROI

			return false;
		}
		else 
		{
			;
		}
	}
	drawCheckingBox();
	cvPutText( mSingletonInstancePtr->result, "PUT TARGET IN", cvPoint(5/*mSingletonInstancePtr->result->width-130*/, 18), &(mSingletonInstancePtr->statefont), CV_RGB(0,255,0) );
	cvPutText( mSingletonInstancePtr->result, "TO FIT THE BOX", cvPoint(5/*mSingletonInstancePtr->result->width-130*/, 45), &(mSingletonInstancePtr->statefont), CV_RGB(0,255,0) );

	return true;
}

void CheckingState::reset()
{
	mMiddleX = 0;
	mMiddleY = 0;
}