#pragma once

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <iostream>
using namespace std;

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvcam.h>

#include "CAMShift.h"
#include "Targetdetect.h"
#include "ABM.h"

#define WIDTH 320
#define HEIGHT 240

extern bool need_to_record;
extern CvVideoWriter *writer;
extern IplImage *frameA, *frameB;
extern clock_t clock2;

extern CAMShift camShift;
extern TargetDetect Target;
extern ABM abm;

void callback( IplImage* frame_capture );
void process( IplImage* image );
void processRelease(void);

int ShowCamVideo( HWND hwnd, int width, int height );

static bool Flag = true;
static bool initial = true;
static bool detect = false;
static int count = 0, motionCount = 0;

static bool tracking = false, show_drawing = false, show_predict = false;
//static IplImage *img, *outImage;
static IplImage * img = cvCreateImage(cvSize(WIDTH,HEIGHT),8,3);
static IplImage * outImage = cvCreateImage(cvSize(WIDTH,HEIGHT),8,3);
static IplImage * grayImage = cvCreateImage(cvSize(WIDTH,HEIGHT),8,1);
static IplImage * bgImage = cvCreateImage(cvSize(WIDTH,HEIGHT),8,1);
static IplImage * motionTarget = cvCreateImage(cvSize(WIDTH,HEIGHT),8,1);
static IplImage * mask = cvCreateImage(cvSize(WIDTH,HEIGHT),8,1);
static CvMat * bgMat = cvCreateMat(HEIGHT, WIDTH, CV_32FC1);
static CvMat * grayMat = cvCreateMat(HEIGHT, WIDTH, CV_32FC1);

#define detect_num 40  //the times we see an object in the observation images
#define thSize 400   //ensure we are ready to track nice object
#define TimeForBG 100 //time for webcam to be stable