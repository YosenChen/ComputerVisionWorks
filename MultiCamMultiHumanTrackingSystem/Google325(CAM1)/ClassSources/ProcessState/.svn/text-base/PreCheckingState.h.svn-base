#ifndef PreCheckingState_H
#define PreCheckingState_H

#include "ProcessState.h"

class PreCheckingState : public ProcessState {
public:
    void enter( OpticalFlow* SingletonInstancePtr );
    void exit( void );

    void pause( void );
    void resume( void );
    void update( unsigned long lTimeElapsed );

	bool execute( IplImage* imageSource );
	bool waifForTempStatePass();


	static PreCheckingState* getSingletonPtr( void ) 
	{ 
		//if( mPreCheckingState == 0 )
		//	mPreCheckingState = new PreCheckingState;

		return &mPreCheckingState;
	}

private:
	IplImage *mSwap_temp;
	bool mStartCheckingFlag;
	time_t mStartTimeStamp;
	time_t mEndTimeStamp;
	time_t mStartTimeStampRe; // for re-initialization count
	time_t mEndTimeStampRe;
	int mLoopnum;
	//int /mStartCnt;
	int mFlags;
	int mCountnum;
	static const int MAX_COUNT = 500;
	static const int TIME = 3;
	static const int WIN_SIZE = 10;
	static const int THFACTOR = 3;
	static const int MOVE_AVGVTH = 30;
	static const int MIN_NO_FEATURE_PT = 25;
	static const int MASKR = 14;
	static const double REINITIALIZATION_TIME;
	static const double TEMP_STATE_PASS_TIME;
	static const int TOL_DIS;
	float mPointdis[MAX_COUNT];
	char* mStatus;
	CvPoint2D32f* mSwap_points;
	CvPoint2D32f* mPoints[TIME];
	double mMoveAvgV;
	float mPointavgdis;
	int mLargeMove;

	void reset();
	void nonMotionDetectionOperationExtracted();
	void initChecking();
	bool canStartObjectChecking();
	bool SetPointavgdis();
	void spillPointsAndShow();
	void preProcessing();


	void AutomaticInitializeTracking();
    
	PreCheckingState() :
		mStartCheckingFlag(true),
		mStartTimeStamp(0),
		mEndTimeStamp(0),
		mStartTimeStampRe(0),
		mEndTimeStampRe(0),
		mLoopnum(0),
		///mStartCnt(0),
		mFlags(0),
		mCountnum(0),
		mStatus(0),
		mSwap_points(0),
		mMoveAvgV(0),
		mPointavgdis(0),
		mLargeMove(0),
		mSwap_temp(0)

	{ 
		for( int i=0; i<MAX_COUNT; i++ )
			mPointdis[i] = 0;

		for( int i=0; i<TIME; i++ ){
			mPoints[i] = 0;
			mPoints[i] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(mPoints[0][0]));
		}
		mStatus = (char*)cvAlloc(MAX_COUNT);
	}

    ~PreCheckingState() 
	{ 
		for( int i=0; i<TIME; i++ ){

			cvFree( &mPoints[i] );
		}
		cvFree( &mStatus );	
	}

    static PreCheckingState    mPreCheckingState;
};
#endif