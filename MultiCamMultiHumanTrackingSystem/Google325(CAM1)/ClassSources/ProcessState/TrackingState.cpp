#include "TrackingState.h"
#include "PreCheckingState.h"
#include "ReTrackingState.h"
//#include "C_PM.h"

TrackingState TrackingState::mTrackingState;

void TrackingState::enter( OpticalFlow* SingletonInstancePtr )
{
	mSingletonInstancePtr = SingletonInstancePtr;

	send2ServerCnt = 0;

	//initialize the observation integration table
	for (int q=0; q<MAX_TARGET_NUM; q++)
	{
		occupiedList_cam2[q] = occupiedList_cam1[q] = false;

		for (int r=0; r<MAX_TARGET_NUM; r++)
		{
			colorScoreMap[q][r] = NONDEFINED;
			objConfMap[q][r] = 0;
			matchCntMap[q][r] = NONDEFINED;
		}
	}
	
	// reset all, otherwise the sample order will be corrupted
//	mSingletonInstancePtr->Cam->mPM->resetTranslationRelatedParameters();
}

void TrackingState::exit( void )
{
}

void TrackingState::pause( void )
{
}

void TrackingState::resume( void )
{
}

void TrackingState::update( unsigned long lTimeElapsed )
{
}

void TrackingState::sendControlSignal( TRACKING_RESULT result )
{
	time (&mEndTimeStamp);

	switch( result )
	{
	case RE_INITIALIZATION:
		//mSingletonInstancePtr->Cam->sendControlSignal( difftime (mEndTimeStamp,mStartTimeStamp), REGRESSION );
		//break;
	case LOSING_OBJECT:
		mSingletonInstancePtr->Cam->sendControlSignal( 0.2, REGRESSION );
		break;
	case FINE:
		mSingletonInstancePtr->Cam->sendControlSignal( difftime (mEndTimeStamp,mStartTimeStamp), DONT_CARE );
		break;
	}

	time (&mStartTimeStamp);
}

TRACKING_RESULT TrackingState::objectTrackingOperation()
{
		CamIterationReturn returnValue;
		returnValue = mSingletonInstancePtr->Cam->iteration( mSingletonInstancePtr->result, mSingletonInstancePtr->fgmask, mSingletonInstancePtr->prevReturnROI );
		mSingletonInstancePtr->prevReturnROI = returnValue.targetROI;

		if( 0/*returnValue.val == -1*/ ) // jump to preCheckingState
		{			
			if (mSingletonInstancePtr->fgmask) 
				cvReleaseImage( &(mSingletonInstancePtr->fgmask));
			mSingletonInstancePtr->fgmask = 0;
			return RE_INITIALIZATION;
		}
		else if( 0/*returnValue.val == -2*/ ) // jump to reTrackingObjectState
		{
			return LOSING_OBJECT;
		}
		else if (0/*returnValue.val == -3*/) // jump to reTrackingObjectState
		{
			if (mSingletonInstancePtr->fgmask) 
				cvReleaseImage( &(mSingletonInstancePtr->fgmask));
			mSingletonInstancePtr->fgmask = 0;
			return LOSING_OBJECT;

		}

		if (mSingletonInstancePtr->fgmask)
			cvReleaseImage(&(mSingletonInstancePtr->fgmask));

		mSingletonInstancePtr->fgmask=0;

		return FINE;  // continue this state
}

bool TrackingState::checkReInitialization()
{
	time( &mEndTimeStampLosing );
	
	if( mLosingCount >= 3 )
	{
		if( difftime( mEndTimeStampLosing, mStartTimeStampLosing ) <= 8.0 )
		{
			mLosingCount = 0;
			return true;
		}
		else
			mLosingCount = 0;
	}
	else if( mLosingCount == 1 )
		time( &mStartTimeStampLosing );

	return false;
}

bool TrackingState::execute( IplImage* imageSource)
{
	TRACKING_RESULT trackingResult = objectTrackingOperation();

	switch( trackingResult )
	{
	case RE_INITIALIZATION: // jump to PreCheckingState
		sendControlSignal( RE_INITIALIZATION );

		mSingletonInstancePtr->mNormalMessage += string("re-initialization!\n");

		changeState( PreCheckingState::getSingletonPtr() );
		return false;
		break;

	case LOSING_OBJECT: // jump to ReTrackingState or CheckingState
		mLosingCount++;

		mSingletonInstancePtr->mNormalMessage += string("Losing\n");

		if( checkReInitialization() == true )
		{
			changeState( PreCheckingState::getSingletonPtr() ); // 如果連續3次losing && 這3次losing在8秒內發生完畢就重新開始
		}
		else
			changeState( ReTrackingState::getSingletonPtr() );

		return false; 
		break;

	case FINE: // continue tracking
		sendControlSignal( FINE );

		break;
	}
	if (mSingletonInstancePtr->Cam->cam_id == CAM1)
		cvPutText( mSingletonInstancePtr->result, "CAM1 TRACKING", cvPoint(5/*mSingletonInstancePtr->result->width-130*/, 18), &(mSingletonInstancePtr->statefont), CV_RGB(255,0,0) );
	else if (mSingletonInstancePtr->Cam->cam_id == CAM2)
		cvPutText( mSingletonInstancePtr->result, "CAM2 TRACKING", cvPoint(5/*mSingletonInstancePtr->result->width-130*/, 18), &(mSingletonInstancePtr->statefont), CV_RGB(255,0,0) );
	else
		cvPutText( mSingletonInstancePtr->result, "CAM3 TRACKING", cvPoint(5/*mSingletonInstancePtr->result->width-130*/, 18), &(mSingletonInstancePtr->statefont), CV_RGB(255,0,0) );
	//Solve Correspondence Here
	//SolveObservationIntegration_greedy();
	if (!(send2ServerCnt%25))
	{
		SendObservationInfo(mSingletonInstancePtr->Cam->cam_id);
	}
	send2ServerCnt++;

	DrawObservationStatus(mSingletonInstancePtr->Cam->cam_id);

	return true;
}
bool TrackingState::correspondenceLikelihoodThresholding(int g, int f)
{
	total = ((abs(intersect_Z - 155) > 20) ? 0 : 5);
	total += ((MAGNITUDE((headpt1 - headpt2)) > 20) ? 0 : 3);
	total += ((color_score > 0.3) ? 0 : 4);
	total += ((matchCnt < 2) ? 0 : 2);
	total += ((best_color_index <= 1) ? 2 : 0);
	total += ((best_color_dist > 0.2) ? 0 : 3);
	total += ((best_color_dist < 0.2) || (color_score < 0.3) || (MAGNITUDE((headpt1 - headpt2)) < 20)) ? 5 : 0;

	if (total >= 18) 
	{
		objConfMap[g][f] = Lv = 4;
		occupiedList_cam1[g] = occupiedList_cam2[f] = true;
	}
	
	return (total >= 14);
}

void TrackingState::SendObservationInfo(int CAMID)
{
	IplImage* Cr = mSingletonInstancePtr->Cam->image_Cr;
	IplImage* Cb = mSingletonInstancePtr->Cam->image_Cb;
	IplImage* Y = mSingletonInstancePtr->Cam->image_Y;
	cvResize(Cr, Cr_down4x4, CV_INTER_AREA);
	cvResize(Cb, Cb_down4x4, CV_INTER_AREA);
	cvResize(Y, Y_down4x4, CV_INTER_AREA);
	cvNamedWindow("Y");
	cvShowImage("Y", Y);
	cvNamedWindow("Cr");
	cvShowImage("Cr", Cr);
	cvNamedWindow("Cb");
	cvShowImage("Cb", Cb);

	//int cam_ID = mSingletonInstancePtr->Cam[0]->cam_id;
	CAMShift* Camptr = mSingletonInstancePtr->Cam;
	CAMShift::Target* obsptr = Camptr->targetList;
	CameraLine3D* lineptr = Camptr->targetLines;
	BodyInfo* body = Camptr->targetInfo;

	if (CAMID == CAM1)
	{
		AliveStaMgr_CAM1 aliveSta;
		time_t timeNow;
		time(&timeNow);
		aliveSta.timeIndex = (int)timeNow;
		for (int i=0; i<MAX_TARGET_NUM; i++)
		{
			aliveSta.aliveArr[i] = obsptr[i].isAlive;
		}
		setAliveSta(aliveSta);

		if (obsptr[0].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[0], &camlinemgr11, aliveSta.timeIndex);
			setCamLine(camlinemgr11);
		}
		if (obsptr[1].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[1], &camlinemgr12, aliveSta.timeIndex);
			setCamLine(camlinemgr12);
		}
		if (obsptr[2].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[2], &camlinemgr13, aliveSta.timeIndex);
			setCamLine(camlinemgr13);
		}
		if (obsptr[3].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[3], &camlinemgr14, aliveSta.timeIndex);
			setCamLine(camlinemgr14);
		}
		if (obsptr[4].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[4], &camlinemgr15, aliveSta.timeIndex);
			setCamLine(camlinemgr15);
		}
	}
	else if (CAMID == CAM2)
	{
		AliveStaMgr_CAM2 aliveSta;
		time_t timeNow;
		time(&timeNow);
		aliveSta.timeIndex = (int)timeNow;
		for (int i=0; i<MAX_TARGET_NUM; i++)
		{
			aliveSta.aliveArr[i] = obsptr[i].isAlive;
		}
		setAliveSta(aliveSta);

		if (obsptr[0].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[0], &camlinemgr21, aliveSta.timeIndex);
			setCamLine(camlinemgr21);
		}
		if (obsptr[1].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[1], &camlinemgr22, aliveSta.timeIndex);
			setCamLine(camlinemgr22);
		}
		if (obsptr[2].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[2], &camlinemgr23, aliveSta.timeIndex);
			setCamLine(camlinemgr23);
		}
		if (obsptr[3].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[3], &camlinemgr24, aliveSta.timeIndex);
			setCamLine(camlinemgr24);
		}
		if (obsptr[4].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[4], &camlinemgr25, aliveSta.timeIndex);
			setCamLine(camlinemgr25);
		}
	}
	else if (CAMID == CAM3)
	{
		AliveStaMgr_CAM3 aliveSta;
		time_t timeNow;
		time(&timeNow);
		aliveSta.timeIndex = (int)timeNow;
		for (int i=0; i<MAX_TARGET_NUM; i++)
		{
			aliveSta.aliveArr[i] = obsptr[i].isAlive;
		}
		setAliveSta(aliveSta);

		if (obsptr[0].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[0], &camlinemgr31, aliveSta.timeIndex);
			setCamLine(camlinemgr31);
		}
		if (obsptr[1].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[1], &camlinemgr32, aliveSta.timeIndex);
			setCamLine(camlinemgr32);
		}
		if (obsptr[2].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[2], &camlinemgr33, aliveSta.timeIndex);
			setCamLine(camlinemgr33);
		}
		if (obsptr[3].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[3], &camlinemgr34, aliveSta.timeIndex);
			setCamLine(camlinemgr34);
		}
		if (obsptr[4].isAlive)
		{
			trans2CamLineMgr(Y_down4x4, Cr_down4x4, Cb_down4x4, &lineptr[4], &camlinemgr35, aliveSta.timeIndex);
			setCamLine(camlinemgr35);
		}
	}
}

void TrackingState::DrawObservationStatus(int CAMID)
{
	getLabelIntegrate(IntegrateStatus);
	
	CAMShift* Camptr = mSingletonInstancePtr->Cam;
	CAMShift::Target* obsptr = Camptr->targetList;

	for (int g=0; g<MAX_TARGET_NUM; g++)
	{
		if (!obsptr[g].isAlive) continue;

		if (CAMID == CAM1)
		{
			//if ((IntegrateStatus.CAM1_id[g] >= 0) && ((IntegrateStatus.CAM1_id[g] - (int)IntegrateStatus.CAM1_id[g]) == 0.1234))
			if ((IntegrateStatus.CAM1_id[g] > 0))
			{
				int conf = (int)IntegrateStatus.Alive[(int)IntegrateStatus.CAM1_id[g]]/1000;
				Camptr->targetHeight[g] = IntegrateStatus.Alive[(int)IntegrateStatus.CAM1_id[g]] - conf*1000;
				Camptr->DrawIntegratedObservation(g, mSingletonInstancePtr->result, IntegrateStatus.CAM1_id[g], conf/*IntegrateStatus.Alive[(int)IntegrateStatus.CAM1_id[g]]*/);
			}
			else
			{
				Camptr->DrawSingleObservation(g, mSingletonInstancePtr->result);
			}
		}
		else if (CAMID == CAM2)
		{
			//if ((IntegrateStatus.CAM2_id[g] >= 0) && ((IntegrateStatus.CAM2_id[g] - (int)IntegrateStatus.CAM2_id[g]) == 0.1234))
			if ((IntegrateStatus.CAM2_id[g] > 0))
			{
				int conf = (int)IntegrateStatus.Alive[(int)IntegrateStatus.CAM2_id[g]]/1000;
				Camptr->targetHeight[g] = IntegrateStatus.Alive[(int)IntegrateStatus.CAM2_id[g]] - conf*1000;
				Camptr->DrawIntegratedObservation(g, mSingletonInstancePtr->result, IntegrateStatus.CAM2_id[g], conf/*IntegrateStatus.Alive[(int)IntegrateStatus.CAM2_id[g]]*/);
			}
			else
			{
				Camptr->DrawSingleObservation(g, mSingletonInstancePtr->result);
			}
		}
		else
			;//do nothing
	}

}

//////void TrackingState::SolveObservationIntegration_greedy()
//////{
//////	CAMShift* Cam1ptr = mSingletonInstancePtr->Cam[CAM1];
//////	CAMShift* Cam2ptr = mSingletonInstancePtr->Cam[CAM2];
//////	CAMShift::Target* obs1ptr = Cam1ptr->targetList;
//////	CAMShift::Target* obs2ptr = Cam2ptr->targetList;
//////	CameraLine3D* line1ptr = Cam1ptr->targetLines;
//////	CameraLine3D* line2ptr = Cam2ptr->targetLines;
//////	BodyInfo* body1 = Cam1ptr->targetInfo;
//////	BodyInfo* body2 = Cam2ptr->targetInfo;
//////	Position3D intersect, line1pt, line2pt;
//////	intersect_Z = 0;
//////	char number[20];
//////
//////
//////	bool IsMatched = false;
//////	for (int g=0; g<MAX_TARGET_NUM; g++)
//////	{
//////		if (!obs1ptr[g].isAlive) 
//////		{
//////			occupiedList_cam1[g] = false; //set available
//////			continue;
//////		}
//////
//////		for (int f=0; f<MAX_TARGET_NUM; f++)
//////		{
//////			if (!obs2ptr[f].isAlive) 
//////			{
//////				occupiedList_cam2[f] = false; //set available
//////				continue;
//////			}
//////
//////			if (occupiedList_cam1[g] || occupiedList_cam2[f]) continue;
//////
//////			if (CheckLineIntersect(&line1ptr[g], &line2ptr[f], intersect, line1pt, line2pt))
//////			{
//////				if (intersect.z < 155.0) continue;
//////
//////				intersect_Z = intersect.z;
//////				bool show3DPos = line1ptr[g].getPosFromZ(intersect_Z, headpt1);
//////				show3DPos = show3DPos && line2ptr[f].getPosFromZ(intersect_Z, headpt2);
//////
//////				if (!show3DPos || MAGNITUDE((headpt1 - headpt2)) > 40) continue;
//////				//now it's geometrically acceptable, record h as height
//////
//////				body1[g].LocateAndMeasureInfo(&line1ptr[g], Cam1ptr->targetGndLines[g], true, intersect_Z/*+10-10*/, Cam1ptr->image_Cr, Cam1ptr->image_Cb, Cam1ptr->hist_mask);
//////				body2[f].LocateAndMeasureInfo(&line2ptr[f], Cam2ptr->targetGndLines[f], true, intersect_Z/*+10+5*/, Cam2ptr->image_Cr, Cam2ptr->image_Cb, Cam2ptr->hist_mask);
//////
//////				matchCnt = NONDEFINED;
//////				color_score = NONDEFINED;
//////				Lv = 0;
//////				color_score = AppearanceComparison(&body1[g], &body2[f], matchCnt, Lv, best_color_dist, best_color_index);
//////
//////				std::stringstream score_ss;
//////				score_ss << int(100.0*color_score);
//////				std::string outSig = "color comparison: " + score_ss.str() + "\n ";
//////
//////				if (correspondenceLikelihoodThresholding(g,f))
//////				{
//////					matchCntMap[g][f] = matchCnt;
//////					colorScoreMap[g][f] = color_score;
//////					objConfMap[g][f] = Lv;
//////					
//////					//simplest method: accept that they are the same person now.
//////					Cam1ptr->targetHeight[g] = intersect_Z/* + 10 - 10*/;
//////					Cam1ptr->targetPos[g] = headpt1;
//////					Cam2ptr->targetHeight[f] = intersect_Z/* + 10 + 5*/;
//////					Cam2ptr->targetPos[f] = headpt2;
//////
//////					//now we got headline, gndline, targetheight, targetPos(3D), and have measured and comparison histograms by blocks
//////					Cam1ptr->DrawIntegratedObservation(g, mSingletonInstancePtr->result[CAM1]);
//////					Cam2ptr->DrawIntegratedObservation(f, mSingletonInstancePtr->result[CAM2]);
//////					
//////					
//////					outSig += "matched!\n";
//////					stringstream h_s;
//////					h_s << ((intersect_Z < 0) ? 0 : (intersect_Z > 200) ? 200 : (int)intersect_Z);
//////					(mSingletonInstancePtr->mControlMessage) += (outSig + "height = " + h_s.str() + ", ");
//////
//////					std::stringstream tscore;
//////					tscore << ((int)total);
//////					(mSingletonInstancePtr->mControlMessage) += ("total = " + tscore.str() + " .\n");
//////
//////					IsMatched = true;
//////
//////
//////				}
//////				else continue;
//////
//////
//////			}
//////
//////		}
//////	}
//////	if (IsMatched == false) 
//////	{
//////		//std::string outSig = "not matched\n";
//////		//(mSingletonInstancePtr->mControlMessage) += outSig;
//////	}
//////	for (int c=0; c<MAX_TARGET_NUM; c++)
//////	{
//////		for (int b=0; b<MAX_TARGET_NUM; b++)
//////		{
//////			if (objConfMap[c][b] != 4) continue;
//////
//////			if (occupiedList_cam1[c] == false || occupiedList_cam2[b] == false)
//////			{
//////				objConfMap[c][b] = 0;
//////				occupiedList_cam1[c] = occupiedList_cam2[b] = false;
//////				continue;
//////			}
//////			
//////			Cam1ptr->DrawIntegratedObservation(c, mSingletonInstancePtr->result[CAM1]);
//////			Cam2ptr->DrawIntegratedObservation(b, mSingletonInstancePtr->result[CAM2]);
//////		}
//////	}
//////}
//////void TrackingState::SolveObservationIntegration()
//////{
//////	CAMShift* Cam1ptr = mSingletonInstancePtr->Cam[CAM1];
//////	CAMShift* Cam2ptr = mSingletonInstancePtr->Cam[CAM2];
//////	CAMShift::Target* obs1ptr = Cam1ptr->targetList;
//////	CAMShift::Target* obs2ptr = Cam2ptr->targetList;
//////	CameraLine3D* line1ptr = Cam1ptr->targetLines;
//////	CameraLine3D* line2ptr = Cam2ptr->targetLines;
//////	BodyInfo* body1 = Cam1ptr->targetInfo;
//////	BodyInfo* body2 = Cam2ptr->targetInfo;
//////	Position3D intersect, line1pt, line2pt;
//////	intersect_Z = 0;
//////	char number[20];
//////
//////
//////	//initialize the observation integration table
//////	for (int q=0; q<MAX_TARGET_NUM; q++)
//////		for (int r=0; r<MAX_TARGET_NUM; r++)
//////		{
//////			colorScoreMap[q][r] = NONDEFINED;
//////			objConfMap[q][r] = 0;
//////			matchCntMap[q][r] = NONDEFINED;
//////		}
//////
//////
//////	bool IsMatched = false;
//////	for (int g=0; g<MAX_TARGET_NUM; g++)
//////	{
//////		if (!obs1ptr[g].isAlive) continue;
//////		for (int f=0; f<MAX_TARGET_NUM; f++)
//////		{
//////			if (!obs2ptr[f].isAlive) continue;
//////
//////			if (CheckLineIntersect(&line1ptr[g], &line2ptr[f], intersect, line1pt, line2pt))
//////			{
//////				if (intersect.z < 140.0) continue;
//////
//////				intersect_Z = intersect.z;
//////				bool show3DPos = line1ptr[g].getPosFromZ(intersect_Z, headpt1);
//////				show3DPos = show3DPos && line2ptr[f].getPosFromZ(intersect_Z, headpt2);
//////
//////				if (!show3DPos || MAGNITUDE((headpt1 - headpt2)) > LARGE_IDENTICAL_THD) continue;
//////				//now it's geometrically acceptable, record h as height
//////
//////				body1[g].LocateAndMeasureInfo(&line1ptr[g], Cam1ptr->targetGndLines[g], true, intersect_Z+10-5, Cam1ptr->image_Cr, Cam1ptr->image_Cb, Cam1ptr->hist_mask);
//////				body2[f].LocateAndMeasureInfo(&line2ptr[f], Cam2ptr->targetGndLines[f], true, intersect_Z+10+5, Cam2ptr->image_Cr, Cam2ptr->image_Cb, Cam2ptr->hist_mask);
//////
//////				matchCnt = NONDEFINED;
//////				color_score = NONDEFINED;
//////				Lv = 0;
//////				color_score = AppearanceComparison(&body1[g], &body2[f], matchCnt, Lv, best_color_dist, best_color_index);
//////
//////				std::stringstream score_ss;
//////				score_ss << int(100.0*color_score);
//////				std::string outSig = "color comparison: " + score_ss.str() + "\n ";
//////
//////				if (color_score < 0.3)
//////				{
//////					matchCntMap[g][f] = matchCnt;
//////					colorScoreMap[g][f] = color_score;
//////					objConfMap[g][f] = Lv;
//////					
//////					//simplest method: accept that they are the same person now.
//////					Cam1ptr->targetHeight[g] = intersect_Z + 10 - 5;
//////					Cam1ptr->targetPos[g] = headpt1;
//////					Cam2ptr->targetHeight[f] = intersect_Z + 10 + 5;
//////					Cam2ptr->targetPos[f] = headpt2;
//////
//////					//now we got headline, gndline, targetheight, targetPos(3D), and have measured and comparison histograms by blocks
//////					Cam1ptr->DrawIntegratedObservation(g, mSingletonInstancePtr->result[CAM1]);
//////					Cam2ptr->DrawIntegratedObservation(f, mSingletonInstancePtr->result[CAM2]);
//////					
//////					
//////					outSig += "matched!\n";
//////					stringstream h_s;
//////					h_s << ((intersect_Z < 0) ? 0 : (intersect_Z > 300) ? 300 : (int)intersect_Z);
//////					(mSingletonInstancePtr->mControlMessage) += (outSig + "height = " + h_s.str() + ", ");
//////
//////					//std::stringstream tscore;
//////					//tscore << ((int)total);
//////					//(mSingletonInstancePtr->mControlMessage) += ("total = " + tscore.str() + " .\n");
//////
//////					IsMatched = true;
//////
//////
//////				}
//////				else continue;
//////
//////
//////			}
//////
//////		}
//////	}
//////	if (IsMatched == false) 
//////	{
//////		//std::string outSig = "not matched\n";
//////		//(mSingletonInstancePtr->mControlMessage) += outSig;
//////	}
//////
//////}
