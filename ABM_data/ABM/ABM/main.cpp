#include <stdio.h>
#include <iostream>
using namespace std;
#include <conio.h>
#include <tchar.h>
#include <windows.h>
#include <time.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvcam.h>

#include "process.h"
#include "ABM.h"

CvVideoWriter *writer;
bool need_to_record;

CvCapture * capture;
IplImage *frame;
IplImage* frameA = cvCreateImage(cvSize(WIDTH,HEIGHT),8,3);
IplImage* frameB = cvCreateImage(cvSize(WIDTH,HEIGHT),8,3);
clock_t clock1 = 0, clock2 = 0;
float fps;

CAMShift camShift;
TargetDetect Target;
ABM abm;

int main(int argc, _TCHAR* argv[])
{
	int fpsCounter = 0;

	// User Interface of choosing video
	char buffer[81], fbuffer[80];
	int i, ch;

	cout << "Input Video File Name: " << endl << "(Press ENTER to use camera sequence; '1': single image)" << endl;
	
	for( i = 0; (i < 80) && ((ch = getchar()) != EOF) && (ch != '\n'); i++ )
		buffer[i] = (char)ch;
	
	/* Terminate string with null character: */
	buffer[i] = '\0';


	cout << "Record File : (Press ENTER to ignore this sequence)" << endl;

	for( i = 0; (i < 80) && ((ch = getchar()) != EOF) && (ch != '\n'); i++ )
		fbuffer[i] = (char)ch;
	
	/* Terminate string with null character: */
	fbuffer[i] = '\0';
	

	if ( strcmp( fbuffer , "" ) != 0) {
		writer = cvCreateVideoWriter( fbuffer, -1 , 7.5, cvSize(WIDTH, HEIGHT), 1 );
		need_to_record = true;
	}
	else {
		need_to_record = false;
	}

	if (!count)
	{
		cout<<endl<<endl;
		cout<<"Press Enter to capture background";
		getchar();
	}



	// create window
	cvNamedWindow( "image", 1 );	// 1: 320x240; 0: 640x480
	cvNamedWindow( "outImage", 1 );
	cvNamedWindow( "background", 1);

	// KEY function
	cout << "Hot keys:" << endl 
		 << "Esc - quit the program" << endl
		 << "t/T - start/stop tracking" << endl
		 << "d/D - show/hide (re)drawing" << endl;

	if ( strcmp( buffer , "" ) == 0  )	// Use camera sequence
	{
/*		// for ShowCamVideo
		if( ShowCamVideo((HWND)cvGetWindowHandle("image"), WIDTH, HEIGHT) )
		{
			cvWaitKey(0);
			cvcamStop();
		}
		cvcamExit();
*/
//		capture		= cvCaptureFromCAM( argc == 2 ? argv[1][0] - '0' : 0 );
//		frame		= cvQueryFrame( capture );
		ShowCamVideo( (HWND)cvGetWindowHandle("image"), WIDTH, HEIGHT );

		while ( 1 )
		{
			if(fpsCounter == 0)
				clock1 = clock();

			//cvShowImage( "testImage", frameA );
			//cout << "frameA" << endl;
			//cvWaitKey(0);
			//cvShowImage( "testImage", frameB );
			//cout << "frameB" << endl;
			//cvWaitKey(0);

			process( frameA );
			process( frameB );
			if ( cvWaitKey(1) == 27 )
				break;
			fpsCounter += 2;
			if(fpsCounter == 30) {
				fps = fpsCounter*1000/( clock2 - clock1 );
				fpsCounter = 0;
				//cout << "fps:" << fps << endl;
			}
		}
		//cvReleaseCapture( &capture );
		cvcamStop();
		cvcamExit();

	}
	else {	// Use video sequence

		if ( strcmp( buffer , "1" ) == 0 )	// Use single image
		{
			frame = cvLoadImage( "test2.bmp", 1 );
			//frame = cvLoadImage( "test3.bmp", 1 );

			while ( 1 ) {	// press any key to interrupt
				process( frame );
				if ( cvWaitKey(1) == 27 )
					break;
				cvWaitKey(1);	// wait to show image
			}
		}
		else {	// Use video sequence

			capture		= cvCaptureFromAVI( buffer );		// cvCaptureFromFile cvCaptureFromAVI
			frame		= cvQueryFrame( capture );
		
			while ( frame ) {
				process( frame );
				frame = cvQueryFrame( capture );
				if ( cvWaitKey(1) == 27 )
					break;
			}
			cvReleaseCapture( &capture );
		}
	}

	if ( need_to_record )
		cvReleaseVideoWriter( &writer );

	cvDestroyAllWindows();
	cvReleaseImage( & frame );
	cvReleaseImage( & frameA );
	cvReleaseImage( & frameB );
	processRelease();

	return 0;
}
