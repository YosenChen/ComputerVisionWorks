#ifndef ReTrackingState_H
#define ReTrackingState_H

#include "ProcessState.h"

class ReTrackingState : public ProcessState {
public:
    void enter( OpticalFlow* SingletonInstancePtr );
    void exit( void );

    void pause( void );
    void resume( void );
    void update( unsigned long lTimeElapsed );

	bool execute( IplImage* imageSource );

	static ReTrackingState* getSingletonPtr( void ) 
	{ 
		return &mReTrackingState;
	}
private:
	time_t mStartTimeStamp;
	time_t mEndTimeStamp;
	CvPoint2D32f mTrackingBoxCenter;
	bool mPauseFinishFlag;
	static const double RECOGNIZING_TIME;

	void reset();
	void drawLosingBox();
	bool waitForReTracking();

	ReTrackingState() :
		mStartTimeStamp(0),
		mEndTimeStamp(0),
		mPauseFinishFlag(false)
	{ 
	
	}

    ~ReTrackingState() 
	{ 
	
	}

    static ReTrackingState    mReTrackingState;
};
#endif