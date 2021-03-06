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
	//mStartCheckingFlag = false;//為了避免按'r'以後馬上trigger checking box，所以加上 flag not1st來避免重新灑點時，
	               //引用上一次啟動前那一刻的影像和金字塔圖而造成largeMove和移動點速度過高而又馬上進入偵測互動物體的回圈

	time(&mStartTimeStamp);
	time(&mStartTimeStampRe);
}

void PreCheckingState::nonMotionDetectionOperationExtracted()
{
	// 印出快速點的詳細狀態
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
	//以下是開始對現在的影像更新綠色點的數目和位置（feature points）
	//mStartCnt++;
	time( &mEndTimeStampRe );
	mLoopnum ++;
	int i, k;
	//我們用現在的影像和上一張影像找出個別綠色點的相對應位置
	cvCalcOpticalFlowPyrLK( mSingletonInstancePtr->prev_grey, mSingletonInstancePtr->grey, mSingletonInstancePtr->prev_pyramid, mSingletonInstancePtr->pyramid,
		mPoints[1], mPoints[0], mCountnum, cvSize(WIN_SIZE,WIN_SIZE), 3, mStatus, 0,
		cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), mFlags );
	mFlags |= CV_LKFLOW_PYR_A_READY;
	mPointavgdis = 0;
	for( i = k = 0; i < mCountnum; i++ )
	{
		mPointdis[i] = 0;
		//這一段是手工加點的
		//if( add_remove_pt )
		//{
		//	double dx = pt.x - points[0][i].x;
		//	double dy = pt.y - points[0][i].y;

		//	//說如果跟已有的feature point太近，就不要加了
		//	if( dx*dx + dy*dy <= 25 )
		//	{
		//		add_remove_pt = 0;
		//		continue;
		//	}
		//}
		// status是array，如果這feature有被找到設為1，已經消失為0
		// 如果此feature point已經消失，則進入下一個feature point
		if( !mStatus[i] )
			continue;


		//如果這個feature point還在，則放到第k個位置
		//因為feature point 只會越來越少，所以可以這樣放在同一個array
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

	//更新最新的綠色點數目
    mCountnum = k;
	
	//如果綠色點太少或者等待太久了就重新灑點吧
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

	//將舊的資料丟到prev那一組記憶體
	CV_SWAP( mSingletonInstancePtr->prev_grey, mSingletonInstancePtr->grey, mSwap_temp );
	CV_SWAP( mSingletonInstancePtr->prev_pyramid, mSingletonInstancePtr->pyramid, mSwap_temp );

	for( int dim=MIN(mLoopnum,TIME-1); dim>0; dim-- ){
		CV_SWAP( mPoints[dim], mPoints[dim-1], mSwap_points );
	}
}
void PreCheckingState::spillPointsAndShow()
{
	//要找出最左最右最上最下
	cvZero(mSingletonInstancePtr->blob);
	mSingletonInstancePtr->top=0/*blob->height*/; 
	mSingletonInstancePtr->bottom=mSingletonInstancePtr->blob->height/*0*/; 
	mSingletonInstancePtr->right=0; 
	mSingletonInstancePtr->left=mSingletonInstancePtr->blob->width;

	for( int i=0; i<mCountnum; i++ ){
		//灑綠點
		cvCircle( mSingletonInstancePtr->image, cvPointFrom32f(mPoints[0][i]), 3, CV_RGB(0,255,0), -1, 8,0);
		
		//對於快的點
		if( mPointdis[i] > THFACTOR*mPointavgdis ) {
			mLargeMove++; 
			mMoveAvgV += mPointdis[i];
			mSingletonInstancePtr->top = (mSingletonInstancePtr->top>=mPoints[0][i].y/*top<=points[0][i].y*/) ? mSingletonInstancePtr->top : static_cast<int>(mPoints[0][i].y);
			mSingletonInstancePtr->bottom = (mSingletonInstancePtr->bottom<=mPoints[0][i].y/*bottom>=points[0][i].y*/) ? mSingletonInstancePtr->bottom : static_cast<int>(mPoints[0][i].y);
			mSingletonInstancePtr->right = (mSingletonInstancePtr->right>=mPoints[0][i].x) ? mSingletonInstancePtr->right : static_cast<int>(mPoints[0][i].x);
			mSingletonInstancePtr->left = (mSingletonInstancePtr->left<=mPoints[0][i].x) ? mSingletonInstancePtr->left : static_cast<int>(mPoints[0][i].x);
			//畫在blob上面
			cvCircle( mSingletonInstancePtr->blob, cvPointFrom32f(mPoints[0][i]), MASKR, CV_RGB(255,255,255), -1, 8,0);
		}
		//幫綠色點編號
		char number[4];
		sprintf(number,"%d", i);
		cvPutText( mSingletonInstancePtr->image, number, cvPointFrom32f(mPoints[0][i]), &(mSingletonInstancePtr->testfont), CV_RGB(0, 0, 255) );
	}

	//畫出黃框框
	//cvRectangle( mSingletonInstancePtr->image, cvPoint(mSingletonInstancePtr->left, mSingletonInstancePtr->top), cvPoint(mSingletonInstancePtr->right, mSingletonInstancePtr->bottom), CV_RGB(255,255,0), 3, 8, 0);

	//將舊的資料丟到prev那一組記憶體
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

	// reject前幾次loop
	double diffTime = difftime( mEndTimeStamp, mStartTimeStamp );
	if( diffTime >= TEMP_STATE_PASS_TIME )
	{
		time(&mStartTimeStamp);
		mStartCheckingFlag = false;//這樣就可以避免一按'重灑點'就馬上進來上面的判斷式，但是幾秒以後就OK可以進來

		return true;
	}
	return false;
}

bool PreCheckingState::execute( IplImage* imageSource )
{
	//直接跳級
	changeState( CheckingState::getSingletonPtr() );
	return false;
	//
}
