#include "process.h"

void callback( IplImage* frame_capture )
{
	// double buffer for capturing real-time images and processing the newest one
	if ( Flag )	{
		cvCopy( frame_capture, frameA, 0 );
	}
	else {
		cvCopy( frame_capture, frameB, 0 );
	}
}

int ShowCamVideo( HWND hwnd, int width, int height )
{
    VidFormat vidFmt={ width, height, 30.0};
	int w = width;
	int h = height;

	//returns the number of available cameras in the system
    int ncams = cvcamGetCamerasCount( );

	//Selects the 1-st found camera
    cvcamSetProperty( 0, CVCAM_PROP_ENABLE, CVCAMTRUE );

	//this callback will process every frame
    cvcamSetProperty( 0, CVCAM_PROP_CALLBACK, callback );

	// Selects a window for video rendering
    cvcamSetProperty( 0, CVCAM_PROP_WINDOW, &hwnd );

    // Set Video Format Property
    cvcamSetProperty( 0, CVCAM_PROP_SETFORMAT, &vidFmt);

    // Set the width of the output video
    //cvcamSetProperty( 0, CVCAM_RNDWIDTH, &w);
    // Set the height of the output video
    //cvcamSetProperty( 0, CVCAM_RNDHEIGHT, &h);

    if( !cvcamInit() )
        return 0;

    cvcamStart();

    return 1;
}

void process( IplImage * image )	// MAIN processing function
{
	clock2 = clock();

	Flag = !Flag;
	if( image->origin == IPL_ORIGIN_TL )
		cvFlip(image,img,0);
	else
		cvCopy( image, img, 0 );

	//cvCvtColor(img, grayImage, CV_BGR2GRAY); 
	cvSplit(img, /*hue*/grayImage, 0, 0, 0);

	if(initial) {
		cvCopy( grayImage, bgImage, 0 );

		//CvScalar one;
		//one = cvScalarAll(255);
		//cvSet(motionTarget, one);
		cvZero(motionTarget);
		//cvZero(scoreImg);
		if(count < TimeForBG) count ++;
		if(count == TimeForBG) {
			initial = false;
			count ++;
			cout << "construct the background OK" << endl;
		}
	}
	else{
		/*double alpha;
		if(detect)	alpha = 0.001;
		else		alpha = 0.0005;
		cvConvert(bgImage, bgMat);
		cvConvert(grayImage, grayMat);
		cvRunningAvg(grayMat, bgMat, alpha, mask);
		cvConvert(bgMat, bgImage);*/
	}

	cvCopy( img, outImage, 0 );
	cvShowImage( "background", bgImage );

	int i, j, KEY;

	if (initial) tracking = false;
	else tracking = true;

	
	cvNamedWindow( "camshift", 1);

	//ABM abm;

	if ( tracking )
	{
		if( 1/*motionCount < detect_num*/ ) {
			abm.Update(grayImage);
			abm.Evaluation(grayImage);
			abm.show();
			/*cvEqualizeHist(scoreImg, scoreImg);
			cvNamedWindow("ABM", 1);
			cvShowImage("ABM", scoreImg);*/



			//Target.ForegroundSegment(grayImage, bgImage, motionTarget);
			////Target.RemoveSkinColor(img, motionTarget);
			//Target.ConnectedComponent(motionTarget);
			//if (Target.motionBox.width + Target.motionBox.height >= thSize)
			//	motionCount = 0;
			//cvRectangle( motionTarget, cvPoint(Target.motionBox.x,Target.motionBox.y),
			//	cvPoint(Target.motionBox.x+Target.motionBox.width,Target.motionBox.y+Target.motionBox.height),
			//	cvScalar(200),4);
			//cvThreshold(motionTarget, mask, 180, 255, CV_THRESH_BINARY_INV);
			////if( Target.motionNZero > 50 ) {
			////	//¥i¥H¥[template matching
			////	motionCount ++;
			////}
			//cvShowImage("outImage", motionTarget);
			//cvShowImage("camshift", img);
			if( motionCount == detect_num ) {
			//	//smaller the 
			//	Target.motionBox.x = Target.motionBox.x+Target.motionBox.width*0.15;
			//	Target.motionBox.y = Target.motionBox.y + Target.motionBox.height * 0.15;
			//	Target.motionBox.width = Target.motionBox.width * 0.7;
			//	Target.motionBox.height = Target.motionBox.height * 0.7;

			//	camShift.init( img, motionTarget, Target.motionBox );
			//	camShift.iteration( img );
				motionCount ++;
			}
		}
		else {
			//camShift.iteration( img );
			//cvShowImage("camshift", img);
		}
	}
	else	// Reset
	{
		//cvZero(motionTarget);
		motionCount = 0;
		/*Target.motionBox.width = 0; Target.motionBox.height = 0;*/
		//cvShowImage("camshift", img);
		//cvShowImage("outImage", motionTarget);
		//cvDestroyWindow( "targethistogram" );
	}

	// record processing result
	if ( need_to_record )
        cvWriteFrame( writer, outImage );
}

void processRelease()
{
	cvReleaseImage( & img );
	cvReleaseImage( & outImage );
	cvReleaseImage( & grayImage );
	cvReleaseImage( & bgImage );
	cvReleaseImage( & motionTarget );
	cvReleaseImage( & mask );
	cvReleaseMat( & bgMat );
}