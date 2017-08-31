#include "CAMShift.h"

CAMShift::CAMShift()
{
	width = 320;
	height = 240;
	hdims = 16;
	float hranges_arr[] = {0,180};
    float* hranges = hranges_arr;
	vmin = 20;  vmax = 240;    smin = 10;

	hsv = cvCreateImage(cvSize(width,height),8,3);
	hue = cvCreateImage(cvSize(width,height),8,1);
	mask = cvCreateImage( cvSize(width,height), 8, 1 );
    backproject = cvCreateImage( cvSize(width,height), 8, 1 );
    hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	histimg = cvCreateImage( cvSize(320,200), 8, 3 );
    cvZero( histimg );
}
void CAMShift::init(IplImage * image, IplImage * fgMask, CvRect target)
{
	float max_val = 0.f;
	track_window = target;
	cvCvtColor( image, hsv, CV_BGR2HSV );
	cvInRangeS( hsv, cvScalar(0,smin,MIN(vmin,vmax),0),
					cvScalar(180,256,MAX(vmin,vmax),0), mask );
	cvSplit( hsv, hue, 0, 0, 0 );

	cvAnd( mask, fgMask, mask );
	cvSetImageROI( hue, target );
	cvSetImageROI( mask, target );
	cvCalcHist( &hue, hist, 0, mask );
	cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 );
	cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 );
	cvResetImageROI( hue );
	cvResetImageROI( mask );
	int bin_w = histimg->width / hdims;
	for( int i = 0; i < hdims; i++ )
	{
		int val = cvRound( cvGetReal1D(hist->bins,i)*histimg->height/255 );
		CvScalar color = hsv2rgb(i*180.f/hdims);
		cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
				cvPoint((i+1)*bin_w,histimg->height - val),
				color, -1, 8, 0 );
	}
	cvNamedWindow("targethistogram", 1);
	cvShowImage("targethistogram", histimg);

	//cvRectangle( image, cvPoint(target.x,target.y),
	//	cvPoint(target.x+target.width,target.y+target.height),
	//	CV_RGB(100,100,0),4);
	//cvNamedWindow("initial", 1);
	//cvShowImage("initial", image);

	cvNormalizeHist(hist,1);

	cvCalcBackProject( &hue, backproject, hist );
	cvAnd( backproject, mask, backproject, 0 );
}

CvScalar CAMShift::hsv2rgb( float hue )
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

void CAMShift::iteration(IplImage *image)
{
	cvCvtColor( image, hsv, CV_BGR2HSV );
	cvInRangeS( hsv, cvScalar(0,smin,MIN(vmin,vmax),0),
			    cvScalar(180,256,MAX(vmin,vmax),0), mask );
    cvSplit( hsv, hue, 0, 0, 0 );
	
	cvCalcBackProject( &hue, backproject, hist );
    cvAnd( backproject, mask, backproject, 0 );
    cvCamShift( backproject, track_window,
                cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),
                &track_comp, &track_box );
    track_window = track_comp.rect;
	if( !image->origin )
		track_box.angle = -track_box.angle;
	//cvRectangle( image, cvPoint(track_window.x,track_window.y),
	//	cvPoint(track_window.x+track_window.width,track_window.y+track_window.height),
	//	CV_RGB(100,100,0),4);
    cvEllipseBox( image, track_box, CV_RGB(255,0,0), 5, CV_AA, 0 );

	track_ppos = track_pos;
	track_pos = cvPoint(track_box.center.x,track_box.center.y);
	track_v.x = track_pos.x-track_ppos.x;
	track_v.y = track_pos.y-track_ppos.y;
	track_agl=track_box.angle;
	cout<<"target velocity=("<<track_v.x<<","<<track_v.y<<")."<<endl;
	cout<<"target position=("<<track_pos.x<<","<<track_pos.y<<")."<<endl;
	cout<<"target angle=("<<track_agl<<")."<<endl;

}

