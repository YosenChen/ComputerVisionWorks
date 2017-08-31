#include "C_ImageSourceWebCam.h"

int            bufferIndex=0;
IplImage       *frame[2];


void __cdecl callback(IplImage* _image)
{
	cvCopy( _image, frame[(bufferIndex+1)%2], 0 );
	cvFlip( frame[(bufferIndex+1)%2], NULL, -1 );	

	bufferIndex=(bufferIndex+1)%2;
}

C_ImageSourceWebCam::~C_ImageSourceWebCam()
{
	// must precede delete statements
	cvcamStop();
	cvcamExit();
	
	cvReleaseImage( &frame[0] );
	cvReleaseImage( &frame[1] );
}

void C_ImageSourceWebCam::initialize( CvPoint grabSize )
{
	cameras = cvcamGetCamerasCount();
	if ( cameras > 0 )
	{
		VidFormat vidFmt={grabSize.x, grabSize.y, 30.0};
		cvcamSetProperty(0, CVCAM_PROP_ENABLE, CVCAMTRUE);
		cvcamSetProperty(0, CVCAM_PROP_CALLBACK, &callback);
		cvcamSetProperty(0, CVCAM_PROP_SETFORMAT, &vidFmt);
	}

	frame[0] = cvCreateImage(cvSize(grabSize.x, grabSize.y),8,3);
	frame[1] = cvCreateImage(cvSize(grabSize.x, grabSize.y),8,3);

	cvcamInit();
	cvcamStart();
}

IplImage* C_ImageSourceWebCam::grabImage()
{
	return frame[bufferIndex];
}
