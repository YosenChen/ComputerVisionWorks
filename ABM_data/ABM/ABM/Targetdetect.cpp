//---------------------------------------------------------------------------


#pragma hdrstop

#include "TargetDetect.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

TargetDetect::TargetDetect( IplImage * src, IplImage * bg, IplImage * fgMask, CvRect * boundBox, int num)
	:srcImage(src), bgImage(bg), fgMask(fgMask), boundBox(boundBox), targetnum(num)
{
	hsv = cvCreateImage(cvSize(320,240), 8, 3);
	hue = cvCreateImage(cvSize(320,240), 8, 1);
	mmask = cvCreateImage(cvSize(320,240), 8, 1);
	backproject = cvCreateImage(cvSize(320,240), 8, 1);
	backproject2 = cvCreateImage(cvSize(320,240), 8, 1);

	IplImage * skin = 0;
	int hdims = 16;
	float hranges_arr[] = {0,180};
    float* hranges = hranges_arr;

	skinHist1 = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	skinHist2 = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );

	skin = cvLoadImage( "skin.jpg", 1);
	CalcHistogram( skin, skinHist1 );
	cvReleaseImage( & skin );

	skin = cvLoadImage( "skin.bmp", 1);
	CalcHistogram( skin, skinHist2 );
	
	//ShowHistogram( skinHist1 );
	cvReleaseImage( & skin );
}

TargetDetect::~TargetDetect() {
	cvReleaseImage( & srcImage );
	cvReleaseImage( & bgImage );
	cvReleaseImage( & fgMask );
	//delete []boundBox;
}

void TargetDetect::ForegroundSegment(IplImage * srcImage, IplImage * bgImage, IplImage * fgMask) {
	cvAbsDiff( srcImage, bgImage, fgMask );
	cvThreshold( fgMask, fgMask, 30, 255, CV_THRESH_BINARY);
	cvSmooth( fgMask, fgMask );
	cvErode( fgMask, fgMask );
	cvDilate( fgMask, fgMask );
	cvThreshold( fgMask, fgMask, 10, 255, CV_THRESH_BINARY);
}

void TargetDetect::RemoveSkinColor(IplImage * srcImage, IplImage * fgMask) {
	//cvNamedWindow( "backproject", 1);
	cvCvtColor( srcImage, hsv, CV_BGR2HSV );
    cvInRangeS( hsv, cvScalar(0,smin,MIN(vmin,vmax),0),
                    cvScalar(180,256,MAX(vmin,vmax),0), mmask );
    cvSplit( hsv, hue, 0, 0, 0 );

	cvCalcBackProject( &hue, backproject, skinHist1 );
	cvCalcBackProject( &hue, backproject2, skinHist2);
	cvOr( backproject, backproject2, backproject );
	//cvShowImage( "backproject", backproject );
	cvSmooth( backproject, backproject );
	cvDilate( backproject, backproject, 0, 3 );
	cvErode( backproject, backproject );

    cvAnd( backproject, mmask, backproject, 0 );
	cvThreshold( backproject, backproject, 10, 255, CV_THRESH_BINARY_INV );
	cvAnd( fgMask, backproject, fgMask, 0 );
}

void TargetDetect::ConnectedComponent(IplImage * fgMask) {
	IplImage * temp = 0;
	CvMemStorage * storage;
	CvSeq *	contour = 0;
	boundBox = new CvRect [300];
	targetnum = 0;
	motionNZero = 0; boundNZero = 0;

	temp = cvCreateImage( cvSize(fgMask->width,fgMask->height), 8, 1 );
    storage = cvCreateMemStorage(0);
	cvCopy( fgMask, temp );

	cvSmooth( temp, temp );
	cvErode( temp, temp, 0, 2 );
	cvDilate( temp, temp );

	cvFindContours( temp, storage, & contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
	if( contour != 0) {
		motionBox = cvBoundingRect( contour );
		cvSetImageROI( fgMask, motionBox );
		motionNZero = cvCountNonZero( fgMask );
		cvResetImageROI( fgMask );
	}
	for( ; contour != 0; contour = contour->h_next ) {
		boundBox[targetnum] = cvBoundingRect( contour );
		if(boundBox[targetnum].width>15 && boundBox[targetnum].height>15 ) {
			cvRectangle( temp, cvPoint(boundBox[targetnum].x,boundBox[targetnum].y),
				cvPoint(boundBox[targetnum].x+boundBox[targetnum].width,boundBox[targetnum].y+boundBox[targetnum].height),
				cvScalar(200), 4);
			cvSetImageROI( fgMask, boundBox[targetnum] );
			boundNZero = cvCountNonZero( fgMask );
			cvResetImageROI( fgMask );

			if( boundNZero > motionNZero ) {
				motionBox = boundBox[targetnum];
				motionNZero = boundNZero;
			}
			targetnum++;
		}
	}

	cvReleaseImage( & temp );
	cvClearMemStorage( storage );
}

void TargetDetect::CalcHistogram( IplImage * skin, CvHistogram * skinHist ){
	IplImage * skinHSV = 0, * skinHue = 0, * skinMask = 0;
	skinHSV = cvCreateImage(cvSize(skin->width,skin->height), 8, 3);
	skinHue = cvCreateImage(cvSize(skin->width,skin->height), 8, 1);
	skinMask = cvCreateImage(cvSize(skin->width,skin->height), 8, 1);

	float max_val = 0.f;
	vmin = 20; vmax = 240; smin = 10;

	//int hdims = 16;
	//float hranges_arr[] = {0,180};
 //   float* hranges = hranges_arr;
	//skinHist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );

	cvCvtColor( skin, skinHSV, CV_BGR2HSV );
    cvInRangeS( skinHSV, cvScalar(0,smin,MIN(vmin,vmax),0),
                    cvScalar(180,256,MAX(vmin,vmax),0), skinMask );
    cvSplit( skinHSV, skinHue, 0, 0, 0 );
    cvCalcHist( &skinHue, skinHist, 0, skinMask );
	cvNormalizeHist( skinHist, 1 );
	cvGetMinMaxHistValue( skinHist, 0, &max_val, 0, 0 );
    cvConvertScale( skinHist->bins, skinHist->bins, max_val ? 255. / max_val : 0., 0 );

	cvReleaseImage( & skinHSV );
	cvReleaseImage( & skinHue );
	cvReleaseImage( & skinMask );
}

void TargetDetect::ShowHistogram( CvHistogram * skinHist ){
	cvNamedWindow("skin Histogram", 1);
	IplImage * histimg = 0;
	histimg = cvCreateImage( cvSize(320,200), 8, 3 );
    cvZero( histimg );


	float max_val = 0.f;
	int hdims = 16;

    int bin_w = histimg->width / hdims;
    for( int i = 0; i < hdims; i++ )
    {
		int val = cvRound( cvGetReal1D(skinHist->bins,i)*histimg->height/255 );
        CvScalar color = hsv2rgb(i*180.f/hdims);
        cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
                 cvPoint((i+1)*bin_w,histimg->height - val),
                 color, -1, 8, 0 );
    }
	cvShowImage("skin Histogram", histimg );
}

CvScalar TargetDetect::hsv2rgb( float hue )
{
    int rgb[3], p, sector;
    static const int sector_data[][3]=
        {{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
    hue *= 0.033333333333333333333333333333333f;
    sector = cvFloor(hue);
    p = cvRound(255*(hue - sector));
    p ^= sector & 1 ? 255 : 0;

    rgb[sector_data[sector][0]] = 255;
    rgb[sector_data[sector][1]] = 0;
    rgb[sector_data[sector][2]] = p;

    return cvScalar(rgb[2], rgb[1], rgb[0],0);
}
