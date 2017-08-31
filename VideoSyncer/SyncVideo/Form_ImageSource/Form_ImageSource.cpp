// Form_ImageSource.cpp : main project file.

#include "Form1.h"

using namespace Form_ImageSource;

int main(array<System::String ^> ^args);


template <class T>
inline std::string to_string(const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}


void trans_IplImage_2_ClassOpenCVMat(IplImage* srcImg, ClassOpenCVMat* mMatTemp, ClassOpenCVMat* mMatColorDisplayImage)
{
	CvMat* pDesImage = cvGetMat( srcImg, mMatTemp->mpMat );
	cvCopy( pDesImage, mMatColorDisplayImage->mpMat );
}

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew Form1());	

	return 0;
}


void Form1::mInitializeMyObjects()
{
	this->pictureBox3->Size = System::Drawing::Size(mWidthCapturedImage, mHeightCapturedImage);
	this->pictureBox4->Size = System::Drawing::Size(mWidthCapturedImage, mHeightCapturedImage);

	mImageSourceWebCam = new C_ImageSourceWebCam;
	mImageSourceWebCam->initialize( cvPoint(mWidthCapturedImage,mHeightCapturedImage) ); //->mInitializeGray(1, cvPoint(320,240), miResizeSize[0], miLT[0], miRB[0]);

	mpCounterForFPS=new CPerformanceCounter;
	mpCounterForProcessingTime=new CPerformanceCounter;

	////-------------initialize the set (matColorDisplayImage, pToDisplayImage, mBitMapImage) for GUI real-time display
	matColorDisplayImage=new ClassOpenCVMat;
	matColorDisplayImage->mCreateRGBUCMat(mWidthCapturedImage,mHeightCapturedImage);
	//assign the buffer head pointer to the bitmap
	IntPtr pToDisplayImage(matColorDisplayImage->mpUCArray);
	mBitMapImage=gcnew System::Drawing::Bitmap(mWidthCapturedImage,mHeightCapturedImage,mWidthCapturedImage*3,
		System::Drawing::Imaging::PixelFormat::Format24bppRgb, pToDisplayImage);
	////---------------------------------------------------------------------------------------------------------------

	////-------------initialize the set (matColorDisplayResult, pToDisplayResult, mBitMapResult) for GUI real-time display
	matColorDisplayResult=new ClassOpenCVMat;
	matColorDisplayResult->mCreateRGBUCMat(mWidthCapturedImage,mHeightCapturedImage);
	//assign the buffer head pointer to the bitmap
	IntPtr pToDisplayResult(matColorDisplayResult->mpUCArray);
	mBitMapResult=gcnew System::Drawing::Bitmap(mWidthCapturedImage,mHeightCapturedImage,mWidthCapturedImage*3,
		System::Drawing::Imaging::PixelFormat::Format24bppRgb, pToDisplayResult);
	////---------------------------------------------------------------------------------------------------------------

	////---------allocate matTmp for trans_IplImage_2_ClassOpenCVMat
	matTemp = new ClassOpenCVMat;
	matTemp->mCreateRGBUCMat(mWidthCapturedImage,mHeightCapturedImage);
	////---------------------------------------------------------------------------------------------------------------

	////--------------pass (matColorDisplayImage, matColorDisplayResult) for later use of tracker (OpticalFlow, CAMShift...)
///	motionDetection->initialize( matColorDisplayImage, matColorDisplayResult );
	////---------------------------------------------------------------------------------------------------------------

	//initialize debug printing tool
	txtOut = new fstream;
	txtOut->open("debug_log.txt", ios::out);

	//initialize Video Sync process
	start_sync_video = false;

	// How to use VideoSyncer to sync 2 videos
	// 1. assign 2 videos' start & final frame idxes as (1, 10000)
	// 2. run then "Check Cap Details" to get total length --> then correct final frame idxes
	// 3. run then use "Stop" to find a nice start point for this pair of videos --> then correct start frame idxes
	// 4. run then use "Stop" to find a nice final point for this pair of videos --> then correct final frame idxes
	// repeat 3. & 4. for fine tuning until it reaches acceptable sync time error
	// tuning tips: 
	//				A. final frame increase(decrease) --> this video speed becomes faster(slower)  
	//				B. start frame increase --> this video run in advance, less delay



	mVideoSyncer = new VideoSyncer(	
									//"srcVideo201004202324_cam1_sync2_cvtd.avi", 
									//"test_cam1.avi",
									//"srcVideo201004202324_cam2_sync_cvtd.avi", 
									//"test_cam2.avi",

									//////"3_people_re-entering_CAM1_cvtd.avi", //1410//4800
									//////"3_people_re-entering_CAM1_cvtd(resync).avi",
									//////"3_people_re-entering_CAM2_cvtd.avi", //180//3650
									//////"3_people_re-entering_CAM2_cvtd(resync).avi",

									"3_single_person_re-entering_CAM1_cvtd.avi", //990//3650
									"3_single_person_re-entering_CAM1_cvtd(resync).avi",
									"3_single_person_re-entering_CAM2_cvtd.avi", //471//3200
									"3_single_person_re-entering_CAM2_cvtd(resync).avi",

									//////"3_people_Corr_Correct_CAM1_cvtd.avi", //1999//3700
									//////"3_people_Corr_Correct_CAM1_cvtd(resync).avi",
									//////"3_people_Corr_Correct_CAM2_cvtd.avi", //1056//2980
									//////"3_people_Corr_Correct_CAM2_cvtd(resync).avi",

									//////"2_people_Occ_CAM1_case2_cvtd.avi", //1970//3211
									//////"2_people_Occ_CAM1_case2_cvtd(resync).avi",
									//////"2_people_Occ_CAM2_case2_cvtd.avi", //1280//2600
									//////"2_people_Occ_CAM2_case2_cvtd(resync).avi",

									//////"2_people_Occ_CAM1_case1_cvtd.avi", //2111//2800
									//////"2_people_Occ_CAM1_case1_cvtd(resync).avi",
									//////"2_people_Occ_CAM2_case1_cvtd.avi", //1621//2300
									//////"2_people_Occ_CAM2_case1_cvtd(resync).avi",

									//////"2_people_Geo_Corr_Correct_CAM1_cvtd.avi", //1500//2688
									//////"2_people_Geo_Corr_Correct_CAM1_cvtd(resync).avi",
									//////"2_people_Geo_Corr_Correct_CAM2_cvtd.avi", //700//1873
									//////"2_people_Geo_Corr_Correct_CAM2_cvtd(resync).avi",
									

									REC_SAMPLINGRATE);

	/*mVideoSyncer->initialize_VideoSyncer("srcVideo201004202324_cam1_sync2_cvtd.avi", "test_cam1.avi",
										 "srcVideo201004202324_cam2_sync_cvtd.avi", "test_cam2.avi",
										 REC_SAMPLINGRATE);*/
	mVideoSyncer->initialize_LinearInterpolation(990, 3650, 471, 3200);



	imageDisp1 = cvCreateImage( cvSize(mWidthCapturedImage,mHeightCapturedImage), 8, 3 );
	imageDisp2 = cvCreateImage( cvSize(mWidthCapturedImage,mHeightCapturedImage), 8, 3 );

	cvInitFont( &frameFont, CV_FONT_HERSHEY_PLAIN, 1.1, 1.1, 0, 2);

	//Flags and Index
	miFrameIndex=0;
	mbGrab=false;
	mbStep=false;
	mStartCount = true;
	
	txtFrameIndex->Text = miFrameIndex.ToString();
	textProcessTime->Text = miFrameIndex.ToString();

	take_A_image = 0;
}

void Form1::mReleaseMyObjects()
{
	delete matColorDisplayImage;
	delete matColorDisplayResult;
	delete mpCounterForFPS;
	delete mpCounterForProcessingTime;
	delete mImageSourceWebCam;


	delete mVideoSyncer;

	*txtOut << "haha\n";
	txtOut->close();
	delete txtOut;

}

IplImage* Form1::getCurrnentImageFromCam()
{
	return mImageSourceWebCam->grabImage();
}

void Form1::mProcessLoop()
{
	if(mbGrab || mbStep)
	{
		mbStep=false;

		miFrameIndex++;

		//Convert the gray buffer to the display color buffer
		IplImage *pImageSource = getCurrnentImageFromCam();
		//IplImage *pImageSource = cvLoadImage("take_A_image.bmp", 1);

		if (take_A_image)
		{
			//cvSaveImage("take_image.bmp", pImageSource);
			
			take_A_image = 0;

		}

		mpCounterForProcessingTime->StartCount();

		cvCopy(pImageSource, imageDisp1);
		cvCopy(pImageSource, imageDisp2);
		//cvZero(imageDisp2);
		
		if (start_sync_video)
		{
			//// Process video sync, return 2 current image ptr for later display
			//// input: frame_cnt (with specified start & end index)
			//// output: 2 cam current image ptrs
			mVideoSyncer->syncByShorterFrame_LinearInterp_thenRec(txtOut);
			
			if ((mVideoSyncer->get_camSet_cur_img_ptr(1) == NULL) && (mVideoSyncer->get_camSet_cur_img_ptr(2) == NULL)) 
			{
				mbGrab = false;
				start_sync_video = false;
				std::string nmlString = "";
				std::string crlString = "both video end\n";
				showTrackingMessage(&nmlString, &crlString);
				return;
			}
			else if (mVideoSyncer->get_camSet_cur_img_ptr(1) == NULL)
			{
				mbGrab = false;
				start_sync_video = false;
				std::string nmlString = "";
				std::string crlString = "cam1 (only) video end\n";
				showTrackingMessage(&nmlString, &crlString);
				return;
			}
			else if (mVideoSyncer->get_camSet_cur_img_ptr(2) == NULL)
			{
				mbGrab = false;
				start_sync_video = false;
				std::string nmlString = "";
				std::string crlString = "cam2 (only) video end\n";
				showTrackingMessage(&nmlString, &crlString);
				return;
			}

			//// write 2 video using sync results, no frame idx, can select "no recording"
			//// input: write flag, 2 cam current image ptrs

			//// show 2 cam sync results on GUI panel, with frame idx 
			//// input: 2 current image ptrs
			//// output: show 2 images
			cvCopy(mVideoSyncer->get_camSet_cur_img_ptr(1), imageDisp1); //cam1
			cvCopy(mVideoSyncer->get_camSet_cur_img_ptr(2), imageDisp2); //cam2

			cvFlip( imageDisp1, imageDisp1);
			cvFlip( imageDisp2, imageDisp2);

			std::string str_cam1_frame = "Cam1 Frame#";
			str_cam1_frame = str_cam1_frame + to_string(mVideoSyncer->get_camSet_vC_frame_cnt(1));
			std::string str_cam2_frame = "Cam2 Frame#";
			str_cam2_frame = str_cam2_frame + to_string(mVideoSyncer->get_camSet_vC_frame_cnt(2));
			cvPutText( imageDisp1, str_cam1_frame.c_str(), cvPoint(imageDisp1->width-180, 18), &frameFont, CV_RGB(0,255,0) );
			cvPutText( imageDisp2, str_cam2_frame.c_str(), cvPoint(imageDisp2->width-180, 18), &frameFont, CV_RGB(0,255,0) );

			Sleep(1000/REC_SAMPLINGRATE-10);

		}

		
		// write 2 video using sync results, no frame idx, can select "no recording"
		// input: write flag, 2 cam current image ptrs
		
		trans_IplImage_2_ClassOpenCVMat(imageDisp1, matTemp, matColorDisplayImage);
		trans_IplImage_2_ClassOpenCVMat(imageDisp2, matTemp, matColorDisplayResult);

		mpCounterForProcessingTime->EndCount();

		//================================
///		showTrackingMessage( motionDetection->getNormalMessage(), motionDetection->getControlMessage() );
	}

	mShowTimer();

	////------------display processed (matColorDisplayImage, matColorDisplayResult) on GUI panel
	this->pictureBox3->Image=mBitMapImage; // show image
	this->pictureBox4->Image=mBitMapResult; // show image
	////---------------------------------------------------------------------------------------------------------------

}

void Form1::MarshalString( String ^ s, string& os )
{
	   using namespace Runtime::InteropServices;
	   const char* chars = 
		  (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	   os = chars;
	   Marshal::FreeHGlobal(IntPtr((void*)chars));
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

void Form1::showTrackingMessage( string *normalMessage, string *controlMessage )
{
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

void Form1::showPMMessage( const string& result)
{
	static int mCountControl = 0;

	mCountControl += result.size();

	normalMessagePresenter->AppendText( stdString2SystemString( result ) + "\n" );
}