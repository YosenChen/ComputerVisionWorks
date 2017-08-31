#ifndef TrackingState_H
#define TrackingState_H

#include "ProcessState.h"
#include "TrackingResult.h"

class TrackingState : public ProcessState {
public:
    void enter( OpticalFlow* SingletonInstancePtr );
    void exit( void );

    void pause( void );
    void resume( void );
    void update( unsigned long lTimeElapsed );

	bool execute( IplImage* imageSource);

	static TrackingState* getSingletonPtr( void ) 
	{ 
		//if( mTrackingState == 0 )
		//	mTrackingState = new TrackingState;

		return &mTrackingState;
	}
private:
	time_t mStartTimeStamp;
	time_t mEndTimeStamp;
	time_t mStartTimeStampLosing;
	time_t mEndTimeStampLosing;
	int mLosingCount;
	int send2ServerCnt;

	double color_score;
	double best_color_dist;
	int best_color_index;
	int matchCnt;
	int Lv;
	double intersect_Z;
	double colorScoreMap[MAX_TARGET_NUM][MAX_TARGET_NUM];
	int objConfMap[MAX_TARGET_NUM][MAX_TARGET_NUM];
	int matchCntMap[MAX_TARGET_NUM][MAX_TARGET_NUM];
	Position3D headpt1, headpt2;
	int total;
	bool occupiedList_cam1[MAX_TARGET_NUM], occupiedList_cam2[MAX_TARGET_NUM];
	
	CamLineMgr_CAM1_OB1 camlinemgr11;
	CamLineMgr_CAM1_OB2 camlinemgr12;
	CamLineMgr_CAM1_OB3 camlinemgr13;
	CamLineMgr_CAM1_OB4 camlinemgr14;
	CamLineMgr_CAM1_OB5 camlinemgr15;
	CamLineMgr_CAM2_OB1 camlinemgr21;
	CamLineMgr_CAM2_OB2 camlinemgr22;
	CamLineMgr_CAM2_OB3 camlinemgr23;
	CamLineMgr_CAM2_OB4 camlinemgr24;
	CamLineMgr_CAM2_OB5 camlinemgr25;
	CamLineMgr_CAM3_OB1 camlinemgr31;
	CamLineMgr_CAM3_OB2 camlinemgr32;
	CamLineMgr_CAM3_OB3 camlinemgr33;
	CamLineMgr_CAM3_OB4 camlinemgr34;
	CamLineMgr_CAM3_OB5 camlinemgr35;

	LabelIntegrateMgr_CAM1 IntegrateStatus;

	IplImage *Cr_down4x4, *Cb_down4x4, *Y_down4x4;


	void sendControlSignal( TRACKING_RESULT result );
	TRACKING_RESULT objectTrackingOperation();
	void SolveObservationIntegration();
	void SolveObservationIntegration_greedy();
	void SendObservationInfo(int CAMID);
	void SolveObservationIntegration_jointMax(){};
	void DrawObservationStatus(int CAMID);
	bool checkReInitialization();
	bool correspondenceLikelihoodThresholding(int g, int f);
	
	
    TrackingState() 
	{ 
		mLosingCount = 0;
		Cr_down4x4 = cvCreateImage(cvSize(80, 60), 8, 1);
		Cb_down4x4 = cvCreateImage(cvSize(80, 60), 8, 1);
		Y_down4x4 = cvCreateImage(cvSize(80, 60), 8, 1);
	}

    ~TrackingState() 
	{ 
	
	}

    static TrackingState    mTrackingState;
};

#endif