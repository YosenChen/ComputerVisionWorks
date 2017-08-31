#ifndef CAMSHIFTH
#define CAMSHIFTH

#include <iostream>
#include <conio.h>
#include <windows.h>
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "highgui.h"

using namespace std;

class CAMShift
{
     private:
		 //CvHistogram *hist;
		 CvPoint track_v;
		 CvPoint track_pos;
		 CvPoint track_ppos;
		 float track_agl;


     public:
		 CAMShift();

		 IplImage *backproject, *histimg, *hsv, *hue, *mask;
		 CvRect track_window;
		 CvBox2D track_box;
		 CvConnectedComp track_comp;
		 int vmin, vmax, smin, width, height, hdims;
		 CvHistogram *hist;

		 void init(IplImage *, IplImage *, CvRect );
		 CvScalar hsv2rgb( float );
		 void iteration(IplImage * );
};

#endif