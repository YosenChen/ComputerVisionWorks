// Form_ImageSource.cpp : main project file.

//#include "stdafx.h"
#include "Form1.h"


using namespace Form_ImageSource;

int main(array<System::String ^> ^args);
//void globaladdThread(void);
//void setupServer();
unsigned recordVideoThread( void *pMyID );

//int onlineCamNo = NONDEFINED;




[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	//showWin32Console();

	//fileOut.open("error report.txt",ios::out);
	// Enabling Windows XP visual effects before any controls are created

//AllocConsole();
//try { 
	//setupServer();
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew Form1());	
// } catch( ... )
//{}
//
//FreeConsole();


   // FreeConsole();
	return 0;
}


void Form1::globaladdThread( void ) 
{
    if (onlineCamNo != NONDEFINED)
	{
		globalThreadNr = onlineCamNo;
		globalhThread[0] = (HANDLE)_beginthreadex(NULL, 0, recordVideoThread, &globalThreadNr, 0, NULL );
	}
	//globalThreadNr++;
	//globalhThread2 = (HANDLE)_beginthreadex(NULL, 0, globalparallelProcess, &globalThreadNr, 0, NULL );
	//globalThreadNr++;
	//globalhThread3 = (HANDLE)_beginthreadex(NULL, 0, globalparallelProcess, &globalThreadNr, 0, NULL );
	//globalThreadNr++;
	//globalhThread4 = (HANDLE)_beginthreadex(NULL, 0, globalparallelProcess, &globalThreadNr, 0, NULL );
}

unsigned recordVideoThread( void *pMyID )
{
	char    *MyID = (char*)pMyID;
	int id=(int)*MyID;
	switch (id)
	{
	case CAM1: 
		srcVideo_out = cvCreateVideoWriter("srcVideo[CAM1].avi",-1,25,cvSize(320,240),1);
		resVideo_out = cvCreateVideoWriter("resVideo[CAM1].avi",-1,25,cvSize(320,240),1);
		while((cvWaitKey(1000/REC_SAMPLINGRATE - 1) != 27) && keepRecFlag)
		{
			if (imageSampleReady)
			{
				cvWriteFrame(srcVideo_out, recSrc);
				cvWriteFrame(resVideo_out, recRes);
			}
			else
			{
				cvWriteFrame(srcVideo_out, recSrcPrev);
				cvWriteFrame(resVideo_out, recResPrev);
			}
		}

		break;

	case CAM2: 
		srcVideo_out = cvCreateVideoWriter("srcVideo[CAM2].avi",-1,25,cvSize(320,240),1);
		resVideo_out = cvCreateVideoWriter("resVideo[CAM2].avi",-1,25,cvSize(320,240),1);
		while((cvWaitKey(1000/REC_SAMPLINGRATE - 1) != 27) && keepRecFlag)
		{
			if (imageSampleReady)
			{
				cvWriteFrame(srcVideo_out, recSrc);
				cvWriteFrame(resVideo_out, recRes);
			}
			else
			{
				cvWriteFrame(srcVideo_out, recSrcPrev);
				cvWriteFrame(resVideo_out, recResPrev);
			}
		}

		break;

	case CAM3:
		//////////while(1) cvWaitKey(100);
		break;


	default:
		;
	}
	return 0;
}


void Form1::mInitializeMyObjects()
{
	cvZero(recRes);
	cvZero(recSrc);
	cvZero(recResPrev);
	cvZero(recSrcPrev);

	//mWidthCapturedImage = 320;
	//mHeightCapturedImage = 240;
	this->pictureBox3->Size = System::Drawing::Size(mWidthCapturedImage, mHeightCapturedImage);
	this->pictureBox4->Size = System::Drawing::Size(mWidthCapturedImage, mHeightCapturedImage);

	mImageSourceWebCam = new C_ImageSourceWebCam;
	mImageSourceWebCam->initialize( cvPoint(320/*720*//*mWidthCapturedImage*/,240/*480*//*mHeightCapturedImage*/) ); //->mInitializeGray(1, cvPoint(320,240), miResizeSize[0], miLT[0], miRB[0]);

	//motionDetection = OpticalFlow::getSingletonPtr( mWidthCapturedImage, mHeightCapturedImage );
	mpCounterForFPS=new CPerformanceCounter;
	mpCounterForProcessingTime=new CPerformanceCounter;

	matColorDisplayImage=new ClassOpenCVMat;
	matColorDisplayImage->mCreateRGBUCMat(mWidthCapturedImage,mHeightCapturedImage);

	matColorDisplayResult=new ClassOpenCVMat;
	matColorDisplayResult->mCreateRGBUCMat(mWidthCapturedImage,mHeightCapturedImage);

	//assign the buffer head pointer to the bitmap
	IntPtr pToDisplayImage(matColorDisplayImage->mpUCArray);
	mBitMapImage=gcnew System::Drawing::Bitmap(mWidthCapturedImage,mHeightCapturedImage,mWidthCapturedImage*3,System::Drawing::Imaging::PixelFormat::Format24bppRgb, pToDisplayImage);

	IntPtr pToDisplayResult(matColorDisplayResult->mpUCArray);
	mBitMapResult=gcnew System::Drawing::Bitmap(mWidthCapturedImage,mHeightCapturedImage,mWidthCapturedImage*3,System::Drawing::Imaging::PixelFormat::Format24bppRgb, pToDisplayResult);

	//motionDetection->initialize( matColorDisplayImage, matColorDisplayResult );

	//initialize trackbars
	//if(mpImageSourceMgr->miMotionThreshold[0]>=0)
	//	barMotionThreshold->Value=mpImageSourceMgr->miMotionThreshold[0];
	
	imageTemp = cvCreateImage( cvSize(mWidthCapturedImage,mHeightCapturedImage), 8, 3 );
	pImageSource320240 = cvCreateImage( cvSize(mWidthCapturedImage,mHeightCapturedImage), 8, 3 );
	distortImg = cvCreateImage( cvSize(mWidthCapturedImage,mHeightCapturedImage), 8, 3 );
	//test_img
	test_img = cvCreateImage( cvSize(mWidthCapturedImage,mHeightCapturedImage), 8, 3 );
	cvZero(test_img);
	CvBox2D test_box;
	test_box.angle = 30;
	test_box.center.x = mWidthCapturedImage/2;
	test_box.center.y = mHeightCapturedImage/2;
	test_box.size.width = 30;
	test_box.size.height = test_box.size.width*1.5;
	cvEllipseBox(test_img, test_box, CV_RGB(250,0,0), -1);
#ifdef _DEBUG
	cvNamedWindow("test_img", 1);
	cvShowImage("test_img", test_img);
#endif
	cvSaveImage("test_img.bmp", test_img);
	//


	//Flags and Index
	miFrameIndex=0;
	mbGrab=false;
	mbStep=false;
	mStartCount = true;
	
	txtFrameIndex->Text = miFrameIndex.ToString();
	textProcessTime->Text = miFrameIndex.ToString();

	readFromAVI = false;
	takephoto = false;
	showCalibLines = false;
	showCalibLines_old = false;
	srcCapture = NULL;
	pImageSource = NULL;

    //motionDetection->srcVideo=cvCreateVideoWriter("srcVideo.avi",-1,22,cvSize(320,240),1);
	//motionDetection->resVideo=cvCreateVideoWriter("resVideo.avi",-1,22,cvSize(320,240),1);

	CAM1_intrinsic = cvCreateMat(3, 3, CV_64F);
	CAM2_intrinsic = cvCreateMat(3, 3, CV_64F);
	CAM1_dist_coeffs = cvCreateMat(1, 4, CV_64F);
	CAM2_dist_coeffs = cvCreateMat(1, 4, CV_64F);
	cvZero(CAM1_intrinsic);
	cvZero(CAM2_intrinsic);
	cvZero(CAM1_dist_coeffs);
	cvZero(CAM2_dist_coeffs);

	cvmSet(CAM1_intrinsic, 0, 0, CAM1_FX);
	cvmSet(CAM1_intrinsic, 0, 2, CAM1_CX);
	cvmSet(CAM1_intrinsic, 1, 1, CAM1_FY);
	cvmSet(CAM1_intrinsic, 1, 2, CAM1_CY);
	cvmSet(CAM1_intrinsic, 2, 2, 1);
	cvmSet(CAM1_dist_coeffs, 0, 0, CAM1_K1);
	cvmSet(CAM1_dist_coeffs, 0, 1, CAM1_K2);
	cvmSet(CAM1_dist_coeffs, 0, 2, CAM1_P1);
	cvmSet(CAM1_dist_coeffs, 0, 3, CAM1_P2);

	cvmSet(CAM2_intrinsic, 0, 0, CAM2_FX);
	cvmSet(CAM2_intrinsic, 0, 2, CAM2_CX);
	cvmSet(CAM2_intrinsic, 1, 1, CAM2_FY);
	cvmSet(CAM2_intrinsic, 1, 2, CAM2_CY);
	cvmSet(CAM2_intrinsic, 2, 2, 1);
	cvmSet(CAM2_dist_coeffs, 0, 0, CAM2_K1);
	cvmSet(CAM2_dist_coeffs, 0, 1, CAM2_K2);
	cvmSet(CAM2_dist_coeffs, 0, 2, CAM2_P1);
	cvmSet(CAM2_dist_coeffs, 0, 3, CAM2_P2);
	//srcCapture_1 = cvCaptureFromCAM(0);
	//srcCapture_2 = cvCaptureFromCAM(0);

	onlineCamNo = NONDEFINED;
	globalThreadNr=0;

	////setup server
	//setupServer();


}
void Form1::setupServer()
{
	//-------------------------------------------//
	// connect to central
	// you can also assign a specific IP
	// connectToServer("192.168.10.201");
	//-------------------------------------------//
	motionDetection->Cam->IPCTrackingClient->connectToServer(/*"192.168.0.182"*/);

	//-------------------------------------------//
	// Subscribe all the messages you need.
	// All the messages you'll use get() function
	// should be subscribed here, or you will get 
	// nothing.
	// The length of argument is not limited. So 
	// you can use this :
	// subscribe( A, B, C, D, ...., NULL );
	//-------------------------------------------//
	//IPCClient.subscribe( SUBGOAL_NAME, ODOMETRY_NAME, NULL );
	motionDetection->Cam->IPCTrackingClient->subscribe( 
		//////////ODOMETRY_NAME, GOAL_NAME,
		//////////HISTCRCB_CAM1_OB1_NAME, 
		//////////HISTCRCB_CAM1_OB2_NAME,
		//////////HISTCRCB_CAM1_OB3_NAME,
		//////////HISTCRCB_CAM1_OB4_NAME,
		//////////HISTCRCB_CAM1_OB5_NAME,
		//////////HISTCRCB_CAM2_OB1_NAME, 
		//////////HISTCRCB_CAM2_OB2_NAME,
		//////////HISTCRCB_CAM2_OB3_NAME,
		//////////HISTCRCB_CAM2_OB4_NAME,
		//////////HISTCRCB_CAM2_OB5_NAME,
		//////////HISTCRCB_CAM3_OB1_NAME, 
		//////////HISTCRCB_CAM3_OB2_NAME,
		//////////HISTCRCB_CAM3_OB3_NAME,
		//////////HISTCRCB_CAM3_OB4_NAME,
		//////////HISTCRCB_CAM3_OB5_NAME,

		//////////TEST_NAME,
		//////////BODYINFO_CAM1_OB1_NAME,
		//////////BODYINFO_CAM1_OB2_NAME,
		//////////BODYINFO_CAM1_OB3_NAME,
		//////////BODYINFO_CAM1_OB4_NAME,
		//////////BODYINFO_CAM1_OB5_NAME,
		//////////BODYINFO_CAM2_OB1_NAME,
		//////////BODYINFO_CAM2_OB2_NAME,
		//////////BODYINFO_CAM2_OB3_NAME,
		//////////BODYINFO_CAM2_OB4_NAME,
		//////////BODYINFO_CAM2_OB5_NAME,
		//////////BODYINFO_CAM3_OB1_NAME,
		//////////BODYINFO_CAM3_OB2_NAME,
		//////////BODYINFO_CAM3_OB3_NAME,
		//////////BODYINFO_CAM3_OB4_NAME,
		//////////BODYINFO_CAM3_OB5_NAME,



		//////////POS3D_CAM1_OB1_NAME,
		//////////POS3D_CAM1_OB2_NAME,
		//////////POS3D_CAM1_OB3_NAME,
		//////////POS3D_CAM1_OB4_NAME,
		//////////POS3D_CAM1_OB5_NAME,
		//////////POS3D_CAM2_OB1_NAME,
		//////////POS3D_CAM2_OB2_NAME,
		//////////POS3D_CAM2_OB3_NAME,
		//////////POS3D_CAM2_OB4_NAME,
		//////////POS3D_CAM2_OB5_NAME,
		//////////POS3D_CAM3_OB1_NAME,
		//////////POS3D_CAM3_OB2_NAME,
		//////////POS3D_CAM3_OB3_NAME,
		//////////POS3D_CAM3_OB4_NAME,
		//////////POS3D_CAM3_OB5_NAME,



		CAMLINE_CAM1_OB1_NAME,
		CAMLINE_CAM1_OB2_NAME,
		CAMLINE_CAM1_OB3_NAME,
		CAMLINE_CAM1_OB4_NAME,
		CAMLINE_CAM1_OB5_NAME,
		CAMLINE_CAM2_OB1_NAME,
		CAMLINE_CAM2_OB2_NAME,
		CAMLINE_CAM2_OB3_NAME,
		CAMLINE_CAM2_OB4_NAME,
		CAMLINE_CAM2_OB5_NAME,
		CAMLINE_CAM3_OB1_NAME,
		CAMLINE_CAM3_OB2_NAME,
		CAMLINE_CAM3_OB3_NAME,
		CAMLINE_CAM3_OB4_NAME,
		CAMLINE_CAM3_OB5_NAME,


		ALIVESTA_CAM1_NAME,
		ALIVESTA_CAM2_NAME,
		ALIVESTA_CAM3_NAME,

		LABELINTEGRATE_CAM1_NAME,
		LABELINTEGRATE_CAM2_NAME,
		LABELINTEGRATE_CAM3_NAME,

		//////////SUBGOAL_NAME, 
		NULL );

	//-------------------------------------------//
	// Publish all the messages you'll send.
	// All the messages you'll use set() function
	// should be published here, or you will see 
	// lots of errors.
	// The length of argument is not limited. So 
	// you can use this :
	// publish( A, B, C, D, ...., NULL );
	//-------------------------------------------//
	//IPCClient.publish( SUBGOAL_NAME, ODOMETRY_NAME, NULL );
	motionDetection->Cam->IPCTrackingClient->publish(
		//////////ODOMETRY_NAME, GOAL_NAME,
		//////////HISTCRCB_CAM1_OB1_NAME, 
		//////////HISTCRCB_CAM1_OB2_NAME,
		//////////HISTCRCB_CAM1_OB3_NAME,
		//////////HISTCRCB_CAM1_OB4_NAME,
		//////////HISTCRCB_CAM1_OB5_NAME,
		//////////HISTCRCB_CAM2_OB1_NAME, 
		//////////HISTCRCB_CAM2_OB2_NAME,
		//////////HISTCRCB_CAM2_OB3_NAME,
		//////////HISTCRCB_CAM2_OB4_NAME,
		//////////HISTCRCB_CAM2_OB5_NAME,
		//////////HISTCRCB_CAM3_OB1_NAME, 
		//////////HISTCRCB_CAM3_OB2_NAME,
		//////////HISTCRCB_CAM3_OB3_NAME,
		//////////HISTCRCB_CAM3_OB4_NAME,
		//////////HISTCRCB_CAM3_OB5_NAME,

		//////////TEST_NAME,
		//////////BODYINFO_CAM1_OB1_NAME,
		//////////BODYINFO_CAM1_OB2_NAME,
		//////////BODYINFO_CAM1_OB3_NAME,
		//////////BODYINFO_CAM1_OB4_NAME,
		//////////BODYINFO_CAM1_OB5_NAME,
		//////////BODYINFO_CAM2_OB1_NAME,
		//////////BODYINFO_CAM2_OB2_NAME,
		//////////BODYINFO_CAM2_OB3_NAME,
		//////////BODYINFO_CAM2_OB4_NAME,
		//////////BODYINFO_CAM2_OB5_NAME,
		//////////BODYINFO_CAM3_OB1_NAME,
		//////////BODYINFO_CAM3_OB2_NAME,
		//////////BODYINFO_CAM3_OB3_NAME,
		//////////BODYINFO_CAM3_OB4_NAME,
		//////////BODYINFO_CAM3_OB5_NAME,



		//////////POS3D_CAM1_OB1_NAME,
		//////////POS3D_CAM1_OB2_NAME,
		//////////POS3D_CAM1_OB3_NAME,
		//////////POS3D_CAM1_OB4_NAME,
		//////////POS3D_CAM1_OB5_NAME,
		//////////POS3D_CAM2_OB1_NAME,
		//////////POS3D_CAM2_OB2_NAME,
		//////////POS3D_CAM2_OB3_NAME,
		//////////POS3D_CAM2_OB4_NAME,
		//////////POS3D_CAM2_OB5_NAME,
		//////////POS3D_CAM3_OB1_NAME,
		//////////POS3D_CAM3_OB2_NAME,
		//////////POS3D_CAM3_OB3_NAME,
		//////////POS3D_CAM3_OB4_NAME,
		//////////POS3D_CAM3_OB5_NAME,



		CAMLINE_CAM1_OB1_NAME,
		CAMLINE_CAM1_OB2_NAME,
		CAMLINE_CAM1_OB3_NAME,
		CAMLINE_CAM1_OB4_NAME,
		CAMLINE_CAM1_OB5_NAME,
		CAMLINE_CAM2_OB1_NAME,
		CAMLINE_CAM2_OB2_NAME,
		CAMLINE_CAM2_OB3_NAME,
		CAMLINE_CAM2_OB4_NAME,
		CAMLINE_CAM2_OB5_NAME,
		CAMLINE_CAM3_OB1_NAME,
		CAMLINE_CAM3_OB2_NAME,
		CAMLINE_CAM3_OB3_NAME,
		CAMLINE_CAM3_OB4_NAME,
		CAMLINE_CAM3_OB5_NAME,


		ALIVESTA_CAM1_NAME,
		ALIVESTA_CAM2_NAME,
		ALIVESTA_CAM3_NAME,

		LABELINTEGRATE_CAM1_NAME,
		LABELINTEGRATE_CAM2_NAME,
		LABELINTEGRATE_CAM3_NAME,

		//////////SUBGOAL_NAME,  
		NULL );




	// you can also use listen( time );
	motionDetection->Cam->IPCTrackingClient->listen();

	//prepare();

	//unsigned threadID;
	//HANDLE hthreads[3];
	//hthreads[0] = (HANDLE)_beginthreadex( NULL, 0, sendThread2, NULL, 0, &threadID );
	//hthreads[1] = (HANDLE)_beginthreadex( NULL, 0, getThread2, NULL, 0, &threadID );
	//hthreads[2] = (HANDLE)_beginthreadex( NULL, 0, getThread_CAM1, NULL, 0, &threadID );

	//while( cvWaitKey(30) != 27)
	//	showImage();

	//WaitForMultipleObjects( 2, hthreads, TRUE, INFINITE );
	//CloseHandle( hthreads[0] );
	//CloseHandle( hthreads[1] );
	//CloseHandle( hthreads[2] );
}
void Form1::mReleaseMyObjects()
{
	cvReleaseVideoWriter(&(motionDetection->srcVideo));
	//cvReleaseVideoWriter((motionDetection->srcVideo));
	cvReleaseVideoWriter(&(motionDetection->resVideo));
	//cvReleaseVideoWriter((motionDetection->resVideo));
	delete matColorDisplayImage;
	delete matColorDisplayResult;
	delete mpCounterForFPS;
	delete mpCounterForProcessingTime;
	delete mImageSourceWebCam;

	OpticalFlow::destroyInstance();
}

IplImage* Form1::getCurrnentImageFromCam()
{
	return mImageSourceWebCam->grabImage();
}

void Form1::mProcessLoop()
{
//	static bool flag = true;
//	_CrtMemState s1, s2, s3;
//_CrtMemCheckpoint( &s2 );
//if ( _CrtMemDifference( &s3, &s1, &s2) && flag == false)
//{
//   _CrtMemDumpStatistics( &s1 );
//   cout <<endl;
//}
	////================================
	if(mbGrab || mbStep)
	{
		mbStep=false;

		miFrameIndex++;

		if (readFromAVI_out && onlineCamNo == CAM2)
		{
			readFromAVI_out = false;
			readFromAVI = true;
			srcCapture = cvCaptureFromAVI(/*"srcVideo[CAM2]201005071811_short_sync_cvtd.avi"*/"srcVideo201004202324_cam2_sync_cvtd.avi"/*"srcVideo201004270017_cam2_short_cvtd.avi"*/);
		}
		if (initializeTracking_out && onlineCamNo == CAM2)
		{
			initializeTracking_out = false;
			initializeTracking = true;
			motionDetection->activateMotionDetection();
		}

		if (readFromAVI && srcCapture)
		{
			pImageSource = cvQueryFrame(srcCapture);
			
			if (pImageSource == NULL)
			{
				mbGrab = !mbGrab;
				return;
			}

			if( pImageSource->origin == IPL_ORIGIN_TL )
				;//cvCopy( pImageSource, pImageSource );
			else
				cvFlip( pImageSource, pImageSource);

		}
		else if (mImageSourceWebCam->cameras > 0)
		{
			pImageSource = getCurrnentImageFromCam();
		}
		else
		{
			//nothing to do
		}


		//////cvResize(pImageSource_1, /*pImageSource320240_1*/distortImg_1, 1);
		//////cvResize(pImageSource_2, /*pImageSource320240_2*/distortImg_2, 1);
		//////cvUndistort2(distortImg_1, pImageSource320240_1, CAM1_intrinsic, CAM1_dist_coeffs);
		//////cvUndistort2(distortImg_2, pImageSource320240_2, CAM2_intrinsic, CAM2_dist_coeffs);

		cvResize(pImageSource, pImageSource320240, 1);


		if (takephoto)
		{
			cvSaveImage("RealTimeImage.bmp",pImageSource320240);
			takephoto = false;
		}
		//IplImage *pImageSource320240 = cvLoadImage("test_imgA.bmp", 1);

		mpCounterForProcessingTime->StartCount();
		if (showCalibLines) mPrintCalibLine(onlineCamNo);
		if (showCalibLines_old) mPrintCalibLine_oldforComparison(onlineCamNo);
		
		motionDetection->startProcess( /*test_img*//*pImageSource*/pImageSource320240);

		mpCounterForProcessingTime->EndCount();

		//================================
		showMessage( motionDetection->getNormalMessage(), motionDetection->getControlMessage() );

		//write image to buffer
		imageSampleReady = false;
		cvCopy(motionDetection->image, recSrc);
		cvCopy(motionDetection->result, recRes);
		imageSampleReady = true;
		cvCopy(motionDetection->image, recSrcPrev);
		cvCopy(motionDetection->result, recResPrev);

	}

	mShowTimer();

	this->pictureBox3->Image=mBitMapImage;
	this->pictureBox4->Image=mBitMapResult;
	//_CrtMemCheckpoint( &s1 );
}
void Form1::mPrintCalibLine(int camid)
{
	if (camid == CAM1)
	{
		////////This is for CAM1 setting
		//////Position3D opticPt1(0,0,220);
		//////Position3D B(158,227,145);
		//////CameraLine3D cam1_line;
		//////cam1_line.initParamBy3DPt(B, opticPt1, B, 0.392338/*, CAM1*/);
		//////cam1_line.setByImgLoc(cvPoint(CAM1_2CX, CAM1_2CY));
		//////Position3D unbiasedB;
		//////cam1_line.getLinePtOnPlane(cam1_line.refPlane, unbiasedB);
		//////cam1_line.initParamBy3DPt_precise(unbiasedB, opticPt1, unbiasedB, 0.5/*0.392338*/, CAM1);	
		
		//This is for CAM1 NEW setting
		Position3D opticPt1(0,0,220);
		Position3D H(276,184,118);
		CameraLine3D cam1_line;
		cam1_line.initParamBy3DPt(H, opticPt1, H, 0.53/*, CAM1*/);
		cam1_line.setByImgLoc(cvPoint(CAM1_2CX, CAM1_2CY));
		Position3D unbiasedH;
		cam1_line.getLinePtOnPlane(cam1_line.refPlane, unbiasedH);
		cam1_line.initParamBy3DPt_precise(unbiasedH, opticPt1, unbiasedH, 0.53, CAM1);	
		printCalibLine_3Dto2D(position(412, 400, 163), position(412, 400, 50), cam1_line, pImageSource320240);
		printCalibLine_3Dto2D(position(301.7, 337.5, 74), position(146, 336.7, 74), cam1_line, pImageSource320240);
		printCalibLine_3Dto2D(position(269, 407, 156.7), position(269, 407, 156.7), cam1_line, pImageSource320240);
		printCalibLine_3Dto2D(position(313, 407, 156.7), position(313, 407, 30), cam1_line, pImageSource320240);

		printCalibLine_3Dto2D(position(355.5, 227, 66), position(355.5, 227-100, 66), cam1_line, pImageSource320240);
		printCalibLine_3Dto2D(position(386-20, 227, 67.5), position(386+20, 227, 67.5), cam1_line, pImageSource320240);
		printCalibLine_3Dto2D(position(418, 407, 142), position(418, 407, 0), cam1_line, pImageSource320240);
		printCalibLine_3Dto2D(position(428.5, 347+30, 10), position(428.5, 347-30, 10), cam1_line, pImageSource320240);
	}	
	else if (camid == CAM2)
	{
		//This is for CAM2 setting
		Position3D opticPt2(412,0,220);
		Position3D G(295,256,104);
		CameraLine3D cam2_line;
		cam2_line.initParamBy3DPt(G, opticPt2, G, 0.8/*, CAM2*/);
		cam2_line.setByImgLoc(cvPoint(CAM2_2CX, CAM2_2CY));
		Position3D unbiasedG;
		cam2_line.getLinePtOnPlane(cam2_line.refPlane, unbiasedG);
		cam2_line.initParamBy3DPt_precise(unbiasedG, opticPt2, unbiasedG, 0.81, CAM2);
	
		printCalibLine_3Dto2D(position(412, 400, 163), position(412, 400, 50), cam2_line, pImageSource320240);
		printCalibLine_3Dto2D(position(301.7, 337.5, 74), position(146, 336.7, 74), cam2_line, pImageSource320240);
		printCalibLine_3Dto2D(position(269, 407, 156.7), position(269, 407, 156.7), cam2_line, pImageSource320240);
		printCalibLine_3Dto2D(position(313, 407, 156.7), position(313, 407, 30), cam2_line, pImageSource320240);
	}
}
void Form1::mPrintCalibLine_oldforComparison(int camid)
{
	if (camid == CAM1)
	{
		//This is for CAM1 setting
		Position3D opticPt1(0,0,220);
		Position3D B(155,227,140);
		CameraLine3D cam1_line;
		cam1_line.initParamBy3DPt(B, opticPt1, B, 0.392338);
		
		printCalibLine_3Dto2D(position(412, 400, 163), position(412, 400, 50), cam1_line, pImageSource320240);
		printCalibLine_3Dto2D(position(301.7, 337.5, 74), position(146, 336.7, 74), cam1_line, pImageSource320240);
		printCalibLine_3Dto2D(position(269, 407, 156.7), position(269, 407, 156.7), cam1_line, pImageSource320240);
		printCalibLine_3Dto2D(position(313, 407, 156.7), position(313, 407, 30), cam1_line, pImageSource320240);
	}
	else if (camid == CAM2)
	{
		//This is for CAM2 setting
		Position3D opticPt2(412,0,220);
		Position3D G(298,256,104);
		CameraLine3D cam2_line;
		cam2_line.initParamBy3DPt(G, opticPt2, G, 0.8);	
	
		printCalibLine_3Dto2D(position(412, 400, 163), position(412, 400, 50), cam2_line, pImageSource320240);
		printCalibLine_3Dto2D(position(301.7, 337.5, 74), position(146, 336.7, 74), cam2_line, pImageSource320240);
		printCalibLine_3Dto2D(position(269, 407, 156.7), position(269, 407, 156.7), cam2_line, pImageSource320240);
		printCalibLine_3Dto2D(position(313, 407, 156.7), position(313, 407, 30), cam2_line, pImageSource320240);
	}
}
void Form1::mShowFPS( int fps )
{
	//frameIndex
	txtFrameIndex->Text = fps.ToString();
}

void Form1::mShowTimer()
{
	//processing time
	textProcessTime->Text = mpCounterForProcessingTime->CalculateElapsedTime().ToString();
}

System::String^	 Form1::stdString2SystemString( string s )
{
	return Marshal::PtrToStringAnsi(static_cast<IntPtr>(const_cast<char *>(s.c_str())));
}

void Form1::showMessage( string *normalMessage, string *controlMessage )
{
	//static int count = 0;
	//count++;
	//if( count % 2 == 0 )
	//{
	//	normalMessagePresenter->AppendText( stdString2SystemString( string("XDDDDDD\n") ) );
	//	controlMessagePresenter->AppendText( stdString2SystemString( string("DDDDDDX\n") ) );
	//}
	//else
	//{
	//	normalMessagePresenter->AppendText( stdString2SystemString( string("XD\n") ) );
	//	controlMessagePresenter->AppendText( stdString2SystemString( string("DX\n") ) );
	//
	//}

	static int mCountNormal = 0;
	static int mCountControl = 0;

	mCountNormal += normalMessage->size();
	mCountControl += controlMessage->size();

	if( mCountNormal >= 1000 )
	{
		normalMessagePresenter->Clear();
		mCountNormal = 0;
	}
	if( mCountControl >= 1000 )
	{
		controlMessagePresenter->Clear();
		mCountControl = 0;

	}

	normalMessagePresenter->AppendText( stdString2SystemString( *normalMessage ) );
	controlMessagePresenter->AppendText( stdString2SystemString( *controlMessage ) );
	
	normalMessage->clear();
	controlMessage->clear();
}