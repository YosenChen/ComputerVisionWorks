#include "PreCheckingState.h"
#include "CheckingState.h"

PreCheckingState PreCheckingState::mPreCheckingState;
const double PreCheckingState::REINITIALIZATION_TIME = 4.0;
const double PreCheckingState::TEMP_STATE_PASS_TIME = 2.0;
const int PreCheckingState::TOL_DIS = 100;

void PreCheckingState::enter( OpticalFlow* SingletonInstancePtr )
{
	mSingletonInstancePtr = SingletonInstancePtr;
	reset();
	mSingletonInstancePtr->Cam->reInit();
	mSingletonInstancePtr->Cam->sendControlSignal( 0.2, REGRESSION );

	AutomaticInitializeTracking();
	preProcessing();
}

void PreCheckingState::exit( void )
{
}

void PreCheckingState::pause( void )
{
}

void PreCheckingState::resume( void )
{
}

void PreCheckingState::update( unsigned long lTimeElapsed )
{
    // Update wat je moet updaten
}

void PreCheckingState::reset()
{
	mStartCheckingFlag = true;
	mStartTimeStamp = 0;
	mEndTimeStamp = 0;
	mStartTimeStampRe = 0;
	mEndTimeStampRe = 0;
	mLoopnum = 0;
	//mStartCnt = 0;
	mFlags = 0;
	mCountnum = 0;

	mPointavgdis = 0;
	mLargeMove = 0;
	
	mSwap_temp = 0;
	mSwap_points = 0;
	mMoveAvgV = 0;
}

void PreCheckingState::AutomaticInitializeTracking()
{
     /* automatic initialization */
    IplImage* eig = cvCreateImage( cvGetSize(mSingletonInstancePtr->grey), 32, 1 );
    IplImage* temp = cvCreateImage( cvGetSize(mSingletonInstancePtr->grey), 32, 1 );

    double quality = 0.01;
    double min_distance = 10;

    mCountnum = MAX_COUNT;
	
    cvGoodFeaturesToTrack( mSingletonInstancePtr->grey, eig, temp, mPoints[0], &mCountnum,
                           quality, min_distance, 0, 3, 0, 0.04 );
    cvFindCornerSubPix( mSingletonInstancePtr->grey, mPoints[0], mCountnum,
        cvSize(WIN_SIZE,WIN_SIZE), cvSize(-1,-1),
        cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
    
	cvReleaseImage( &eig );
    cvReleaseImage( &temp );

	mLoopnum = 1;
	for( int i=0; i<mCountnum; i++ )
	{
		mPoints[2][i]=mPoints[1][i]=mPoints[0][i];
		mPointdis[i] = 0;
	}
	mPointavgdis = 0;
	//mStartCheckingFlag = false;//���F�קK��'r'�H�ᰨ�Wtrigger checking box�A�ҥH�[�W flag not1st���קK���s�x�I�ɡA
	               //�ޥΤW�@���Ұʫe���@�誺�v���M���r��Ϧӳy��largeMove�M�����I�t�׹L���ӤS���W�i�J�������ʪ��骺�^��

	time(&mStartTimeStamp);
	time(&mStartTimeStampRe);
}

void PreCheckingState::nonMotionDetectionOperationExtracted()
{
	// �L�X�ֳt�I���ԲӪ��A
	if (mLargeMove!=0)
	{
		cout<<"countnum = "<<mCountnum<<endl;
		cout<<"moveAvgV/(double)largeMove = "<<mMoveAvgV/(double)mLargeMove<<endl;
		cout<<"largeMove = "<<mLargeMove<<endl;
	}

	mLargeMove = 0; // temp
	mMoveAvgV = 0; // temp
}

bool PreCheckingState::canStartObjectChecking()
{
	return (mCountnum>25 && (mLargeMove > ((mCountnum<120)?8:12))  && (mMoveAvgV/(double)mLargeMove >= ((mCountnum<120)?0.6*MOVE_AVGVTH:MOVE_AVGVTH)))/*  == true*/;

	//return (countnum>25 && (largeMove > ((countnum<120)?8:12))  && (moveAvgV/(double)largeMove >= ((countnum<120)?0.6*MOVE_AVGVTH:MOVE_AVGVTH)) && notFirst3) || mDoMotionDetection == true;
}

bool PreCheckingState::SetPointavgdis()
{
	//�H�U�O�}�l��{�b���v����s����I���ƥةM��m�]feature points�^
	//mStartCnt++;
	time( &mEndTimeStampRe );
	mLoopnum ++;
	int i, k;
	//�ڭ̥β{�b���v���M�W�@�i�v����X�ӧO����I���۹�����m
	cvCalcOpticalFlowPyrLK( mSingletonInstancePtr->prev_grey, mSingletonInstancePtr->grey, mSingletonInstancePtr->prev_pyramid, mSingletonInstancePtr->pyramid,
		mPoints[1], mPoints[0], mCountnum, cvSize(WIN_SIZE,WIN_SIZE), 3, mStatus, 0,
		cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), mFlags );
	mFlags |= CV_LKFLOW_PYR_A_READY;
	mPointavgdis = 0;
	for( i = k = 0; i < mCountnum; i++ )
	{
		mPointdis[i] = 0;
		//�o�@�q�O��u�[�I��
		//if( add_remove_pt )
		//{
		//	double dx = pt.x - points[0][i].x;
		//	double dy = pt.y - points[0][i].y;

		//	//���p�G��w����feature point�Ӫ�A�N���n�[�F
		//	if( dx*dx + dy*dy <= 25 )
		//	{
		//		add_remove_pt = 0;
		//		continue;
		//	}
		//}
		// status�Oarray�A�p�G�ofeature���Q���]��1�A�w�g������0
		// �p�G��feature point�w�g�����A�h�i�J�U�@��feature point
		if( !mStatus[i] )
			continue;


		//�p�G�o��feature point�٦b�A�h����k�Ӧ�m
		//�]��feature point �u�|�V�ӶV�֡A�ҥH�i�H�o�˩�b�P�@��array
		for( int dim=0; dim<MIN(mLoopnum,TIME-1); dim++ )
		{
			//replace k with i
			mPoints[dim][k] = mPoints[dim][i];
			if( dim )
			{
				//delete bad feature points
				if (abs(mPoints[dim][k].x-mPoints[dim-1][k].x)<tolDis && abs(mPoints[dim][k].y-mPoints[dim-1][k].y)<TOL_DIS)
					cvLine( mSingletonInstancePtr->image, cvPointFrom32f(mPoints[dim][k]), cvPointFrom32f(mPoints[dim-1][k]), CV_RGB(255,0,0), 1, 8, 0 );
				else mPoints[dim-1][k]=mPoints[dim][k]; //neglect newest data, maintain previous data.

				if(dim < 3)
				{
					float dx, dy;
					dx = mPoints[dim][k].x - mPoints[dim-1][k].x;
					dy = mPoints[dim][k].y - mPoints[dim-1][k].y;
					mPointdis[k] += pow( (float)(dx*dx + dy*dy), (float)0.5 );
					mPointavgdis += pow( (float)(dx*dx + dy*dy), (float)0.5 );
				}
			}
		}
		k++;
	}

	//��s�̷s������I�ƥ�
    mCountnum = k;
	
	//�p�G����I�Ӥ֩Ϊ̵��ݤӤ[�F�N���s�x�I�a
	if (mCountnum < MIN_NO_FEATURE_PT || difftime( mEndTimeStampRe, mStartTimeStampRe ) > REINITIALIZATION_TIME /*mStartCnt>200*/ )
	{
		//re_init = true;
		//cout<<"re-initialization!"<<endl;
		//mStartCnt = 0;
		return false;
	}

	mPointavgdis = mPointavgdis/mCountnum;

	return true;
}

void PreCheckingState::preProcessing()
{
	//cvRectangle( mSingletonInstancePtr->image, cvPoint(mSingletonInstancePtr->left, mSingletonInstancePtr->top), cvPoint(mSingletonInstancePtr->right, mSingletonInstancePtr->bottom), CV_RGB(255,255,0), 3, 8, 0);

	//�N�ª���ƥ��prev���@�հO����
	CV_SWAP( mSingletonInstancePtr->prev_grey, mSingletonInstancePtr->grey, mSwap_temp );
	CV_SWAP( mSingletonInstancePtr->prev_pyramid, mSingletonInstancePtr->pyramid, mSwap_temp );

	for( int dim=MIN(mLoopnum,TIME-1); dim>0; dim-- ){
		CV_SWAP( mPoints[dim], mPoints[dim-1], mSwap_points );
	}
}
void PreCheckingState::spillPointsAndShow()
{
	//�n��X�̥��̥k�̤W�̤U
	cvZero(mSingletonInstancePtr->blob);
	mSingletonInstancePtr->top=0/*blob->height*/; 
	mSingletonInstancePtr->bottom=mSingletonInstancePtr->blob->height/*0*/; 
	mSingletonInstancePtr->right=0; 
	mSingletonInstancePtr->left=mSingletonInstancePtr->blob->width;

	for( int i=0; i<mCountnum; i++ ){
		//�x���I
		cvCircle( mSingletonInstancePtr->image, cvPointFrom32f(mPoints[0][i]), 3, CV_RGB(0,255,0), -1, 8,0);
		
		//���֪��I
		if( mPointdis[i] > THFACTOR*mPointavgdis ) {
			mLargeMove++; 
			mMoveAvgV += mPointdis[i];
			mSingletonInstancePtr->top = (mSingletonInstancePtr->top>=mPoints[0][i].y/*top<=points[0][i].y*/) ? mSingletonInstancePtr->top : static_cast<int>(mPoints[0][i].y);
			mSingletonInstancePtr->bottom = (mSingletonInstancePtr->bottom<=mPoints[0][i].y/*bottom>=points[0][i].y*/) ? mSingletonInstancePtr->bottom : static_cast<int>(mPoints[0][i].y);
			mSingletonInstancePtr->right = (mSingletonInstancePtr->right>=mPoints[0][i].x) ? mSingletonInstancePtr->right : static_cast<int>(mPoints[0][i].x);
			mSingletonInstancePtr->left = (mSingletonInstancePtr->left<=mPoints[0][i].x) ? mSingletonInstancePtr->left : static_cast<int>(mPoints[0][i].x);
			//�e�bblob�W��
			cvCircle( mSingletonInstancePtr->blob, cvPointFrom32f(mPoints[0][i]), MASKR, CV_RGB(255,255,255), -1, 8,0);
		}
		//������I�s��
		char number[4];
		sprintf(number,"%d", i);
		cvPutText( mSingletonInstancePtr->image, number, cvPointFrom32f(mPoints[0][i]), &(mSingletonInstancePtr->testfont), CV_RGB(0, 0, 255) );
	}

	//�e�X���خ�
	//cvRectangle( mSingletonInstancePtr->image, cvPoint(mSingletonInstancePtr->left, mSingletonInstancePtr->top), cvPoint(mSingletonInstancePtr->right, mSingletonInstancePtr->bottom), CV_RGB(255,255,0), 3, 8, 0);

	//�N�ª���ƥ��prev���@�հO����
	CV_SWAP( mSingletonInstancePtr->prev_grey, mSingletonInstancePtr->grey, mSwap_temp );
	CV_SWAP( mSingletonInstancePtr->prev_pyramid, mSingletonInstancePtr->pyramid, mSwap_temp );

	for( int dim=MIN(mLoopnum,TIME-1); dim>0; dim-- ){
		CV_SWAP( mPoints[dim], mPoints[dim-1], mSwap_points );
	}
}

bool PreCheckingState::waifForTempStatePass()
{
	if( mStartCheckingFlag == false )
		return true;

	time(&mEndTimeStamp);

	// reject�e�X��loop
	double diffTime = difftime( mEndTimeStamp, mStartTimeStamp );
	if( diffTime >= TEMP_STATE_PASS_TIME )
	{
		time(&mStartTimeStamp);
		mStartCheckingFlag = false;//�o�˴N�i�H�קK�@��'���x�I'�N���W�i�ӤW�����P�_���A���O�X���H��NOK�i�H�i��

		return true;
	}
	return false;
}

bool PreCheckingState::execute( IplImage* imageSource )
{
	//��������
	changeState( CheckingState::getSingletonPtr() );
	return false;
	//
}