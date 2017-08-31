#ifndef CheckingState_H
#define CheckingState_H

#include "ProcessState.h"

class CheckingState : public ProcessState
{
public:
    void enter( OpticalFlow* SingletonInstancePtr );
    void exit( void );

    void pause( void );
    void resume( void );
    void update( unsigned long lTimeElapsed );

	bool execute( IplImage* imageSource );

	static CheckingState* getSingletonPtr( void )
	{
		return &mCheckingState;
	}
private:
	time_t mStartTimeStamp;
	time_t mEndTimeStamp;
	CvRect mTar_box;
	int mMiddleX;
	int mMiddleY;

	static double RECOGNIZING_TIME; // seconds

	void reset();
	void initChecking();
	void CheckingState::drawCheckingBox();
	bool waitForChecking();
	bool setupRecognizedImageROI();
    
	CheckingState() :
		mStartTimeStamp(0),
		mEndTimeStamp(0),
		mMiddleX(0),
		mMiddleY(0)
	{ 
	
	}

    ~CheckingState() 
	{
	}

    static CheckingState    mCheckingState;
};
#endif