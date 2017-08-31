#include "ReTrackingState.h"
#include "TrackingState.h"
//#include "C_PM.h"

ReTrackingState ReTrackingState::mReTrackingState;
const double ReTrackingState::RECOGNIZING_TIME = 2.0;

void ReTrackingState::enter( OpticalFlow* SingletonInstancePtr )
{
	mSingletonInstancePtr = SingletonInstancePtr;
	reset();
	mTrackingBoxCenter = mSingletonInstancePtr->Cam->getTrack_box().center;
	time(&mStartTimeStamp);
}

void ReTrackingState::exit( void )
{
}

void ReTrackingState::pause( void )
{
}

void ReTrackingState::resume( void )
{
}

void ReTrackingState::update( unsigned long lTimeElapsed )
{
}

void ReTrackingState::reset()
{
	mPauseFinishFlag = false;
}

void ReTrackingState::drawLosingBox()
{	
	cvCircle(mSingletonInstancePtr->result, cvPointFrom32f(mTrackingBoxCenter), 1, CV_RGB(128,200,20), 3);
	cvRectangle( mSingletonInstancePtr->result, cvPoint( static_cast<int>(mTrackingBoxCenter.x-0.5*tar_square), static_cast<int>(mTrackingBoxCenter.y-0.5*tar_square)),
		cvPoint(static_cast<int>(mTrackingBoxCenter.x+0.5*tar_square), static_cast<int>(mTrackingBoxCenter.y+0.5*tar_square)), CV_RGB(120,40,160), 3, 8, 0);
	cvPutText( mSingletonInstancePtr->result, "PUT HERE", cvPoint(static_cast<int>(mTrackingBoxCenter.x-0.5*tar_square),
		static_cast<int>(mTrackingBoxCenter.y-0.7*tar_square)), &(mSingletonInstancePtr->Tfont), CV_RGB(120,40,160) );
}

bool ReTrackingState::waitForReTracking()
{
	time(&mEndTimeStamp);

	if( difftime( mEndTimeStamp, mStartTimeStamp ) >= RECOGNIZING_TIME ) /*if( g == recognizingTime-1 )*/
	{
		return false;
	}

	return true;
}

bool ReTrackingState::execute( IplImage* imageSource )
{
	drawLosingBox();
        cvPutText( mSingletonInstancePtr->result, "PUT TARGET BACK", cvPoint(5/*mSingletonInstancePtr->result->width-150*/, 18), &(mSingletonInstancePtr->statefont), CV_RGB(255,255,0) );
	if( waitForReTracking() == true )
	{
		return true;
	}
	else
	{
		changeState( TrackingState::getSingletonPtr() );

		return false;
	}

	return true;
}
