#include "stdafx.h"
#include "likelihood.h"


likelihood::likelihood()
{
	fileOut3.open("error report3.txt",ios::out);
	sqrt_2 = sqrt(2.0);

	float * Cr_ranges, * Cb_ranges;
	Cr_ranges = new float[2];
	Cb_ranges = new float[2];
	Cr_ranges[0] = Cb_ranges[0] = RANGE_MIN;
	Cr_ranges[1] = Cb_ranges[1] = RANGE_MAX;

	int hist_size[] = { Cr_BINS, Cb_BINS };
	float * ranges[] = { Cr_ranges, Cb_ranges };

	ref_hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	current_hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );

	for (int h=0; h<MAX_TARGET_NUM; h++)
	{
		ref_texture[h] = cvCreateImage( cvSize(TEMPLATE_WIDTH,TEMPLATE_HEIGHT), 8, 1 );
		prev_ref_texture[h] = cvCreateImage( cvSize(TEMPLATE_WIDTH,TEMPLATE_HEIGHT), 8, 1 );
		ref_tex_cnt[h] = 0;
	}
	
	scale_img = cvCreateImage( cvSize(TEMPLATE_WIDTH,TEMPLATE_HEIGHT), 8, 1 );
	colormask = cvCreateImage( cvSize(WIDTH, HEIGHT), 8, 1 );

	texture_count = 0;
	ellipse_ratio = ELLIPSE_RATIO;//default value

	// pre-construct the sample points with various radius
	M_circleedgesamples = NULL;
	SampleEllipseContour();
	ConstructSINCOSR();



	//bottle model construction
	bottle = cvLoadImage("bottle0a.bmp", 1);
	gray = cvCreateImage(cvGetSize(bottle), 8, 1 );
	cvCvtColor(bottle, gray, CV_BGR2GRAY);

	long double sumX = 0, sumY = 0;
	long int count = 0;
	for (int g=0; g<cvGetSize(gray).width; g++)
	{
		for (int n=0; n<cvGetSize(gray).height; n++)
		{
			if (cvGetReal2D(gray, n, g))
			{
				count++;
				sumX += g;
				sumY += n;
			}
		}
	}
	double centerX = sumX/count;
	double centerY = sumY/count;
	//fileOut3 << "centerX = " << centerX << endl;
	//fileOut3 << "centerY = " << centerY << endl;

	CvPoint center = cvPoint(centerX, centerY);
	cvCircle(bottle, center, 3, CV_RGB(255, 0, 0));
#ifdef _DEBUG
	cvNamedWindow("bottle", 1);
	cvShowImage("bottle", bottle);
#endif
	//ConstructSINCOSR();
	
	for (int i=0; i<circle_edge_sample_number; i++)
	{
		for (int r=0; r<0.5*cvGetSize(gray).width; r++)
		{
			if (cvGetReal2D(gray, centerY /*+*/- getRotatedY(r, 0, i), centerX + getRotatedX(r, 0, i)) < 10)
			{
				R[i] = r;
				break;
			}
		}
	}
	for (int i=0; i<circle_edge_sample_number; i++)
	{
		//fileOut3 << "i = " << i << ", r = " << R[i] << endl;
	}
	for (int i=0; i<circle_edge_sample_number; i++)
	{
		if (i < circle_edge_sample_number-1)
			cvLine(bottle, cvPoint(centerX + getRotatedX(R[i], 0, i),centerY /*+*/- getRotatedY(R[i], 0, i)),
				cvPoint(centerX + getRotatedX(R[i+1], 0, i+1),centerY /*+*/- getRotatedY(R[i+1], 0, i+1)), CV_RGB(0,255,0));
		else // i = circle_edge_sample_number-1, last one
			cvLine(bottle, cvPoint(centerX + getRotatedX(R[i], 0, i),centerY /*+*/- getRotatedY(R[i], 0, i)),
				cvPoint(centerX + getRotatedX(R[0], 0, 0),centerY /*+*/- getRotatedY(R[0], 0, 0)), CV_RGB(0,255,0));
	}
#ifdef _DEBUG
	cvNamedWindow("bottle", 1);
	cvShowImage("bottle", bottle);
#endif
	for (int i=0; i<circle_edge_sample_number; i++)
	{
		if (i < circle_edge_sample_number-1)
			cvLine(bottle, cvPoint(centerX + getRotatedX(R[i]*0.3, 0, i),centerY /*+*/- getRotatedY(R[i]*0.3, 0, i)),
				cvPoint(centerX + getRotatedX(R[i+1]*0.3, 0, i+1),centerY /*+*/- getRotatedY(R[i+1]*0.3, 0, i+1)), CV_RGB(0,255,0));
		else // i = circle_edge_sample_number-1, last one
			cvLine(bottle, cvPoint(centerX + getRotatedX(R[i]*0.3, 0, i),centerY /*+*/- getRotatedY(R[i]*0.3, 0, i)),
				cvPoint(centerX + getRotatedX(R[0]*0.3, 0, 0),centerY /*+*/- getRotatedY(R[0]*0.3, 0, 0)), CV_RGB(0,255,0));
	}



	//MAP_data drawing test
	for (int i=0; i<circle_edge_sample_number*10; i++)
	{
		for (int r=0; r<0.5*cvGetSize(gray).width; r++)
		{
			if (cvGetReal2D(gray, centerY /*+*/- MAPgetRotatedY(r, 0, i), centerX + MAPgetRotatedX(r, 0, i)) < 10)
			{
				MAP_R[i] = r;
				break;
			}
		}
	}
	for (int i=0; i<circle_edge_sample_number*10; i++)
	{
		//fileOut3 << "i = " << i << ", map_r = " << MAP_R[i] << endl;
	}
	for (int i=0; i<circle_edge_sample_number*10; i++)
	{
		if (i < circle_edge_sample_number*10-1)
			cvLine(bottle, cvPoint(centerX + MAPgetRotatedX(MAP_R[i], 0, i),centerY /*+*/- MAPgetRotatedY(MAP_R[i], 0, i)),
				cvPoint(centerX + MAPgetRotatedX(MAP_R[i+1], 0, i+1),centerY /*+*/- MAPgetRotatedY(MAP_R[i+1], 0, i+1)), CV_RGB(0,255,0));
		else // i = circle_edge_sample_number-1, last one
			cvLine(bottle, cvPoint(centerX + MAPgetRotatedX(MAP_R[i], 0, i),centerY /*+*/- MAPgetRotatedY(MAP_R[i], 0, i)),
				cvPoint(centerX + MAPgetRotatedX(MAP_R[0], 0, 0),centerY /*+*/- MAPgetRotatedY(MAP_R[0], 0, 0)), CV_RGB(0,255,0));
	}
#ifdef _DEBUG
	cvNamedWindow("bottle", 1);
	cvShowImage("bottle", bottle);
	//MAP_data drawing test
#endif	
	
	cvLine(bottle, cvPoint(10,20), cvPoint(30, 25), CV_RGB(100, 200, 10), 5);
	cvCircle(bottle, cvPoint(40,10), 3, CV_RGB(200,0,100),3);
	CvBox2D box;
	box.angle = 20;
	box.center.x = 10;
	box.center.y = 100;
	box.size.width = 20;
	box.size.height = 50;
	cvEllipseBox(bottle, box, CV_RGB(20,20,20), -1);
	cvRectangle(bottle, cvPoint(20, 50), cvPoint(40, 90), CV_RGB(200, 250, 0), -1);
	cvRectangle(bottle, cvPoint(80, 50), cvPoint(40, 90), CV_RGB(200, 0, 0), -1);
#ifdef _DEBUG
	cvNamedWindow("bottle", 1);
	cvShowImage("bottle", bottle);
#endif
	//
	ck = cvCreateStructuringElementEx( /*11*/17, /*11*/17, 8, 8, CV_SHAPE_ELLIPSE, NULL );
	erosion = cvCreateStructuringElementEx( /*11*/7, /*11*/7, 3, 3, CV_SHAPE_ELLIPSE, NULL );
	bigErosion = cvCreateStructuringElementEx( 11, 11, 5, 5, CV_SHAPE_ELLIPSE, NULL );
	smlErosion = cvCreateStructuringElementEx( /*11*/4, /*11*/4, 2, 2, CV_SHAPE_ELLIPSE, NULL );
	bp_img = cvCreateImage( cvSize(WIDTH,HEIGHT), 8, 1 );
	morphImage = cvCreateImage( cvSize(WIDTH,HEIGHT), 8, 1 );
	morph_half = cvCreateImage( cvSize(WIDTH/2,HEIGHT/2), 8, 1 );
	src_half = cvCreateImage( cvSize(WIDTH/2,HEIGHT/2), 8, 3 );
	temp_half = cvCreateImage( cvSize(WIDTH/2,HEIGHT/2), 8, 1 );
	sml_bkpj = cvCreateImage( cvSize(WIDTH/2,HEIGHT/2), 8, 1 );
	sml_CC_out = cvCreateImage( cvSize(WIDTH/2,HEIGHT/2), 8, 1 );
	CC_out = cvCreateImage( cvSize(WIDTH,HEIGHT), 8, 1 );

	X_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	Y_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	R_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	Theta_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );

	bp_storage = cvCreateMemStorage(0);
	pt1 = new CvPoint[4];

	


	delete [] Cr_ranges;
	delete [] Cb_ranges;

#ifdef FPS
	fps_counter = 0;
	fps_1 = fps_2 = fps_3 = fps_4 = fps_5 = fps_6 = 0;
#endif
}
likelihood::likelihood(double elp_ratio)
{
	fileOut3.open("error report3.txt",ios::out);
	sqrt_2 = sqrt(2.0);

	float * Cr_ranges, * Cb_ranges;
	Cr_ranges = new float[2];
	Cb_ranges = new float[2];
	Cr_ranges[0] = Cb_ranges[0] = RANGE_MIN;
	Cr_ranges[1] = Cb_ranges[1] = RANGE_MAX;

	int hist_size[] = { Cr_BINS, Cb_BINS };
	float * ranges[] = { Cr_ranges, Cb_ranges };

	ref_hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	current_hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );

	for (int h=0; h<MAX_TARGET_NUM; h++)
	{	
		ref_texture[h] = cvCreateImage( cvSize(TEMPLATE_WIDTH,TEMPLATE_HEIGHT), 8, 1 );
		prev_ref_texture[h] = cvCreateImage( cvSize(TEMPLATE_WIDTH,TEMPLATE_HEIGHT), 8, 1 );
		ref_tex_cnt[h] = 0;
	}

	scale_img = cvCreateImage( cvSize(TEMPLATE_WIDTH,TEMPLATE_HEIGHT), 8, 1 );
	colormask = cvCreateImage( cvSize(WIDTH, HEIGHT), 8, 1 );

	texture_count = 0;

	// pre-construct the sample points with various radius
	//ellipse_ratio = elp_ratio;
	ellipse_ratio = ELLIPSE_RATIO;//default value
	M_circleedgesamples = NULL;
	SampleEllipseContour();
	ConstructSINCOSR();


	//bottle model construction
	bottle = cvLoadImage("bottle0a.bmp", 1);
	gray = cvCreateImage(cvGetSize(bottle), 8, 1 );
	cvCvtColor(bottle, gray, CV_BGR2GRAY);

	long double sumX = 0, sumY = 0;
	long int count = 0;
	for (int g=0; g<cvGetSize(gray).width; g++)
	{
		for (int n=0; n<cvGetSize(gray).height; n++)
		{
			if (cvGetReal2D(gray, n, g))
			{
				count++;
				sumX += g;
				sumY += n;
			}
		}
	}
	double centerX = sumX/count;
	double centerY = sumY/count;
	//fileOut3 << "centerX = " << centerX << endl;
	//fileOut3 << "centerY = " << centerY << endl;

	CvPoint center = cvPoint(centerX, centerY);
	cvCircle(bottle, center, 3, CV_RGB(255, 0, 0));
#ifdef _DEBUG
	cvNamedWindow("bottle", 1);
	cvShowImage("bottle", bottle);
#endif
	//ConstructSINCOSR();
	
	for (int i=0; i<circle_edge_sample_number; i++)
	{
		for (int r=0; r<0.5*cvGetSize(gray).width; r++)
		{
			if (cvGetReal2D(gray, centerY /*+*/- getRotatedY(r, 0, i), centerX + getRotatedX(r, 0, i)) < 10)
			{
				R[i] = r;
				break;
			}
		}
	}
	for (int i=0; i<circle_edge_sample_number; i++)
	{
		//fileOut3 << "i = " << i << ", r = " << R[i] << endl;
	}
	for (int i=0; i<circle_edge_sample_number; i++)
	{
		if (i < circle_edge_sample_number-1)
			cvLine(bottle, cvPoint(centerX + getRotatedX(R[i], 0, i),centerY /*+*/- getRotatedY(R[i], 0, i)),
				cvPoint(centerX + getRotatedX(R[i+1], 0, i+1),centerY /*+*/- getRotatedY(R[i+1], 0, i+1)), CV_RGB(0,255,0));
		else // i = circle_edge_sample_number-1, last one
			cvLine(bottle, cvPoint(centerX + getRotatedX(R[i], 0, i),centerY /*+*/- getRotatedY(R[i], 0, i)),
				cvPoint(centerX + getRotatedX(R[0], 0, 0),centerY /*+*/- getRotatedY(R[0], 0, 0)), CV_RGB(0,255,0));
	}

#ifdef _DEBUG
	cvNamedWindow("bottle", 1);
	cvShowImage("bottle", bottle);
#endif
	for (int i=0; i<circle_edge_sample_number; i++)
	{
		if (i < circle_edge_sample_number-1)
			cvLine(bottle, cvPoint(centerX + getRotatedX(R[i]*0.3, 0, i),centerY /*+*/- getRotatedY(R[i]*0.3, 0, i)),
				cvPoint(centerX + getRotatedX(R[i+1]*0.3, 0, i+1),centerY /*+*/- getRotatedY(R[i+1]*0.3, 0, i+1)), CV_RGB(0,255,0));
		else // i = circle_edge_sample_number-1, last one
			cvLine(bottle, cvPoint(centerX + getRotatedX(R[i]*0.3, 0, i),centerY /*+*/- getRotatedY(R[i]*0.3, 0, i)),
				cvPoint(centerX + getRotatedX(R[0]*0.3, 0, 0),centerY /*+*/- getRotatedY(R[0]*0.3, 0, 0)), CV_RGB(0,255,0));
	}


	//MAP_data drawing test
	for (int i=0; i<circle_edge_sample_number*10; i++)
	{
		for (int r=0; r<0.5*cvGetSize(gray).width; r++)
		{
			if (cvGetReal2D(gray, centerY /*+*/- MAPgetRotatedY(r, 0, i), centerX + MAPgetRotatedX(r, 0, i)) < 10)
			{
				MAP_R[i] = r;
				break;
			}
		}
	}
	for (int i=0; i<circle_edge_sample_number*10; i++)
	{
		//fileOut3 << "i = " << i << ", map_r = " << MAP_R[i] << endl;
	}
	for (int i=0; i<circle_edge_sample_number*10; i++)
	{
		if (i < circle_edge_sample_number*10-1)
			cvLine(bottle, cvPoint(centerX + MAPgetRotatedX(MAP_R[i], 0, i),centerY /*+*/- MAPgetRotatedY(MAP_R[i], 0, i)),
				cvPoint(centerX + MAPgetRotatedX(MAP_R[i+1], 0, i+1),centerY /*+*/- MAPgetRotatedY(MAP_R[i+1], 0, i+1)), CV_RGB(0,255,0));
		else // i = circle_edge_sample_number-1, last one
			cvLine(bottle, cvPoint(centerX + MAPgetRotatedX(MAP_R[i], 0, i),centerY /*+*/- MAPgetRotatedY(MAP_R[i], 0, i)),
				cvPoint(centerX + MAPgetRotatedX(MAP_R[0], 0, 0),centerY /*+*/- MAPgetRotatedY(MAP_R[0], 0, 0)), CV_RGB(0,255,0));
	}
#ifdef _DEBUG
	cvNamedWindow("bottle", 1);
	cvShowImage("bottle", bottle);
	//MAP_data drawing test
#endif

	cvLine(bottle, cvPoint(10,20), cvPoint(30, 25), CV_RGB(100, 200, 10), 5);
	cvCircle(bottle, cvPoint(40,10), 3, CV_RGB(200,0,100),3);
	CvBox2D box;
	box.angle = 20;
	box.center.x = 10;
	box.center.y = 100;
	box.size.width = 20;
	box.size.height = 50;
	cvEllipseBox(bottle, box, CV_RGB(20,20,20), -1);
	cvRectangle(bottle, cvPoint(20, 50), cvPoint(40, 90), CV_RGB(200, 250, 0), -1);
	cvRectangle(bottle, cvPoint(80, 50), cvPoint(40, 90), CV_RGB(200, 0, 0), -1);
#ifdef _DEBUG
	cvNamedWindow("bottle", 1);
	cvShowImage("bottle", bottle);
#endif

	ck = cvCreateStructuringElementEx( /*11*/17, /*11*/17, 8, 8, CV_SHAPE_ELLIPSE, NULL );
	erosion = cvCreateStructuringElementEx( /*11*/7, /*11*/7, 3, 3, CV_SHAPE_ELLIPSE, NULL );
	bigErosion = cvCreateStructuringElementEx( /*11*/9, /*11*/9, 4, 4, CV_SHAPE_ELLIPSE, NULL );
	smlErosion = cvCreateStructuringElementEx( /*11*/4, /*11*/4, 2, 2, CV_SHAPE_ELLIPSE, NULL );
	bp_img = cvCreateImage( cvSize(WIDTH,HEIGHT), 8, 1 );
	morphImage = cvCreateImage( cvSize(WIDTH,HEIGHT), 8, 1 );
	src_copy = cvCreateImage( cvSize(WIDTH,HEIGHT), 8, 3 );
	morph_half = cvCreateImage( cvSize(WIDTH/2,HEIGHT/2), 8, 1 );
	src_half = cvCreateImage( cvSize(WIDTH/2,HEIGHT/2), 8, 3 );
	temp_half = cvCreateImage( cvSize(WIDTH/2,HEIGHT/2), 8, 1 );
	sml_bkpj = cvCreateImage( cvSize(WIDTH/2,HEIGHT/2), 8, 1 );
	sml_CC_out = cvCreateImage( cvSize(WIDTH/2,HEIGHT/2), 8, 1 );
	CC_out = cvCreateImage( cvSize(WIDTH,HEIGHT), 8, 1 );
	X_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	Y_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	R_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	Theta_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	
	bp_storage = cvCreateMemStorage(0);
	pt1 = new CvPoint[4];

	delete [] Cr_ranges;
	delete [] Cb_ranges;

#ifdef FPS
	fps_counter = 0;
	fps_1 = fps_2 = fps_3 = fps_4 = fps_5 = fps_6 = 0;
#endif
}
void likelihood::setEllipse_ratio(double elp_ratio)
{
	ellipse_ratio = elp_ratio;
	SampleEllipseContour();
}

likelihood::~likelihood()
{
	cvReleaseHist( & current_hist );
	cvReleaseHist( & ref_hist );

	for (int g=0; g<MAX_TARGET_NUM; g++)
		cvReleaseImage( & ref_texture[g] );

	cvReleaseImage( & colormask );
	cvReleaseImage( &scale_img );
	cvReleaseImage( &src_copy );
	cvReleaseStructuringElement( &ck );
	cvReleaseStructuringElement( &erosion );
	cvReleaseStructuringElement( &bigErosion );
	cvReleaseStructuringElement( &smlErosion );
	cvReleaseImage( &bp_img );
	cvReleaseImage( &morphImage );
	cvReleaseImage( &src_copy );
	
	cvReleaseMat( & M_circleedgesamples );

	delete [] pt1;
}

void likelihood::UpdateRefHistogram( float x, float y, float r, IplImage* srcImage_Cr, IplImage* srcImage_Cb )
{
	// for each target
	// out: new_hist_h, new_hist_s
	// in: x, y, r, srcImage_h, srcImage_s

	CvRect in_select;

	// Convert color space
//	cvCvtColor( srcImage, srcImage_hsv, CV_BGR2HSV );
//	cvSplit( srcImage_hsv, srcImage_h, 0, 0, 0 );

	// Set ROI: a square rectangle within a circle
	in_select.x = x - r / 2;
	in_select.y = y - r / 2 * ellipse_ratio;
	in_select.width = r;
	in_select.height = r * ellipse_ratio;

	cvSetImageROI( srcImage_Cr, in_select );
	cvSetImageROI( srcImage_Cb, in_select );

	IplImage * planes[] = { srcImage_Cr, srcImage_Cb };
	cvCalcHist( planes, ref_hist, 0, NULL );

	float max_val = 0;
	cvGetMinMaxHistValue( ref_hist, 0, &max_val, 0, 0 );
	cvConvertScale( ref_hist->bins, ref_hist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????

	cvResetImageROI( srcImage_Cr );
	cvResetImageROI( srcImage_Cb );
}

void likelihood::UpdateRefHistogram( CvBox2D box, CvRect bound, IplImage * srcImage_Cr, IplImage * srcImage_Cb, CvHistogram * update_hist )
{
	cvSetImageROI( colormask, bound );
	cvSetImageROI( srcImage_Cr, bound );
	cvSetImageROI( srcImage_Cb, bound );

	cvZero( colormask );
	cvEllipseBox( colormask, box, CV_RGB(255,255,255), -1, 8, 0 );

	IplImage * planes[] = { srcImage_Cr, srcImage_Cb };
	float max_val = 0;

	// Get current histogram
	cvCalcHist( planes, update_hist, 0, colormask );
	cvGetMinMaxHistValue( update_hist, 0, &max_val, 0, 0 );
	cvConvertScale( update_hist->bins, update_hist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????
	cvNormalizeHist( update_hist, 1 );

	cvResetImageROI( colormask );
	cvResetImageROI( srcImage_Cr );
	cvResetImageROI( srcImage_Cb );
	//cout << distance << "\t";
}
double likelihood::getRotatedX(double x0, double y0, int i)
{
	if (i<0)
	{
		for (;i<circle_edge_sample_number; i+=circle_edge_sample_number)
			;
		i -= circle_edge_sample_number;
	}
	else //i>=0
	{
		for (;i>=circle_edge_sample_number; i-=circle_edge_sample_number)
			;
	}
	//int i0 = (i < 0) ? i+circle_edge_sample_number : (i >= circle_edge_sample_number ) ? i-circle_edge_sample_number : i; 
	return x0*COS[i]-y0*SIN[i];
}
double likelihood::getRotatedY(double x0, double y0, int i) 
{
	if (i<0)
	{
		for (;i<circle_edge_sample_number; i+=circle_edge_sample_number)
			;
		i -= circle_edge_sample_number;
	}
	else //i>=0
	{
		for (;i>=circle_edge_sample_number; i-=circle_edge_sample_number)
			;
	}
	//int i0 = (i < 0) ? i+circle_edge_sample_number : (i >= circle_edge_sample_number ) ? i-circle_edge_sample_number : i;
	return x0*SIN[i]+y0*COS[i];
}
double likelihood::MAPgetRotatedX(double x0, double y0, int i)
{
	if (i<0)
	{
		for (;i<circle_edge_sample_number*10; i+=circle_edge_sample_number*10)
			;
		i -= circle_edge_sample_number*10;
	}
	else //i>=0
	{
		for (;i>=circle_edge_sample_number*10; i-=circle_edge_sample_number*10)
			;
	}
	//int i0 = (i < 0) ? i+circle_edge_sample_number : (i >= circle_edge_sample_number ) ? i-circle_edge_sample_number : i; 
	return x0*MAP_COS[i]-y0*MAP_SIN[i];
}
double likelihood::MAPgetRotatedY(double x0, double y0, int i) 
{
	if (i<0)
	{
		for (;i<circle_edge_sample_number*10; i+=circle_edge_sample_number*10)
			;
		i -= circle_edge_sample_number*10;
	}
	else //i>=0
	{
		for (;i>=circle_edge_sample_number*10; i-=circle_edge_sample_number*10)
			;
	}
	//int i0 = (i < 0) ? i+circle_edge_sample_number : (i >= circle_edge_sample_number ) ? i-circle_edge_sample_number : i;
	return x0*MAP_SIN[i]+y0*MAP_COS[i];
}

double likelihood::MeasureHairColor(IplImage* scrImage_Cr, IplImage* scrImage_Cb, const double x, const double y, const double r, CvHistogram * cmp_hist, IplImage* src, IplImage* msk)
{
	cvZero(msk);
	CvBox2D hairbox;
	hairbox.center.x = x;
	hairbox.center.y = y-r*MODEL_R*ELLIPSE_RATIO;
	hairbox.angle = 0.0;
	hairbox.size.width = r*MODEL_R/2;
	hairbox.size.height = 1.8*r*MODEL_R;
	cvEllipseBox(msk, hairbox, cvScalar(255), -1);
	//cvEllipseBox(src, hairbox, CV_RGB(0,255,0), 2);
	
	IplImage * planes[] = { scrImage_Cr, scrImage_Cb };
	// Get current histogram
	cvCalcHist( planes, current_hist, 0, msk);
	float max_val = 0.0f;
	cvGetMinMaxHistValue( current_hist, 0, &max_val, 0, 0 );
//	fileOut3 << "max_val = " << max_val << endl;
	cvConvertScale( current_hist->bins, current_hist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????
	cvNormalizeHist( current_hist, 1 );
	return cvCompareHist( cmp_hist, current_hist, CV_COMP_BHATTACHARYYA );
}



//Color likelihood evaluation version 2
//This version uses rotated rectangle masking for histogram extraction
//implement with 2 CvBox2D masks (one for inner, one for outer) to get 
//internal / external histogram, and evaluate with mixed score from both
double likelihood::MeasureColor2( IplImage* scrImage_Cr, IplImage* scrImage_Cb, const double x, const double y, const double r, const double theta, CvHistogram * cmp_hist, IplImage* src, IplImage* msk )
{
	cvZero(msk);
//	double sqr_length = sqrt(double(2*/*CV_PI*/3.1415926*MODEL_R*r*MODEL_R*r*ellipse_ratio));
	CvBox2D box;
	//box.angle = /*-*/theta;
	box.angle = /*-*/(theta > 180) ? theta-180 : (theta < -180) ? theta+180 : theta;
	box.center.x = x;
	box.center.y = y;
	box.size.width = 28.0*2*r;//2*MODEL_R*r;
	box.size.height = 81.0*2*r;//box.size.width*ellipse_ratio;

//	CvPoint2D32f pt[4];
	cvBoxPoints( box,  pt );

//	CvPoint* pt1;
//	pt1 = new CvPoint[4];
	pt1[0] = cvPoint(pt[0].x,pt[0].y);
	pt1[1] = cvPoint(pt[1].x,pt[1].y);
	pt1[2] = cvPoint(pt[2].x,pt[2].y);
	pt1[3] = cvPoint(pt[3].x,pt[3].y);
	npt1 = 4;
	cvFillPoly(msk, &pt1, &npt1, 1, cvScalar(255));
//	cvFillPoly(src, &pt1, &npt1, 1, cvScalar(0,255,0));


//	cvEllipseBox(msk, box, cvScalar(255), -1);
//	cvEllipseBox(src, box, CV_RGB(0,255,0), -1);
	
	IplImage * planes[] = { scrImage_Cr, scrImage_Cb };
	// Get current histogram
	cvCalcHist( planes, current_hist, 0, msk);
	float max_val = 0.0f;
	cvGetMinMaxHistValue( current_hist, 0, &max_val, 0, 0 );
//	fileOut3 << "max_val = " << max_val << endl;
	cvConvertScale( current_hist->bins, current_hist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????
	// Normalize
	//cvNormalizeHist( ref_hist, 1 );
	cvNormalizeHist( current_hist, 1 );
//	cvGetMinMaxHistValue( current_hist, 0, &max_val, 0, 0 );
//	fileOut3 << "max_val = " << max_val << endl;
	// Note: the method CV_COMP_BHATTACHARYYA only works with normalized histograms.
	// cvNormalizeHist normalizes the histogram bins by scaling them, such that the sum of the bins becomes equal to factor.
	// double cvCompareHist( const CvHistogram* hist1, const CvHistogram* hist2, int method );
	//cvCalcBackProjectPatch
	// Compare
	double inner_distance = cvCompareHist( cmp_hist, current_hist, CV_COMP_BHATTACHARYYA );

//	cvRectangle(msk, cvPoint(x-0.5*sqr_length, y-0.5*sqr_length), cvPoint(x+0.5*sqr_length, y+0.5*sqr_length), cvScalar(255), -1);
	CvBox2D bigbox = box;
	bigbox.size.width = box.size.width*sqrt(2.0f);
	bigbox.size.height = box.size.height*sqrt(2.0f);

	cvBoxPoints( bigbox,  pt );
	pt1[0] = cvPoint(pt[0].x,pt[0].y);
	pt1[1] = cvPoint(pt[1].x,pt[1].y);
	pt1[2] = cvPoint(pt[2].x,pt[2].y);
	pt1[3] = cvPoint(pt[3].x,pt[3].y);
	cvFillPoly(msk, &pt1, &npt1, 1, cvScalar(255));
	
	cvBoxPoints( box,  pt );
	pt1[0] = cvPoint(pt[0].x,pt[0].y);
	pt1[1] = cvPoint(pt[1].x,pt[1].y);
	pt1[2] = cvPoint(pt[2].x,pt[2].y);
	pt1[3] = cvPoint(pt[3].x,pt[3].y);
	cvFillPoly(msk, &pt1, &npt1, 1, cvScalar(0));
	
//	cvEllipseBox(msk, bigbox, cvScalar(255), -1);
//	cvEllipseBox(msk, box, cvScalar(0), -1);
	cvCalcHist( planes, current_hist, 0, msk);
	max_val = 0.0f;
	cvGetMinMaxHistValue( current_hist, 0, &max_val, 0, 0 );
	cvConvertScale( current_hist->bins, current_hist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????
	// Normalize
	//cvNormalizeHist( ref_hist, 1 );
	cvNormalizeHist( current_hist, 1 );
	// Note: the method CV_COMP_BHATTACHARYYA only works with normalized histograms.
	// cvNormalizeHist normalizes the histogram bins by scaling them, such that the sum of the bins becomes equal to factor.
	// double cvCompareHist( const CvHistogram* hist1, const CvHistogram* hist2, int method );
	//cvCalcBackProjectPatch
	// Compare
	double outer_distance = cvCompareHist( cmp_hist, current_hist, CV_COMP_BHATTACHARYYA );

	//return inner_distance - outer_distance + 1;
	return inner_distance - outer_distance + 1;

}

//Color likelihood evaluation version 1
//This version uses rotated ellipse masking for histogram extraction
//implement with 2 ellipse masks (one for inner, one for outer) to get 
//internal / external histogram, and evaluate with mixed score from both
double likelihood::MeasureColor1( IplImage* scrImage_Cr, IplImage* scrImage_Cb, const double x, const double y, const double r, const double theta, CvHistogram * cmp_hist, IplImage* src, IplImage* msk )
{
	cvZero(msk);
	double sqr_length = sqrt(double(2*/*CV_PI*/3.1415926*MODEL_R*r*MODEL_R*r*ellipse_ratio));
	CvBox2D box;
	//box.angle = /*-*/theta;
	box.angle = /*-*/(theta > 180) ? theta-180 : (theta < -180) ? theta+180 : theta;
	box.center.x = x;
	box.center.y = y;
	box.size.width = 2*MODEL_R*r;
	box.size.height = box.size.width*ellipse_ratio;
	cvEllipseBox(msk, box, cvScalar(255), -1);
//	cvEllipseBox(src, box, CV_RGB(0,255,0), -1);
	
	IplImage * planes[] = { scrImage_Cr, scrImage_Cb };
	// Get current histogram
	cvCalcHist( planes, current_hist, 0, msk);
	float max_val = 0.0f;
	cvGetMinMaxHistValue( current_hist, 0, &max_val, 0, 0 );
//	fileOut3 << "max_val = " << max_val << endl;
	cvConvertScale( current_hist->bins, current_hist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????
	// Normalize
	//cvNormalizeHist( ref_hist, 1 );
	cvNormalizeHist( current_hist, 1 );
//	cvGetMinMaxHistValue( current_hist, 0, &max_val, 0, 0 );
//	fileOut3 << "max_val = " << max_val << endl;
	// Note: the method CV_COMP_BHATTACHARYYA only works with normalized histograms.
	// cvNormalizeHist normalizes the histogram bins by scaling them, such that the sum of the bins becomes equal to factor.
	// double cvCompareHist( const CvHistogram* hist1, const CvHistogram* hist2, int method );
	//cvCalcBackProjectPatch
	// Compare
	double inner_distance = cvCompareHist( cmp_hist, current_hist, CV_COMP_BHATTACHARYYA );

//	cvRectangle(msk, cvPoint(x-0.5*sqr_length, y-0.5*sqr_length), cvPoint(x+0.5*sqr_length, y+0.5*sqr_length), cvScalar(255), -1);
	CvBox2D bigbox = box;
	bigbox.size.width = box.size.width*sqrt(2.0f);
	bigbox.size.height = box.size.height*sqrt(2.0f);
	cvEllipseBox(msk, bigbox, cvScalar(255), -1);
	cvEllipseBox(msk, box, cvScalar(0), -1);
	cvCalcHist( planes, current_hist, 0, msk);
	max_val = 0.0f;
	cvGetMinMaxHistValue( current_hist, 0, &max_val, 0, 0 );
	cvConvertScale( current_hist->bins, current_hist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????
	// Normalize
	//cvNormalizeHist( ref_hist, 1 );
	cvNormalizeHist( current_hist, 1 );
	// Note: the method CV_COMP_BHATTACHARYYA only works with normalized histograms.
	// cvNormalizeHist normalizes the histogram bins by scaling them, such that the sum of the bins becomes equal to factor.
	// double cvCompareHist( const CvHistogram* hist1, const CvHistogram* hist2, int method );
	//cvCalcBackProjectPatch
	// Compare
	double outer_distance = cvCompareHist( cmp_hist, current_hist, CV_COMP_BHATTACHARYYA );

	//return inner_distance - outer_distance + 1;
	return 2.0*inner_distance - outer_distance + 1;

}

//Color likelihood evaluation version 0
//This version uses tightly non-rotated bounding box
//implement with sin cos projection to get box size
double likelihood::MeasureColor( IplImage * srcImage_Cr, IplImage* srcImage_Cb, double x, double y, double r, double theta, CvHistogram * cmp_hist, IplImage* src )
{
	// for each particle, the most similar one has the smallest BHATTACHARYYA distance
	// out: distance
	// in: ref_hist_h, ref_hist_s, x, y, r, scrImage_h, scrImage_s

	double distance;
	int X_min, X_max, Y_min, Y_max;
	double d_x = max(r*ellipse_ratio*cos(theta*CV_PI/180), (r*sin(theta*CV_PI/180) >= 0 ? r*sin(theta*CV_PI/180) : -r*sin(theta*CV_PI/180)));
	double d_y = max((r*ellipse_ratio*sin(theta*CV_PI/180) >= 0 ? r*ellipse_ratio*sin(theta*CV_PI/180) : -r*ellipse_ratio*sin(theta*CV_PI/180)), 
		r*cos(theta*CV_PI/180));

	X_min = cvRound(x - d_x);
	X_max = cvRound(x + d_x);
	Y_min = cvRound(y - d_y);
	Y_max = cvRound(y + d_y);

	cvRectangle(src, cvPoint(X_min, Y_min), cvPoint(X_max, Y_max), CV_RGB(255,255,255));
	distance = HomogenousRegion( X_min, X_max, Y_min, Y_max, cmp_hist, srcImage_Cr, srcImage_Cb );

	return distance;
}

double likelihood::MeasureBoxColor( IplImage * srcImage_Cr, IplImage * srcImage_Cb, CvBox2D box, CvHistogram * cmp_hist, CvRect rect )
{
#ifdef FPS
	int frame_count = 1800;
	fps_counter ++;
	temp1 = clock();
#endif

	double distance = 0;
	CvBox2D maskbox;
	maskbox = box;
	maskbox.center = cvPoint2D32f( box.center.x-rect.x, box.center.y-rect.y );

#ifdef FPS
	temp2 = clock();
	fps_1 += (temp2-temp1);
#endif
	cvSetImageROI( colormask, rect );
	cvZero( colormask );
	cvEllipseBox( colormask, maskbox, CV_RGB(255,255,255), -1, 8, 0 );
	
#ifdef FPS
	temp1 = clock();
	fps_2 += (temp1-temp2);
#endif

	cvSetImageROI( srcImage_Cr, rect );
	cvSetImageROI( srcImage_Cb, rect );
#ifdef FPS
	temp2 = clock();
	fps_3 += (temp2-temp1);
#endif

	IplImage * planes[] = { srcImage_Cr, srcImage_Cb };
	float max_val = 0;

#ifdef FPS
	temp1 = clock();
	fps_4 += (temp1-temp2);
#endif
	// Get current histogram
	cvCalcHist( planes, current_hist, 0, colormask );
	cvGetMinMaxHistValue( current_hist, 0, &max_val, 0, 0 );
	cvConvertScale( current_hist->bins, current_hist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????
	cvNormalizeHist( current_hist, 1 );

#ifdef FPS
	temp2 = clock();
	fps_5 += (temp2-temp1);
#endif

	distance = cvCompareHist( cmp_hist, current_hist, CV_COMP_BHATTACHARYYA );
#ifdef FPS
	temp1 = clock();
	fps_6 += (temp1-temp2);
#endif

	cvResetImageROI( srcImage_Cr );
	cvResetImageROI( srcImage_Cb );
	cvResetImageROI( colormask );
	//cout << distance << "\t";

#ifdef FPS
	if( fps_counter == frame_count ) {
		cout << setprecision(5);
		cout << endl << "MEASURE COLOW" << endl;
		cout << "\t1\t" << setw(10) << fps_1/frame_count*60 << "\t2\t" << setw(10) << fps_2/frame_count*60 << endl;
		cout << "\t3\t" << setw(10) << fps_3/frame_count*60 << "\t4\t" << setw(10) << fps_4/frame_count*60 << endl;
		cout << "\t5\t" << setw(10) << fps_5/frame_count*60 << "\t6\t" << setw(10) << fps_6/frame_count*60 << endl;
		fps_counter = 0;
		fps_1 = fps_2 = fps_3 = fps_4 = fps_5 = fps_6 = 0;
	}
#endif

	return distance;
}

double likelihood::MeasureRectColor( IplImage * srcImage_Cr, IplImage * srcImage_Cb, int x, int y, int width, CvHistogram const * cmp_hist )
{
	//cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
	//IplImage * test;
	//test = cvCreateImage( cvGetSize(srcImage_Cr), 8, 1 );
	//cvCopy( srcImage_Cr, test );

	double distance = 0;

	CvRect rect;
	rect.x = x-width/2 < 0 ? 0 : x-width/2;
	rect.y = y-width/2 < 0 ? 0 : y-width/2;
	//cout << rect.x << "\t" << rect.y << "\t";
	if( rect.x > srcImage_Cr->width || rect.y > srcImage_Cr->height )
		return 1;

	rect.width = x+width/2 > srcImage_Cr->width ? srcImage_Cr->width-rect.x : width;
	rect.height = y+width/2 > srcImage_Cr->height ? srcImage_Cr->height-rect.y : width;
	//cout << rect.width << "\t" << rect.height << endl;
	if( rect.width == 0 || rect.height == 0 )
		return 1;

	cvSetImageROI( srcImage_Cr, rect );
	cvSetImageROI( srcImage_Cb, rect );

	IplImage * planes[] = { srcImage_Cr, srcImage_Cb };
	float max_val = 0;

	// Get current histogram
	cvCalcHist( planes, current_hist, 0, 0 );
	cvGetMinMaxHistValue( current_hist, 0, &max_val, 0, 0 );
	cvConvertScale( current_hist->bins, current_hist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????
	cvNormalizeHist( current_hist, 1 );

	distance = cvCompareHist( cmp_hist, current_hist, CV_COMP_BHATTACHARYYA );

	cvResetImageROI( srcImage_Cr );
	cvResetImageROI( srcImage_Cb );
	//cout << distance << "\t";

	//cvRectangle( test, cvPoint(rect.x,rect.y), cvPoint(rect.x+rect.width,rect.y+rect.height), CV_RGB(255,255,255), 1, 8, 0 );
	//cvShowImage( "test", test );
	//cvReleaseImage( & test );

	return distance;
}


//Contour matching likelihood version 2
//this version models the target as CvBox2D which
//is defined by bottle.bmp.
//we sample 40 edge points on that CvBox2D to search for strongest line
//using quantized vector rotation / scaling to measure the total matching distance at any size and (quantized) angle
double likelihood::MeasureEllipseEdge2( const double x, const double y, const double r, const double theta, IplImage *edgeImage, IplImage* src )
{
	CvBox2D box;
	//box.angle = /*-*/theta;
	box.angle = /*-*/(theta > 180) ? theta-180 : (theta < -180) ? theta+180 : theta;
	box.center.x = x;
	box.center.y = y;
	box.size.width = 28.0*2*r;//2*MODEL_R*r;
	box.size.height = 81.0*2*r;//box.size.width*ellipse_ratio;

	cvBoxPoints( box,  pt );
	pt1[0] = cvPoint(pt[0].x,pt[0].y);
	pt1[1] = cvPoint(pt[1].x,pt[1].y);
	pt1[2] = cvPoint(pt[2].x,pt[2].y);
	pt1[3] = cvPoint(pt[3].x,pt[3].y);

	//construct unit vector
	int i_index = cvRound( theta/360*circle_edge_sample_number );
	//head angle vector
	double dx1 = getRotatedX(1.0, 0, i_index);
	double dy1 = getRotatedY(1.0, 0, i_index);
	//left side outward vector
	double dx2 = getRotatedX(1.0, 0, i_index+circle_edge_sample_number/4);
	double dy2 = getRotatedY(1.0, 0, i_index+circle_edge_sample_number/4);

	int normal_sample_number = 0;
	if (r > 0.7) normal_sample_number = 2;
	else if (r<0.7 && r>0.5) normal_sample_number = 2;
	else normal_sample_number = 2;


	double bdptX, bdptY;
	int searchX, searchY;
	unsigned char edge_val;
	double max_val;
	double dist;
	double nor_dist;
	double total_dist = 0;
	double total_point = 0;
	double avg_edge_value = 0;
	double matching_cnt = 0;
	double worst_value = normal_sample_number;




	for (double i=1; i<=circle_edge_sample_number/4; i++)
	{
		bdptX = ((double)pt1[0].x*i + (double)pt1[1].x*((double)circle_edge_sample_number/4 + 1.0-i))/((double)circle_edge_sample_number/4 + 1.0);
		bdptY = ((double)pt1[0].y*i + (double)pt1[1].y*((double)circle_edge_sample_number/4 + 1.0-i))/((double)circle_edge_sample_number/4 + 1.0);
		if (( bdptX >=0 ) && ( bdptX < WIDTH ) && ( bdptY >=0 ) && ( bdptY < HEIGHT ))
		{
			max_val = 0;
			for (double j=-normal_sample_number; j<=normal_sample_number; j++)
			{
				searchX = bdptX + (dx2)*j;
				searchY = bdptY - (dy2)*j;
				if (( searchX >=0 ) && ( searchX < WIDTH ) && ( searchY >=0 ) && ( searchY < HEIGHT ))
				{
					edge_val = CV_IMAGE_ELEM( edgeImage, unsigned char, searchY, searchX );
					//cvCircle(src, cvPoint(searchX, searchY), 1, CV_RGB(0,255,0), -1);
					if ( ((double)edge_val)*exp(-((double)abs(j))*0.3) > (double)max_val )
					{
						dist = abs(j);
						max_val = ((double)edge_val)*exp(-dist*0.3);
						
						dist_arr[(int)i-1] = j;//含正負號

					}
				}

			}
			// no find edge, give penalty value
			if ( max_val == 0 )
			{
				dist_arr[(int)i-1] = dist = worst_value;

			}
			else
				matching_cnt ++;


			avg_edge_value += max_val;

			total_dist = total_dist + dist;
			total_point ++;

		}
		//.............................................................
		bdptX = ((double)pt1[2].x*i + (double)pt1[1].x*((double)circle_edge_sample_number/4 + 1.0-i))/((double)circle_edge_sample_number/4 + 1.0);
		bdptY = ((double)pt1[2].y*i + (double)pt1[1].y*((double)circle_edge_sample_number/4 + 1.0-i))/((double)circle_edge_sample_number/4 + 1.0);
		if (( bdptX >=0 ) && ( bdptX < WIDTH ) && ( bdptY >=0 ) && ( bdptY < HEIGHT ))
		{
			max_val = 0;
			for (double j=-normal_sample_number; j<=normal_sample_number; j++)
			{
				searchX = bdptX + (dx1)*j;
				searchY = bdptY - (dy1)*j;
				if (( searchX >=0 ) && ( searchX < WIDTH ) && ( searchY >=0 ) && ( searchY < HEIGHT ))
				{
					edge_val = CV_IMAGE_ELEM( edgeImage, unsigned char, searchY, searchX );
					//cvCircle(src, cvPoint(searchX, searchY), 1, CV_RGB(0,255,0), -1);
					if ( ((double)edge_val)*exp(-((double)abs(j))*0.3) > (double)max_val )
					{
						dist = abs(j);
						max_val = ((double)edge_val)*exp(-dist*0.3);
						
						dist_arr[(int)i+circle_edge_sample_number/4-1] = j;//含正負號

					}
				}

			}
			// no find edge, give penalty value
			if ( max_val == 0 )
			{
				dist_arr[(int)i+circle_edge_sample_number/4-1] = dist = worst_value;

			}
			else
				matching_cnt ++;


			avg_edge_value += max_val;

			total_dist = total_dist + dist;
			total_point ++;

		}
		//.......................................................
		bdptX = ((double)pt1[3].x*i + (double)pt1[2].x*((double)circle_edge_sample_number/4 + 1.0-i))/((double)circle_edge_sample_number/4 + 1.0);
		bdptY = ((double)pt1[3].y*i + (double)pt1[2].y*((double)circle_edge_sample_number/4 + 1.0-i))/((double)circle_edge_sample_number/4 + 1.0);
		if (( bdptX >=0 ) && ( bdptX < WIDTH ) && ( bdptY >=0 ) && ( bdptY < HEIGHT ))
		{
			max_val = 0;
			for (double j=-normal_sample_number; j<=normal_sample_number; j++)
			{
				searchX = bdptX - (dx2)*j;
				searchY = bdptY + (dy2)*j;
				if (( searchX >=0 ) && ( searchX < WIDTH ) && ( searchY >=0 ) && ( searchY < HEIGHT ))
				{
					edge_val = CV_IMAGE_ELEM( edgeImage, unsigned char, searchY, searchX );
					//cvCircle(src, cvPoint(searchX, searchY), 1, CV_RGB(0,255,0), -1);
					if ( ((double)edge_val)*exp(-((double)abs(j))*0.3) > (double)max_val )
					{
						dist = abs(j);
						max_val = ((double)edge_val)*exp(-dist*0.3);
						
						dist_arr[(int)i+circle_edge_sample_number*2/4-1] = j;//含正負號

					}
				}

			}
			// no find edge, give penalty value
			if ( max_val == 0 )
			{
				dist_arr[(int)i+circle_edge_sample_number*2/4-1] = dist = worst_value;

			}
			else
				matching_cnt ++;


			avg_edge_value += max_val;

			total_dist = total_dist + dist;
			total_point ++;

		}
		//.........................................................................
		bdptX = ((double)pt1[0].x*i + (double)pt1[3].x*((double)circle_edge_sample_number/4 + 1.0-i))/((double)circle_edge_sample_number/4 + 1.0);
		bdptY = ((double)pt1[0].y*i + (double)pt1[3].y*((double)circle_edge_sample_number/4 + 1.0-i))/((double)circle_edge_sample_number/4 + 1.0);
		if (( bdptX >=0 ) && ( bdptX < WIDTH ) && ( bdptY >=0 ) && ( bdptY < HEIGHT ))
		{
			max_val = 0;
			for (double j=-normal_sample_number; j<=normal_sample_number; j++)
			{
				searchX = bdptX - (dx1)*j;
				searchY = bdptY + (dy1)*j;
				if (( searchX >=0 ) && ( searchX < WIDTH ) && ( searchY >=0 ) && ( searchY < HEIGHT ))
				{
					edge_val = CV_IMAGE_ELEM( edgeImage, unsigned char, searchY, searchX );
					//cvCircle(src, cvPoint(searchX, searchY), 1, CV_RGB(0,255,0), -1);
					if ( ((double)edge_val)*exp(-((double)abs(j))*0.3) > (double)max_val )
					{
						dist = abs(j);
						max_val = ((double)edge_val)*exp(-dist*0.3);
						
						dist_arr[(int)i+circle_edge_sample_number*3/4-1] = j;//含正負號

					}
				}

			}
			// no find edge, give penalty value
			if ( max_val == 0 )
			{
				dist_arr[(int)i+circle_edge_sample_number*3/4-1] = dist = worst_value;

			}
			else
				matching_cnt ++;


			avg_edge_value += max_val;

			total_dist = total_dist + dist;
			total_point ++;

		}

		

	}
	//dist_arr is constructed
	double dist_sum = 0;
	for (int k=0; k<circle_edge_sample_number/4; k++)
	{
		dist_sum += abs(dist_arr[k]-dist_arr[int(circle_edge_sample_number/2 + k)]);
		dist_sum += abs(dist_arr[circle_edge_sample_number/4 + k]-dist_arr[int(circle_edge_sample_number*3/4 + k)]);
	}
	double sym_val = (dist_sum/total_point)*2.0/worst_value;

	//avg_edge_value can be evaluated
	avg_edge_value = avg_edge_value/total_point/255;
	

	//cout << "total dist " << total_dist << "\ttotal point " << total_point << endl;
	if ( total_point > 0 )		// distance = [0,1]
		nor_dist = total_dist / total_point / worst_value;
	else	// all samples are out of image
		nor_dist = 1;

	return  (nor_dist + sym_val*/*0.4*/0.6 /*+ (1-total_point/circle_edge_sample_number)*/ 
		+ (1 - avg_edge_value) + 0.8*(1-matching_cnt/circle_edge_sample_number) + 0.2/*0.2*/*(1-r/(Max_r/MODEL_R)))/3;



}

//Contour matching likelihood version 1
//this version models the target as arbitrary 40 polygon which
//is defined by bottle.bmp.
//we sample 40 edge points on that bottle as our model vector
//using vector shifting scaling to measure the total matching distance at any size and (quantized) angle
double likelihood::MeasureEllipseEdge1( const double x, const double y, const double r, const double theta, IplImage *edgeImage, IplImage* src )
{
	// for each particle, the most similar one has the smallest edge distance
	// out: distance
	// in: x, y, r, edgeImage

	int x_index = cvRound( x );
	int y_index = cvRound( y );
	int r_index = cvRound( r );
	int i_index = cvRound( theta/360*circle_edge_sample_number );

	int i, j, dx, dy;
	double dist;
	double nor_dist;
	unsigned char edge_val;
	double max_val;
	double total_dist = 0;
	double total_point = 0;
	double x0, y0;
	double avg_edge_value = 0;
	double matching_cnt = 0;

	double worst_value = circle_normal_sample_number/2 + 1;

	//CvFont testfont;
	//cvInitFont( &testfont, CV_FONT_VECTOR0, 1, 1, 0, 1 );

	for ( i = 0; i < circle_edge_sample_number; i++ ) {
		// sample point on the circle
		/*x0 = cvmGet( M_circleedgesamples, r_index * circle_edge_sample_number + i, 0);
		y0 = cvmGet( M_circleedgesamples, r_index * circle_edge_sample_number + i, 1);
		dx = x_index + getRotatedX(x0, y0, i_index-10);
		dy = y_index + getRotatedY(x0, y0, i_index-10);
		*/
		dx = x_index + getRotatedX(R[i]*r, 0, i+i_index-10);
		dy = y_index - getRotatedY(R[i]*r, 0, i+i_index-10);
		//centerX + getRotatedX(R[i]*0.3, 0, i+bias),centerY /*+*/- getRotatedY(R[i]*0.3, 0, i+bias)
		if ( ( dx >=0 ) && ( dx < WIDTH ) && ( dy >=0 ) && ( dy < HEIGHT ) ) {	// check within image
			// find strongest edge point along the normal vector of each sample point
//			cvCircle(src, cvPoint(dx,dy), 2, CV_RGB(255,255,255), -1);
			max_val = 0;
			for ( j = 0; j <= circle_normal_sample_number * 2; j++ ) {
				/*x0 = cvmGet( M_circleedgesamples, (r_index - circle_normal_sample_number + j) * circle_edge_sample_number + i, 0);
				y0 = cvmGet( M_circleedgesamples, (r_index - circle_normal_sample_number + j) * circle_edge_sample_number + i, 1);

				dx = x_index + getRotatedX(x0, y0, i_index-10);
				dy = y_index + getRotatedY(x0, y0, i_index-10);*/
				dx = x_index + getRotatedX(R[i]*r-circle_normal_sample_number + j, 0, i+i_index-10);
				dy = y_index - getRotatedY(R[i]*r-circle_normal_sample_number + j, 0, i+i_index-10);

				if ( ( dx >=0 ) && ( dx < WIDTH ) && ( dy >=0 ) && ( dy < HEIGHT ) ) {	// check within image
					edge_val = CV_IMAGE_ELEM( edgeImage, unsigned char, dy, dx );
					if ( ((double)edge_val)*exp(-((double)abs(j - circle_normal_sample_number))*0.3) > (double)max_val ) {
						/*max_val = edge_val;*/
						/*dist_arr[i] = */dist = abs(j - circle_normal_sample_number);
						max_val = ((double)edge_val)*exp(-dist*0.3);
						
						dist_arr[i] = j - circle_normal_sample_number;//含正負號

						//cvPutText( edgeImage, ".", cvPoint( dx, dy ), &testfont, CV_RGB(255, 255, 255) );
					}
				}
			}

			// no find edge, give penalty value
			if ( max_val == 0 )
			{
				dist_arr[i] = dist = worst_value;

			}
			else
				matching_cnt ++;


			avg_edge_value += max_val;

			total_dist = total_dist + dist;
			total_point ++;
		}
	}

	//dist_arr is constructed
	double dist_sum = 0;
	for (int k=0; k<circle_edge_sample_number/8; k++)
	{
		//if ()
		dist_sum += abs(dist_arr[k]-dist_arr[int(/*k+*/0.5*circle_edge_sample_number - k)]);
		dist_sum += abs(dist_arr[circle_edge_sample_number-1-k]-dist_arr[int(0.5*circle_edge_sample_number/*-1-k*/ + 1 + k)]);
	}
	double sym_val = (dist_sum/total_point)*4/worst_value;

	//avg_edge_value can be evaluated
	avg_edge_value = avg_edge_value/total_point/255;
	

	//cout << "total dist " << total_dist << "\ttotal point " << total_point << endl;
	if ( total_point > 0 )		// distance = [0,1]
		nor_dist = total_dist / total_point / worst_value;
	else	// all samples are out of image
		nor_dist = 1;

	return  (nor_dist + sym_val*0.4 /*+ (1-total_point/circle_edge_sample_number)*/ 
		+ (1 - avg_edge_value) + 1.2*(1-matching_cnt/circle_edge_sample_number) + 0.4/*0.2*/*(1-r/(Max_r/MODEL_R)))/3;
}

//Backproject drawing particles version 2
//this function draw particles from color-backprojected connected components
//Here we will only use large scale images.
double likelihood::Backproj2(IplImage* scrImage_Cr, IplImage* scrImage_Cb, CvMat* add_X, CvMat* add_Y, CvMat* add_R, CvMat* add_Theta, IplImage* src, CvHistogram /*const*/ * cmp_hist, bool & detected, CvPoint prev_loc)
{
	double weight_ratio = -1;
	cvResetImageROI(scrImage_Cr);
	cvResetImageROI(scrImage_Cb);
	cvResetImageROI(src);
	cvResetImageROI(morphImage);

	cvZero( bp_img );
	cvZero( morphImage );

	IplImage * planes[] = {scrImage_Cr, scrImage_Cb};
	
	time_t time1, time2;
	time(&time1);
//	for (int g=0; g<1000; g++)
		cvCalcBackProject( planes, morphImage, cmp_hist );
	time(&time2);


//	fileOut3 << "time: backproj " << (double)difftime(time2, time1) << endl;
	//check sum
	double	sum = 0;


	cvCopy(src, src_copy, 0);
	CvBox2D Ellipse, temp;

	CvRNG rng = cvRNG( rand() );

	
	// Add a black boundary to avoid the blob adjecent to boundary will not be detected !!!
	cvRectangle( morphImage, cvPoint(0,0), cvPoint(WIDTH,HEIGHT), CV_RGB(0, 0, 0), 10, 8, 0 );

	cvErode(morphImage, morphImage, smlErosion, 1);
	cvDilate(morphImage, morphImage, bigErosion, 1);

	
	CvMat * X_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	CvMat * Y_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	CvMat * R_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	CvMat * Theta_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );

	cvGetCols( add_X, X_rand, 0, ADD_PARTICLE_NUM );
	cvGetCols( add_Y, Y_rand, 0, ADD_PARTICLE_NUM );
	cvGetCols( add_R, R_rand, 0, ADD_PARTICLE_NUM );
	cvGetCols( add_Theta, Theta_rand, 0, ADD_PARTICLE_NUM );

	detected = ConnectedComponent2(morphImage, X_rand, Y_rand, R_rand, Theta_rand);
		
	return weight_ratio;//sum/(cvGetSize(src).width*cvGetSize(src).height*255.0f);

}

//Backproject drawing particles version 1
//this function draw particles from color-backprojected connected components
//Mostly, we do image processing with small scale images for sppeding up
double likelihood::Backproj1(IplImage* scrImage_Cr, IplImage* scrImage_Cb, CvMat* add_X, CvMat* add_Y, CvMat* add_R, CvMat* add_Theta, IplImage* src, CvHistogram /*const*/ * cmp_hist, bool & detected, CvPoint prev_loc)
{
	double weight_ratio = -1;
	cvResetImageROI(scrImage_Cr);
	cvResetImageROI(scrImage_Cb);
	cvResetImageROI(src);
	cvResetImageROI(morphImage);

	cvZero( bp_img );
	cvZero( morphImage );

	IplImage * planes[] = {scrImage_Cr, scrImage_Cb};
	
	time_t time1, time2;
	time(&time1);
//	for (int g=0; g<1000; g++)
		cvCalcBackProject( planes, morphImage, cmp_hist );
	time(&time2);
	cvThreshold(morphImage, morphImage, 30, 255, CV_THRESH_TOZERO);
#ifdef _DEBUG
	cvNamedWindow("morphImage_raw");
	cvShowImage("morphImage_raw", morphImage);
#endif
//	fileOut3 << "time: backproj " << (double)difftime(time2, time1) << endl;
	//check sum
	double	sum = 0;


	cvCopy(src, src_copy, 0);
	CvBox2D Ellipse, temp;

	CvRNG rng = cvRNG( rand() );

	
	// Add a black boundary to avoid the blob adjecent to boundary will not be detected !!!
	cvRectangle( morphImage, cvPoint(0,0), cvPoint(WIDTH,HEIGHT), CV_RGB(0, 0, 0), 10, 8, 0 );

	//downsampling to half of the original size
	cvResize(morphImage, temp_half, CV_INTER_AREA);
	cvResize(morphImage, morph_half, CV_INTER_LINEAR);
	cvErode(morph_half, morph_half, smlErosion, 1);
	cvDilate(morph_half, morph_half, /*big*/ck, 1);

	
	//CvMat * X_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	//CvMat * Y_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	//CvMat * R_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );
	//CvMat * Theta_rand = cvCreateMat( 1, ADD_PARTICLE_NUM, CV_32FC1 );

	cvGetCols( add_X, X_rand, 0, ADD_PARTICLE_NUM );
	cvGetCols( add_Y, Y_rand, 0, ADD_PARTICLE_NUM );
	cvGetCols( add_R, R_rand, 0, ADD_PARTICLE_NUM );
	cvGetCols( add_Theta, Theta_rand, 0, ADD_PARTICLE_NUM );


		detected = ConnectedComponent1(morph_half, X_rand, Y_rand, R_rand, Theta_rand);
		



		return weight_ratio;//sum/(cvGetSize(src).width*cvGetSize(src).height*255.0f);



}

//Contour matching likelihood version 4
//Almost the same as version 0, only add edge segment details
//we use "copy by reference" to obtain 4 edge scores

//this version models the target as ellipse shape which
//is defined by the ellipse_ratio.
//we sample 40 control points to get the total matching distance
double likelihood::MeasureEllipseEdge4( double x, double y, double r, double theta, IplImage *edgeImage, IplImage* src, 
									  double & upperright, double & upperleft, double & downright, double & downleft, int cam_id)
{
	// for each particle, the most similar one has the smallest edge distance
	// out: distance
	// in: x, y, r, edgeImage

	//reset 4 edge scores
	upperright = upperleft = downright = downleft = 0;
	int x_index = cvRound( x );
	int y_index = cvRound( y );
	int r_index = cvRound( MODEL_R*r );
	int i_index = cvRound( theta/360*circle_edge_sample_number );

	int i, j, dx, dy;
	double dist;
	double nor_dist;
	unsigned char edge_val;
	double max_val;
	double total_dist = 0;
	double total_point = 0;
	double x0, y0;
	double avg_edge_value = 0;
	double matching_cnt = 0;

	double worst_value = circle_normal_sample_number/2 + 1;

	//CvFont testfont;
	//cvInitFont( &testfont, CV_FONT_VECTOR0, 1, 1, 0, 1 );

	for ( i = 0; i < circle_edge_sample_number; i++ ) {
		// sample point on the circle
		x0 = cvmGet( M_circleedgesamples, r_index * circle_edge_sample_number + i, 0);
		y0 = cvmGet( M_circleedgesamples, r_index * circle_edge_sample_number + i, 1);
		dx = x_index + getRotatedX(x0, y0, i_index-10);
		dy = y_index /*+*/- getRotatedY(x0, y0, i_index-10);
		if ( ( dx >=0 ) && ( dx < WIDTH ) && ( dy >=0 ) && ( dy < HEIGHT ) ) {	// check within image
			// find strongest edge point along the normal vector of each sample point
			//cvCircle(src, cvPoint(dx,dy), 2, CV_RGB(255,255,255), -1);
			max_val = 0;
			for ( j = 0; j <= circle_normal_sample_number * 2; j++ ) {
				x0 = cvmGet( M_circleedgesamples, (r_index - circle_normal_sample_number + j) * circle_edge_sample_number + i, 0);
				y0 = cvmGet( M_circleedgesamples, (r_index - circle_normal_sample_number + j) * circle_edge_sample_number + i, 1);

				dx = x_index + getRotatedX(x0, y0, i_index-10);
				dy = y_index /*+*/- getRotatedY(x0, y0, i_index-10);

				if ( ( dx >=0 ) && ( dx < WIDTH ) && ( dy >=0 ) && ( dy < HEIGHT ) ) {	// check within image
					edge_val = CV_IMAGE_ELEM( edgeImage, unsigned char, dy, dx );
					if ( ((double)edge_val)*exp(-((double)abs(j - circle_normal_sample_number))*0.3) > (double)max_val ) 
					{
						//cvPutText( edgeImage, ".", cvPoint( dx, dy ), &testfont, CV_RGB(255, 255, 255) );
						
						/*dist_arr[i] = */dist = abs(j - circle_normal_sample_number);
						max_val = ((double)edge_val)*exp(-dist*0.3);
						
						dist_arr[i] = j - circle_normal_sample_number;//含正負號
					}
				}
			}
			
			if (i < circle_edge_sample_number/4) upperright += max_val;
			else if (i < circle_edge_sample_number*2/4) upperleft += max_val;
			else if (i < circle_edge_sample_number*3/4) downleft += max_val;
			else if (i < circle_edge_sample_number) downright += max_val;
			else ;

			// no find edge, give penalty value
			if ( max_val == 0 )
			{
				dist_arr[i] = dist = worst_value;
			}
			else
				matching_cnt ++;

			avg_edge_value += max_val;

			total_dist = total_dist + dist;
			total_point ++;
		}
	}
	//dist_arr is constructed
	double dist_sum = 0;
	for (int k=0; k<circle_edge_sample_number/8; k++)
	{
		//if ()
		dist_sum += abs(dist_arr[k]-dist_arr[int(/*k+*/0.5*circle_edge_sample_number - k)]);
		dist_sum += abs(dist_arr[circle_edge_sample_number-1-k]-dist_arr[int(0.5*circle_edge_sample_number/*-1-k*/ + 1 + k)]);
	}
	double sym_val = (dist_sum/total_point)*4/worst_value;

	//avg_edge_value can be evaluated
	avg_edge_value = avg_edge_value/total_point/255;
	

	//cout << "total dist " << total_dist << "\ttotal point " << total_point << endl;
	if ( total_point > 0 )		// distance = [0,1]
		nor_dist = total_dist / total_point / worst_value;
	else	// all samples are out of image
		nor_dist = 1;

	if (cam_id == CAM1)
	{
		fileOut3 << "edge CAM1 = " << cam_id << endl;
		return  (nor_dist + sym_val*0.4 /*+ (1-total_point/circle_edge_sample_number)*/ 
			+ (1 - avg_edge_value) + 1.2*(1-matching_cnt/circle_edge_sample_number) + 0.4/*0.2*/*(1-r/(Max_r/MODEL_R)))/3;
	}
	else //cam_id == CAM2
//This is for CAM2 setting
	{
		fileOut3 << "edge CAM2 = " << cam_id << endl;
		return  (nor_dist + sym_val*0.4 /*+ (1-total_point/circle_edge_sample_number)*/ 
			+ (1 - avg_edge_value) + 1.2*(1-matching_cnt/circle_edge_sample_number) - 0.8/*0.2*/*(1-r/(Max_r/MODEL_R)))/2.5;
	}
}

//Contour matching likelihood version 0
//this version models the target as ellipse shape which
//is defined by the ellipse_ratio.
//we sample 40 control points to get the total matching distance
double likelihood::MeasureEllipseEdge( double x, double y, double r, double theta, IplImage *edgeImage, IplImage* src)
{
	// for each particle, the most similar one has the smallest edge distance
	// out: distance
	// in: x, y, r, edgeImage

	int x_index = cvRound( x );
	int y_index = cvRound( y );
	int r_index = cvRound( MODEL_R*r );
	int i_index = cvRound( theta/360*circle_edge_sample_number );

	int i, j, dx, dy;
	double dist;
	double nor_dist;
	unsigned char edge_val;
	double max_val;
	double total_dist = 0;
	double total_point = 0;
	double x0, y0;
	double avg_edge_value = 0;
	double matching_cnt = 0;

	double worst_value = circle_normal_sample_number/2 + 1;

	//CvFont testfont;
	//cvInitFont( &testfont, CV_FONT_VECTOR0, 1, 1, 0, 1 );

	for ( i = 0; i < circle_edge_sample_number; i++ ) {
		// sample point on the circle
		x0 = cvmGet( M_circleedgesamples, r_index * circle_edge_sample_number + i, 0);
		y0 = cvmGet( M_circleedgesamples, r_index * circle_edge_sample_number + i, 1);
		dx = x_index + getRotatedX(x0, y0, i_index-10);
		dy = y_index /*+*/- getRotatedY(x0, y0, i_index-10);
		if ( ( dx >=0 ) && ( dx < WIDTH ) && ( dy >=0 ) && ( dy < HEIGHT ) ) {	// check within image
			// find strongest edge point along the normal vector of each sample point
			//cvCircle(src, cvPoint(dx,dy), 2, CV_RGB(255,255,255), -1);
			max_val = 0;
			for ( j = 0; j <= circle_normal_sample_number * 2; j++ ) {
				x0 = cvmGet( M_circleedgesamples, (r_index - circle_normal_sample_number + j) * circle_edge_sample_number + i, 0);
				y0 = cvmGet( M_circleedgesamples, (r_index - circle_normal_sample_number + j) * circle_edge_sample_number + i, 1);

				dx = x_index + getRotatedX(x0, y0, i_index-10);
				dy = y_index /*+*/- getRotatedY(x0, y0, i_index-10);

				if ( ( dx >=0 ) && ( dx < WIDTH ) && ( dy >=0 ) && ( dy < HEIGHT ) ) {	// check within image
					edge_val = CV_IMAGE_ELEM( edgeImage, unsigned char, dy, dx );
					if ( ((double)edge_val)*exp(-((double)abs(j - circle_normal_sample_number))*0.3) > (double)max_val ) 
					{
						//cvPutText( edgeImage, ".", cvPoint( dx, dy ), &testfont, CV_RGB(255, 255, 255) );
						
						/*dist_arr[i] = */dist = abs(j - circle_normal_sample_number);
						max_val = ((double)edge_val)*exp(-dist*0.3);
						
						dist_arr[i] = j - circle_normal_sample_number;//含正負號
					}
				}
			}

			// no find edge, give penalty value
			if ( max_val == 0 )
			{
				dist_arr[i] = dist = worst_value;
			}
			else
				matching_cnt ++;

			avg_edge_value += max_val;

			total_dist = total_dist + dist;
			total_point ++;
		}
	}
	//dist_arr is constructed
	double dist_sum = 0;
	for (int k=0; k<circle_edge_sample_number/8; k++)
	{
		//if ()
		dist_sum += abs(dist_arr[k]-dist_arr[int(/*k+*/0.5*circle_edge_sample_number - k)]);
		dist_sum += abs(dist_arr[circle_edge_sample_number-1-k]-dist_arr[int(0.5*circle_edge_sample_number/*-1-k*/ + 1 + k)]);
	}
	double sym_val = (dist_sum/total_point)*4/worst_value;

	//avg_edge_value can be evaluated
	avg_edge_value = avg_edge_value/total_point/255;
	

	//cout << "total dist " << total_dist << "\ttotal point " << total_point << endl;
	if ( total_point > 0 )		// distance = [0,1]
		nor_dist = total_dist / total_point / worst_value;
	else	// all samples are out of image
		nor_dist = 1;

	return  (nor_dist + sym_val*0.4 /*+ (1-total_point/circle_edge_sample_number)*/ 
		+ (1 - avg_edge_value) + 1.2*(1-matching_cnt/circle_edge_sample_number) + 0.4/*0.2*/*(1-r/(Max_r/MODEL_R)))/3;

}


void likelihood::UpdateRefTexture( double x, double y, double r, IplImage* scrImage_gray, int target_ID)
{
	// for each target, update its texture template
	// out: M_new_template
	// in: template_size, x, y, r, scrImage_gray
	// input each template region left-top origin

	CvRect select;

	if( ref_tex_cnt[target_ID] % UPDATE_FREQ == 0 ){
		// check within image range
		select.x = (x-r*MODEL_R) < 0 ? 0 : (x+r*MODEL_R) < WIDTH 
							 ? (x-r*MODEL_R) : WIDTH-2*r*MODEL_R;
		select.y = (y-r*MODEL_R) < 0 ? 0 : (y+r*MODEL_R) < HEIGHT 
							 ? (y-r*MODEL_R) : HEIGHT-2*r*MODEL_R;
		select.width = select.height = 2*r*MODEL_R;

		cvSetImageROI( scrImage_gray, select );
		cvCopy(ref_texture[target_ID], prev_ref_texture[target_ID]);
		cvResize( scrImage_gray, ref_texture[target_ID] );
		cvResetImageROI( scrImage_gray );
		ref_tex_cnt[target_ID] = 0;
//#ifdef _DEBUG
		fileOut3 << "update texture[" << target_ID << "]" << endl;
		cvNamedWindow("ref_texture0", 1);
		cvShowImage("ref_texture0", ref_texture[0]);
		cvNamedWindow("ref_texture1", 1);
		cvShowImage("ref_texture1", ref_texture[1]);
		cvNamedWindow("ref_texture2", 1);
		cvShowImage("ref_texture2", ref_texture[2]);
		cvNamedWindow("ref_texture3", 1);
		cvShowImage("ref_texture3", ref_texture[3]);
		cvNamedWindow("ref_texture4", 1);
		cvShowImage("ref_texture4", ref_texture[4]);
//#endif
	}
	ref_tex_cnt[target_ID] ++;
	//cvShowImage( "Texture", ref_texture );
}

double likelihood::MeasureTexture( float x, float y, float r, float theta, IplImage* scrImage_gray, int target_ID )
{
	// for each particle, the most similar one has the smallest texture(SAD) distance
	// out: distance
	// in: M_template, template_size, x, y, r, scrImage_gray
	// input each SAD region left-top origin

	double distance;
	CvRect select;
	CvScalar difc;
	//IplImage* scale_img = cvCreateImage( cvSize(TEMPLATE_WIDTH,TEMPLATE_HEIGHT), IPL_DEPTH_8U, 1 );

	// current SAD size
	// check within image range
	select.x = (x-r*MODEL_R) < 0 ? 0 : (x+r*MODEL_R) < WIDTH 
						 ? (x-r*MODEL_R) : WIDTH-2*r*MODEL_R;
	select.y = (y-r*MODEL_R) < 0 ? 0 : (y+r*MODEL_R) < HEIGHT 
						 ? (y-r*MODEL_R) : HEIGHT-2*r*MODEL_R;
	select.width = select.height = 2*r*MODEL_R;

	//cout << "x:" << select.x << " y:" << select.y << " w:" << select.width << " h:" << select.height << endl;

	// set ROI at current SAD region
	cvSetImageROI( scrImage_gray, select );
	cvResize( scrImage_gray, scale_img );
	cvResetImageROI( scrImage_gray );

	// compute SAD value
	cvAbsDiff( ref_texture[target_ID], scale_img, scale_img );
	difc = cvSum( scale_img );

	distance = difc.val[0] / (TEMPLATE_WIDTH*TEMPLATE_HEIGHT) / 128;

	//cvReleaseImage( &scale_img );

	return distance;
}

double likelihood::MeasureTexture( IplImage * src, IplImage * texture )
{
	//IplImage* scale_img = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );
	CvRect rect;
	CvScalar difc;
	double distance;

	rect.x = rect.y = 0;
	rect.width = src->width;
	rect.height = src->height;
	cvSetImageROI( scale_img, rect );

	cvAbsDiff( texture, src, scale_img );
	difc = cvSum( scale_img );

	distance = difc.val[0] / (src->width*src->height) / 128;

	cvResetImageROI( scale_img );
	//cvReleaseImage( & scale_img );
	return distance;
}

double likelihood::MeasureLine( IplImage * edgeImage, CvPoint pt1, CvPoint pt2, double dx, double dy )
{
	CvPoint line;
	CvPoint normal;

	unsigned char edge_val, max_val;
	int pdist, ndist;
	int worst_value = line_normal_sample_number + 1;
	double total_pdist = 0, total_ndist = 0;
	int total_ppoint = 0, total_npoint = 0;
	double distance = 0;

	//CvFont testfont;
	//cvInitFont( &testfont, CV_FONT_VECTOR0, 1, 1, 0, 1 );

	for( int i=0; i<line_edge_sample_number; i++ ) {
		max_val = 0;
		line.x = ( (line_edge_sample_number-i)*pt1.x + i*pt2.x ) / line_edge_sample_number;
		line.y = ( (line_edge_sample_number-i)*pt1.y + i*pt2.y ) / line_edge_sample_number;
		if( ( line.x >=0 ) && ( line.x < WIDTH ) && ( line.y >=0 ) && ( line.y < HEIGHT ) ) {
			for( int j=0; j<line_normal_sample_number; j++ ) {
				normal.x = line.x + j*dx;
				normal.y = line.y + j*dy;
				//cvPutText( edgeImage, ".", normal1, &testfont, CV_RGB(0, 255, 255) );
				if ( ( normal.x >=0 ) && ( normal.x < WIDTH ) && ( normal.y >=0 ) && ( normal.y < HEIGHT ) ) {	// check within image
					edge_val = CV_IMAGE_ELEM( edgeImage, unsigned char, normal.y, normal.x );
					if ( edge_val > max_val ) {
						max_val = edge_val;
						pdist = j;
						ndist = worst_value;
					}
				}
				normal.x = line.x - j*dx;
				normal.y = line.y - j*dy;
				//cvPutText( edgeImage, ".", normal, &testfont, CV_RGB(0, 255, 255) );
				if ( ( normal.x >=0 ) && ( normal.x < WIDTH ) && ( normal.y >=0 ) && ( normal.y < HEIGHT ) ) {	// check within image
					edge_val = CV_IMAGE_ELEM( edgeImage, unsigned char, normal.y, normal.x );
					if ( edge_val > max_val ) {
						max_val = edge_val;
						ndist = j;
						pdist = worst_value;
					}
				}
			}
			if( max_val == 0 ){}
			else if( pdist < ndist ) {
				total_pdist = total_pdist + pdist;
				total_ppoint ++;
			}
			else if( pdist > ndist ) {
				total_ndist = total_ndist + ndist;
				total_npoint ++;
			}

			//cout << "line (" << line.x << ", " << line.y << ")\t";
			//cout << "dist1 " << dist1;
			//cvPutText( edgeImage, "+", line, &testfont, CV_RGB(255, 255, 255) );
			//cout << "pdist: " << pdist << "\tndist: " << ndist << endl;
		}
	}

	//cout << total_pdist << " " << total_ppoint << "\t" << -total_ndist << " " << total_npoint << "\t";
	if(total_ppoint > 0 )
		distance = total_pdist/total_ppoint;
	if(total_npoint > 0 ) {
		distance = distance/total_ppoint < total_ndist/total_npoint/total_npoint ? distance : -total_ndist/total_npoint;
	}

	//cout << distance << endl;
	return distance;
}

double likelihood::MeasureBoxEdge( IplImage *edgeImage, CvBox2D box, int length )
{
	/*------------------------------------
	 edge_dist 計算box的長邊和edge image的吻合程度
		找切線方向最大強度的pixel和box的距離
	 length_dist 代表box的長度越長越好
		依initial手臂長度的百分比當作基本的distance
		再依情況進行減分的處理
		ex: box只有0.5有edge，就要0.5*0.75當作加上的分數
	 sym_dist 代表box旁edge的對稱性如何
	*/
	CvPoint2D32f pt[4];
	CvPoint line1, line2;
	CvPoint normal1, normal2;

	double dx, dy;
	unsigned char edge_val1, edge_val2;
	unsigned char max_val1, max_val2;
	int dist1, dist2;
	int worst_value = line_normal_sample_number + 2;
	double total_dist = 0;
	int total_point = 0;
	double edge_dist = 0, length_dist = 0, sym_dist_SAD = 0, intensity_dist = 0;
	double distance = 0;
	int break_point = 0;

	dx = 1.5 * sin(box.angle*CV_PI/180);
	dy = 1.5 * cos(box.angle*CV_PI/180);

	cvBoxPoints( box, pt );

	length_dist = 1 - box.size.height/length;

	//CvFont testfont;
	//cvInitFont( &testfont, CV_FONT_VECTOR0, 1, 1, 0, 1 );

	for( int i=0; i<line_edge_sample_number; i++ ) {
		max_val1 = max_val2 = 0;
		dist1 = dist2 = 10000;
		line1.x = ( (line_edge_sample_number-i)*pt[0].x + i*pt[1].x ) / line_edge_sample_number;
		line1.y = ( (line_edge_sample_number-i)*pt[0].y + i*pt[1].y ) / line_edge_sample_number;
		if( ( line1.x >=0 ) && ( line1.x < WIDTH ) && ( line1.y >=0 ) && ( line1.y < HEIGHT ) ) {
			for( int j=0; j<line_normal_sample_number; j++ ) {
				normal1.x = line1.x + cvRound(j*dx);
				normal1.y = line1.y + cvRound(j*dy);
				//cvPutText( edgeImage, ".", normal1, &testfont, CV_RGB(0, 255, 255) );
				if ( ( normal1.x >=0 ) && ( normal1.x < WIDTH ) && ( normal1.y >=0 ) && ( normal1.y < HEIGHT ) ) {	// check within image
					edge_val1 = CV_IMAGE_ELEM( edgeImage, unsigned char, normal1.y, normal1.x );
					if ( edge_val1 > max_val1 ) {
						max_val1 = edge_val1;
						dist1 = j;
					}
				}
				normal1.x = line1.x - cvRound(j*dx);
				normal1.y = line1.y - cvRound(j*dy);
				//cvPutText( edgeImage, ".", normal1, &testfont, CV_RGB(0, 255, 255) );
				if ( ( normal1.x >=0 ) && ( normal1.x < WIDTH ) && ( normal1.y >=0 ) && ( normal1.y < HEIGHT ) ) {	// check within image
					edge_val1 = CV_IMAGE_ELEM( edgeImage, unsigned char, normal1.y, normal1.x );
					if ( edge_val1 > max_val1 ) {
						max_val1 = edge_val1;
						dist1 = -j;
					}
				}
			}
			if( max_val1 == 0 )
				dist1 = worst_value;
			intensity_dist = intensity_dist + (double)max_val1;
			total_dist = total_dist + abs(dist1);
			total_point ++;
			//cout << "line1 (" << line1.x << ", " << line1.y << ")\t";
			//cout << "dist1 " << dist1;
			//cout << "dist1\t" << dist1;
			//cvPutText( edgeImage, "+", line1, &testfont, CV_RGB(255, 255, 255) );
		}

		line2.x = ( (line_edge_sample_number-i)*pt[3].x + i*pt[2].x ) / line_edge_sample_number;
		line2.y = ( (line_edge_sample_number-i)*pt[3].y + i*pt[2].y ) / line_edge_sample_number;
		if( ( line2.x >=0 ) && ( line2.x < WIDTH ) && ( line2.y >=0 ) && ( line2.y < HEIGHT ) ) {
			for( int j=0; j<line_normal_sample_number; j++ ) {
				normal2.x = line2.x + cvRound(j*dx);
				normal2.y = line2.y + cvRound(j*dy);
				//cvPutText( edgeImage, ".", normal2, &testfont, CV_RGB(0, 255, 120) );
				if ( ( normal2.x >=0 ) && ( normal2.x < WIDTH ) && ( normal2.y >=0 ) && ( normal2.y < HEIGHT ) ) {	// check within image
					edge_val2 = CV_IMAGE_ELEM( edgeImage, unsigned char, normal2.y, normal2.x );
					if ( edge_val2 > max_val2 ) {
						max_val2 = edge_val2;
						dist2 = -j;
					}
				}
				normal2.x = line2.x - cvRound(j*dx);
				normal2.y = line2.y - cvRound(j*dy);
				//cvPutText( edgeImage, ".", normal2, &testfont, CV_RGB(0, 255, 120) );
				if ( ( normal2.x >=0 ) && ( normal2.x < WIDTH ) && ( normal2.y >=0 ) && ( normal2.y < HEIGHT ) ) {	// check within image
					edge_val2 = CV_IMAGE_ELEM( edgeImage, unsigned char, normal2.y, normal2.x );
					if ( edge_val2 > max_val2 ) {
						max_val2 = edge_val2;
						dist2 = j;
					}
				}
			}
			if( max_val2 == 0 )
				dist2 = worst_value;
			intensity_dist = intensity_dist + (double)max_val2;
			total_dist = total_dist + abs(dist2);
			total_point ++;
			//cout << "line2 (" << line2.x << ", " << line2.y << ")\n";
			//cout << "\tdist2 " << dist2 << "\ttotal dist " << total_dist << endl;
			//cout << "\tdist2\t" << dist2 << endl;
			//cvPutText( edgeImage, "+", line2, &testfont, CV_RGB(255, 255, 255) );
		}

		//cout << "max value\t" << (int)max_val1 << "\t" << (int)max_val2 << endl;
		if( max_val1==0 && max_val2==0 ){
			break_point ++;
		}
		else {
			//cout << "break_point: " << break_point << "\tdist old: " << length_dist; 
			length_dist = length_dist + ((double)break_point/line_edge_sample_number)*box.size.height/length*1.7;
			//cout<< "\tnew: " << length_dist << endl;
			break_point = 0;
		}

		if( abs(dist1)<worst_value && abs(dist2)<worst_value ){
			sym_dist_SAD = sym_dist_SAD + abs(dist1-dist2);
		}
		else{
			sym_dist_SAD = sym_dist_SAD + worst_value/2;
		}
	}
	length_dist = length_dist + ((double)break_point/line_edge_sample_number)*box.size.height/length*1.7;
	if( total_dist > total_point*worst_value*0.8 ){
		sym_dist_SAD = 1;
	}
	else{
		sym_dist_SAD = sym_dist_SAD / total_point*2 / worst_value;
	}

	//cout << "length: " << length << "\t" << box.size.height << "\tlength_dist: " << length_dist << endl;

	if ( total_point > 0 ) {		// distance = [0,1]
		edge_dist = total_dist / total_point / worst_value;
		intensity_dist = 1 - intensity_dist / total_point / 255;
	}
	else {	// all samples are out of image
		edge_dist = 1;
		intensity_dist = 1;
	}
	//cout << "total dist " << total_dist << "\ttotal point " << total_point << "\tdistance " << distance << "\tangle " << box.angle << endl;

	//cout << "sym_dist:\t" << sym_dist_dot << "\t" << sym_dist_SAD << endl;
	//cout << "length_dsit:\t" << length_dist << endl;
	//cout << "edge_dist:\t" << edge_dist << endl;
	//cout << "intensity_dist:\t" << intensity_dist << endl;

	distance = ( sym_dist_SAD + length_dist + edge_dist*2 + intensity_dist )/2 - 1;

	return distance;
}

double likelihood::MeasureBoxArea( CvBox2D upper_box, CvBox2D fore_box )
{
	double distance = 0;
	double diff_angle;
	double threshold = 45;
	float ratio = upper_box.size.height/fore_box.size.height;
	diff_angle = abs( upper_box.angle - fore_box.angle - 180 );
	//diff_angle = abs( diff_angle );
	if( diff_angle > 180 )
		diff_angle = abs( 360-diff_angle );
	//diff_angle = diff_angle > 180 ? abs( 360 - diff_angle ) : diff_angle;
	//cout << "upperbox:\t" << upper_box.angle << "fore_box:\t" << fore_box.angle << "diff->\t" << diff_angle << endl;
	if( diff_angle < threshold )
		distance = ( threshold - diff_angle ) / threshold;
	else
		distance = 0;
	//cout << "diff_angle\t" << diff_angle << "distance\t" << distance << endl;
	if( ratio > 2 )
		distance = distance + ratio*2/10;
	else if( ratio < 0.5 )
		distance = distance + 2/ratio/10;

	return distance;
}

double likelihood::MeasureRectEdge( double x, double y, double r, double alpha, IplImage *edgeImage )
{
	CvPoint pt[4];
	CvPoint line1, line2;
	CvPoint normal1, normal2;

	float dx = 1, dy = 0;
	unsigned char edge_val1, edge_val2;
	unsigned char max_val1, max_val2;
	int dist1, dist2;
	int worst_value = line_normal_sample_number + 1;
	double total_dist = 0;
	int total_point = 0;
	double distance = 0;

	//原本肩寬2r
	//身長放長為4r，只計算下半身的部份，避開手臂造成無直線的情況
	pt[0] = cvPoint( (int)(x-2*r*cos(alpha)), (int)(y+3*r) );
	pt[1] = cvPoint( (int)(x-2*r*cos(alpha)), (int)(y+3*r+2.5*r) );
	pt[2] = cvPoint( (int)(x+2*r*cos(alpha)), (int)(y+3*r) );
	pt[3] = cvPoint( (int)(x+2*r*cos(alpha)), (int)(y+3*r+2.5*r) );

	//CvFont testfont;
	//cvInitFont( &testfont, CV_FONT_VECTOR0, 1, 1, 0, 1 );

	for( int i=0; i<line_edge_sample_number; i++ ) {
		max_val1 = max_val2 = 0;
		line1.x = ( (line_edge_sample_number-i)*pt[0].x + i*pt[1].x ) / line_edge_sample_number;
		line1.y = ( (line_edge_sample_number-i)*pt[0].y + i*pt[1].y ) / line_edge_sample_number;
		if( ( line1.x >=0 ) && ( line1.x < WIDTH ) && ( line1.y >=0 ) && ( line1.y < HEIGHT ) ) {
			for( int j=0; j<line_normal_sample_number; j++ ) {
				normal1.x = line1.x + j*dx;
				normal1.y = line1.y + j*dy;
				//cvPutText( edgeImage, ".", normal1, &testfont, CV_RGB(0, 255, 255) );
				if ( ( normal1.x >=0 ) && ( normal1.x < WIDTH ) && ( normal1.y >=0 ) && ( normal1.y < HEIGHT ) ) {	// check within image
					edge_val1 = CV_IMAGE_ELEM( edgeImage, unsigned char, normal1.y, normal1.x );
					if ( edge_val1 > max_val1 ) {
						max_val1 = edge_val1;
						dist1 = j;
					}
				}
				normal1.x = line1.x - j*dx;
				normal1.y = line1.y - j*dy;
				//cvPutText( edgeImage, ".", normal1, &testfont, CV_RGB(0, 255, 255) );
				if ( ( normal1.x >=0 ) && ( normal1.x < WIDTH ) && ( normal1.y >=0 ) && ( normal1.y < HEIGHT ) ) {	// check within image
					edge_val1 = CV_IMAGE_ELEM( edgeImage, unsigned char, normal1.y, normal1.x );
					if ( edge_val1 > max_val1 ) {
						max_val1 = edge_val1;
						dist1 = j;
					}
				}
			}
			if( max_val1 == 0 )
				dist1 = worst_value;
			total_dist = total_dist + dist1;
			total_point ++;
			//cout << "line1 (" << line1.x << ", " << line1.y << ")\t";
			//cout << "dist1 " << dist1;
			//cvPutText( edgeImage, "+", line1, &testfont, CV_RGB(255, 255, 255) );
		}

		line2.x = ( (line_edge_sample_number-i)*pt[3].x + i*pt[2].x ) / line_edge_sample_number;
		line2.y = ( (line_edge_sample_number-i)*pt[3].y + i*pt[2].y ) / line_edge_sample_number;
		if( ( line2.x >=0 ) && ( line2.x < WIDTH ) && ( line2.y >=0 ) && ( line2.y < HEIGHT ) ) {
			for( int j=0; j<line_normal_sample_number; j++ ) {
				normal2.x = line2.x + j*dx;
				normal2.y = line2.y + j*dy;
				//cvPutText( edgeImage, ".", normal2, &testfont, CV_RGB(0, 255, 120) );
				if ( ( normal2.x >=0 ) && ( normal2.x < WIDTH ) && ( normal2.y >=0 ) && ( normal2.y < HEIGHT ) ) {	// check within image
					edge_val2 = CV_IMAGE_ELEM( edgeImage, unsigned char, normal2.y, normal2.x );
					if ( edge_val2 > max_val2 ) {
						max_val2 = edge_val2;
						dist2 = j;
					}
				}
				normal2.x = line2.x - j*dx;
				normal2.y = line2.y - j*dy;
				//cvPutText( edgeImage, ".", normal2, &testfont, CV_RGB(0, 255, 120) );
				if ( ( normal2.x >=0 ) && ( normal2.x < WIDTH ) && ( normal2.y >=0 ) && ( normal2.y < HEIGHT ) ) {	// check within image
					edge_val2 = CV_IMAGE_ELEM( edgeImage, unsigned char, normal2.y, normal2.x );
					if ( edge_val2 > max_val2 ) {
						max_val2 = edge_val2;
						dist2 = j;
					}
				}
			}
			if( max_val2 == 0 )
				dist2 = worst_value;
			total_dist = total_dist + dist2;
			total_point ++;
			//cout << "line2 (" << line2.x << ", " << line2.y << ")\n";
			//cout << "\tdist2 " << dist2 << "\ttotal dist " << total_dist << endl;
			//cvPutText( edgeImage, "+", line2, &testfont, CV_RGB(255, 255, 255) );
		}
	}

	//cvRectangle( edgeImage, pt[0], pt[3], CV_RGB(255,255,255), 1, 8, 0 );
	if ( total_point > 0 )		// distance = [0,1]
		distance = total_dist / total_point / worst_value;
	else	// all samples are out of image
		distance = 1;
	//cout << "total dist " << total_dist << "\ttotal point " << total_point << "\tdistance " << distance << "\tangle " << box.angle << endl;
	return distance;
}

double likelihood::HomogenousRegion( int x_min, int x_max, int y_min, int y_max, CvHistogram * cmp_hist, IplImage* srcImage_Cr, IplImage* srcImage_Cb )
{
	// measure the difference of the homogeneous region (color)
	// out: dist
	// in: x_min, x_max, y_min, y_max, ref_hist_h, ref_hist_s, (of the target's inner blob), scrImage_h, scrImage_s
	CvRect in_select, top_select, bottom_select, left_select, right_select;
	double dist;
	double in_dist, top_dist, bottom_dist, left_dist, right_dist;
	int in_action, top_action, right_action, bottom_action, left_action;

	// Set in_ROI
	in_select.x = x_min;
	in_select.y = y_min;
	in_select.width = x_max - x_min + 1;
	in_select.height = y_max - y_min + 1;
	BoundSelection( in_action, in_select, in_select );
	CompareColorHist( in_dist, in_action, cmp_hist, in_select, srcImage_Cr, srcImage_Cb );

	double w = ( sqrt_2 + 1 )/2 * in_select.width;
	double h = ( sqrt_2 - 1 )/2 * in_select.height;

	// Set top_ROI
	top_select.x = x_min - h;
	top_select.y = y_min - h;
	top_select.width = w;
	top_select.height = h;
	BoundSelection( top_action, top_select, top_select );
	CompareColorHist( top_dist, top_action, cmp_hist, top_select, srcImage_Cr, srcImage_Cb );

	// Set right_ROI
	right_select.x = x_max;
	right_select.y = y_min - h;
	right_select.width = h;
	right_select.height = w;
	BoundSelection( right_action, right_select, right_select );
	CompareColorHist( right_dist, right_action, cmp_hist, right_select, srcImage_Cr, srcImage_Cb );

	// Set bottom_ROI
	bottom_select.x = x_min;
	bottom_select.y = y_max + h;
	bottom_select.width = w;
	bottom_select.height = h;
	BoundSelection( bottom_action, bottom_select, bottom_select );
	CompareColorHist( bottom_dist, bottom_action, cmp_hist, bottom_select, srcImage_Cr, srcImage_Cb );

	// Set left_ROI
	left_select.x = x_min - h;
	left_select.y = y_min;
	left_select.width = h;
	left_select.height = w;
	BoundSelection( left_action, left_select, left_select );
	CompareColorHist( left_dist, left_action, cmp_hist, left_select, srcImage_Cr, srcImage_Cb );


	// MINIMIZE the in_distance, MAXIMIZE others
	dist = ( in_dist + 1 - ( top_dist + right_dist + bottom_dist + left_dist ) / ( top_action + right_action + bottom_action + left_action ) ) / 2;
	// ONLY MINIMIZE the in_distance (within the target)
	//total_distance = in_distance;
	//cout << in_dist << "\t" << top_dist << "\t" << right_dist << "\t" << bottom_dist << "\t" << left_dist << endl;

	return dist;
}

void likelihood::BoundSelection( int &action, CvRect out_selection, CvRect in_selection )
{
	int check = 0;
	out_selection = in_selection;

	if ( ( in_selection.width <= 0 ) && ( in_selection.height <= 0 ) )		// no used
	{
		action = 0;
		check = 0;
	}
	else if ( ( in_selection.x >= 0 ) && ( in_selection.y >= 0 ) && ( (in_selection.x + in_selection.width) < WIDTH ) && ( (in_selection.y + in_selection.height) < HEIGHT ) )	// fully used
	{
		action = 1;
		check = 0;
	}
	else if ( ( (in_selection.x + in_selection.width) <= 0 ) || ( (in_selection.y + in_selection.height) <= 0 ) )	// no used
	{
		action = 0;
		check = 0;
	}
	else if ( ( in_selection.x >= WIDTH ) || ( in_selection.y >= HEIGHT ) )		// no used
	{
		action = 0;
		check = 0;
	}
	else
		check = 1;


	if ( check == 1 )		// partly used
	{
		action = 1;

		if ( in_selection.x < 0 )	
		{
			out_selection.x = 0;
			out_selection.width = in_selection.width - in_selection.x;
		}
		if ( in_selection.y < 0 )
		{
			out_selection.y = 0;
			out_selection.height = in_selection.height - in_selection.y;
		}
		if ( (in_selection.x + in_selection.width) > WIDTH )	
		{
			out_selection.width = WIDTH - in_selection.x;
		}
		if ( (in_selection.y + in_selection.height) > HEIGHT )	
		{
			out_selection.height = HEIGHT - in_selection.y;
		}
	}

}


void likelihood::CompareColorHist( double& distance, int action, CvHistogram * cmp_hist, CvRect selection, IplImage * srcImage_Cr, IplImage* srcImage_Cb )
{
	// the most similar one has the smallest BHATTACHARYYA distance
	// out: distance
	// in: ref_hist_h, ref_hist_s, selection, scrImage_h, scrImage_s

	if ( action == 1)
	{
		cvSetImageROI( srcImage_Cr, selection );
		cvSetImageROI( srcImage_Cb, selection );

		IplImage * planes[] = { srcImage_Cr, srcImage_Cb };
		// Get current histogram
		cvCalcHist( planes, current_hist, 0, NULL );

		float max_val = 0.0f;
		cvGetMinMaxHistValue( current_hist, 0, &max_val, 0, 0 );
		cvConvertScale( current_hist->bins, current_hist->bins, max_val ? 255. / max_val : 0., 0 );		// need ?????

		// Normalize
		//cvNormalizeHist( ref_hist, 1 );
		cvNormalizeHist( current_hist, 1 );

		// Note: the method CV_COMP_BHATTACHARYYA only works with normalized histograms.
		// cvNormalizeHist normalizes the histogram bins by scaling them, such that the sum of the bins becomes equal to factor.
		// double cvCompareHist( const CvHistogram* hist1, const CvHistogram* hist2, int method );
		//cvCalcBackProjectPatch

		// Compare
		distance = cvCompareHist( cmp_hist, current_hist, CV_COMP_BHATTACHARYYA );

		cvResetImageROI( srcImage_Cr );
		cvResetImageROI( srcImage_Cb );
	}
	else
		distance = 0;
}
void likelihood::ConstructSINCOSR()
{
	double delta = 2*CV_PI/circle_edge_sample_number;
	for (int i=0; i<circle_edge_sample_number; i++)
	{
		COS[i] = cos(delta*i);
		SIN[i] = sin(delta*i);
		R[i] = 0;
	}

	double sml_delta = 2*CV_PI/(circle_edge_sample_number*10);
	for (int i=0; i<circle_edge_sample_number*10; i++)
	{
		MAP_COS[i] = cos(sml_delta*i);
		MAP_SIN[i] = sin(sml_delta*i);
		MAP_R[i] = 0;
	}

}
void likelihood::SampleEllipseContour()
{
	// pre-construct the sample points & their normal vectors on ellipse ( for track human head, instead of circles )
	// ellipse center (0,0), short axis:a, long axis:b

	int r, i, x, y;
	double a, b;
	// int K = cvRound(normal_sample_number/2);
	double theta;

	if (M_circleedgesamples != NULL)
	{
		cvReleaseMat( &M_circleedgesamples );
		M_circleedgesamples = NULL;
	}
	
	M_circleedgesamples = cvCreateMat( (Max_r + circle_normal_sample_number*2) * circle_edge_sample_number, 2 /*+ 2*circle_normal_sample_number*/, CV_32FC1 );
	cvSetZero( M_circleedgesamples );

	// for show sample points
	//IplImage* testimage = cvCreateImage( cvSize(WIDTH,HEIGHT), 8, 1 );
	//cvSetZero( testimage );
	//CvFont testfont;
	//cvInitFont( &testfont, CV_FONT_VECTOR0, 1, 1, 0, 1 );

	for ( r = Min_r - circle_normal_sample_number; r <= Max_r + circle_normal_sample_number; r++ )
	{
		a = r;
		b = ellipse_ratio * a;

		// sample points & their normal vectors
		for ( i = 0; i < circle_edge_sample_number; i++ )
		{
			theta = i * 2 * CV_PI / circle_edge_sample_number;

			// sample point
			x = cvRound( a * cos(theta) );
			y = cvRound( b * sin(theta) );
			cvmSet( M_circleedgesamples, r * circle_edge_sample_number + i, 0, x );
			cvmSet( M_circleedgesamples, r * circle_edge_sample_number + i, 1, y );
			//cvPutText( testimage, ".", cvPoint( 150+cvmGet( M_circleedgesamples, r * circlecontour_sample_number + i, 0), 150+cvmGet( M_circleedgesamples, r * circlecontour_sample_number + i, 1) ), &testfont, CV_RGB(255, 255, 255) );
		}

	}

	//cvNamedWindow( "circle contour", 1 );
	//cvShowImage( "circle contour", testimage );
	//cvWaitKey(0);
	//cvReleaseImage( &testimage );
}

void likelihood::prtPline(int r, int c, double angle, IplImage* img)
{
	double m = tan(angle);
	double b = ((double)c)-m*((double)r);

	//plot the line by scaning through rows
	for (int x=0; x<img->height; x++)
	{
		if (m*((double)x)+b < img->width && m*((double)x)+b >= 0)
			CV_IMAGE_ELEM(img, unsigned char, x, int(m*((double)x)+b)) = 0 ;
	}
	
	//plot the line by scaning through columns, to avoid dot-lines with steep slopes
	for (int y=0; y<img->width; y++)
	{
		if (((double)y-b)/m < img->height && ((double)y-b)/m >= 0)
			CV_IMAGE_ELEM(img, unsigned char, int(((double)y-b)/m), y) = 0 ;
	}

}

//cvFindContours with CvSeq implementation version 1
//using small scale image operation
//Choose the contour based on rough geometry and size continuity
//draw particles after cvMoments
bool likelihood::ConnectedComponent1(IplImage* backproj, CvMat* mx, CvMat* my, CvMat* mr, CvMat* mtheta)
{
	//GOAL: independent resource for particle, hopefully not (or slightly) correlated with our previous estimation
	//sometimes connected component is good but MAP result is bad, so never use MAP as feedback here
	//QUICK search for best connected component using rough contour information (approximation)
	//Once decide which connected component, refine all information carefully to draw particles
	//restore all refined information for next iteration here
	//beware of small/large scale
	

	int c, count ;
	int value, r_sum, c_sum;

	cvResize(backproj, sml_bkpj, 1);

    bp_contour = 0 ;

    cvFindContours( sml_bkpj, bp_storage, &bp_contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ) ;
    
    //count = 255 ;

	double bestArea = 0, curArea = 0, maxScore = -10000, curScore = 0;
	CvPoint bestLoc = cvPoint(10000,10000);
	CvPoint pt;
	main_contour = NULL;

	cvZero(sml_CC_out);
	int contour_num = 0, sml_CC_count = 0;
	for( ; bp_contour != 0 ; bp_contour = bp_contour->h_next )
    {
		contour_num++;
		curArea = abs(cvContourArea(bp_contour));
		int sumx = 0, sumy = 0;
		for (int k=0; k < 4; k++)
		{
			pt = *(CvPoint*)cvGetSeqElem(bp_contour, k*(bp_contour->total/4));
			sumx += pt.x;
			sumy += pt.y;
		}
		sumx/=4;
		sumy/=4;
		pt = cvPoint(sumx, sumy);

		curScore = -(abs(pt.x - lastLoc.x) + abs(pt.y - lastLoc.y) + sqrt(abs(curArea - lastArea))); 
		if (curArea < 350.0) 
		{
			curScore = -10000;
			sml_CC_count++;
		}

//		fileOut3 << "contour area = " << cur_area << endl;
		if (curScore > maxScore)
		{
			maxScore = curScore;
			main_contour = bp_contour;
			bestArea = curArea;
			bestLoc = pt;

		}
    }

	if (contour_num == 0 || contour_num == sml_CC_count)
	{
		return false;
	}
    CvScalar color = cvScalar(255);
	cvZero(sml_bkpj);
   	cvDrawContours( sml_bkpj, main_contour, color, color, -1, CV_FILLED, 8 ) ;
	double r = sqrt(bestArea*4.0/(CV_PI*ellipse_ratio))/MODEL_R;
	
	CvMoments m;
		
	cvMoments( sml_bkpj, &m, 0/*1*/ ) ;
	cvGetSpatialMoment( &m, 1, 0);
	cvGetSpatialMoment( &m, 0, 1);
	int centerX = m.m10/m.m00;
	int centerY = m.m01/m.m00;

	cvGetCentralMoment( &m, 1, 1 ) ;
	cvGetCentralMoment( &m, 2, 0 ) ;
	cvGetCentralMoment( &m, 0, 2 ) ;

	double thita = 180 * ( atan2( 2*m.mu11 , m.mu20 - m.mu02 ) / (2*3.1415926) ) ;
	double stheta = (thita >= 0 ? 90-thita : -(90+thita) );
	////////fileOut3 << "......................................\n" << endl;
	////////fileOut3 << "(x,y) = " << "(" << centerX*2.0 << "," << centerY*2.0 << ")" << endl;
	////////fileOut3 << "scale = " << r << endl;
	////////fileOut3 << "angle = " << stheta + 90.0 << endl;

	cvCircle(sml_bkpj, cvPoint(centerX, centerY), 3, cvScalar(125), -1);
	prtPline(centerY, centerX, (stheta/180)*3.1415926, sml_bkpj);
////reserve
	cvNamedWindow("sml_bkpj",1);
	cvShowImage("sml_bkpj",sml_bkpj);

	CvRNG rng = cvRNG( rand() );

	cvRandArr( &rng, mx, CV_RAND_NORMAL, cvScalar(centerX*2.0) /* mean */, cvScalar(X_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Blob.MinX()) , cvScalar(Blob.MaxX()) );
	cvRandArr( &rng, my, CV_RAND_NORMAL, cvScalar(centerY*2.0) /* mean */, cvScalar(Y_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Blob.MinY()) , cvScalar(Blob.MaxY()) );
	cvRandArr( &rng, mr, CV_RAND_NORMAL, cvScalar(0.9*r/*0.5*min(temp.size.width, temp.size.height)/MODEL_R*/) /* mean */, cvScalar(R_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );
	cvRandArr( &rng, mtheta, CV_RAND_NORMAL, cvScalar(stheta + 90.0) /* mean */, cvScalar(THETA_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );

	lastArea = bestArea;
	lastScore = maxScore;
	lastLoc = cvPoint(centerX, centerY);

	return true;

	
}

//cvFindContours with CvSeq implementation version 2
//using large scale image operation
//Choose the contour based on rough geometry and size continuity
//draw particles after cvMoments
bool likelihood::ConnectedComponent2(IplImage* backproj, CvMat* mx, CvMat* my, CvMat* mr, CvMat* mtheta)
{
	//GOAL: independent resource for particle, hopefully not (or slightly) correlated with our previous estimation
	//sometimes connected component is good but MAP result is bad, so never use MAP as feedback here
	//QUICK search for best connected component using rough contour information (approximation)
	//Once decide which connected component, refine all information carefully to draw particles
	//restore all refined information for next iteration here
	//beware of small/large scale
	

	int c, count ;
	int value, r_sum, c_sum;

    bp_contour = 0 ;

    cvFindContours( backproj, bp_storage, &bp_contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ) ;
    
    //count = 255 ;

	double bestArea = 0, curArea = 0, maxScore = -10000, curScore = 0;
	CvPoint bestLoc = cvPoint(10000,10000);
	CvPoint pt;
	main_contour = NULL;

	cvZero(CC_out);
	int contour_num = 0;
	for( ; bp_contour != 0 ; bp_contour = bp_contour->h_next )
    {
		contour_num++;
		curArea = abs(cvContourArea(bp_contour));
		int sumx = 0, sumy = 0;
		for (int k=0; k < 4; k++)
		{
			pt = *(CvPoint*)cvGetSeqElem(bp_contour, k*(bp_contour->total/4));
			sumx += pt.x;
			sumy += pt.y;
		}
		sumx/=4;
		sumy/=4;
		pt = cvPoint(sumx, sumy);

		curScore = -(abs(pt.x - lastLoc.x) + abs(pt.y - lastLoc.y) + sqrt(abs(curArea - lastArea))); 

//		fileOut3 << "contour area = " << cur_area << endl;
		if (curScore > maxScore)
		{
			maxScore = curScore;
			main_contour = bp_contour;
			bestArea = curArea;
			bestLoc = pt;

		}
    }

	if (contour_num == 0)
	{
		return false;
	}
    CvScalar color = cvScalar(255);
	cvZero(backproj);
   	cvDrawContours( backproj, main_contour, color, color, -1, CV_FILLED, 8 ) ;
	double r = sqrt(bestArea/(CV_PI*ellipse_ratio))/MODEL_R;
	
	CvMoments m;
		
	cvMoments( backproj, &m, 0/*1*/ ) ;
	cvGetSpatialMoment( &m, 1, 0);
	cvGetSpatialMoment( &m, 0, 1);
	int centerX = m.m10/m.m00;
	int centerY = m.m01/m.m00;

	cvGetCentralMoment( &m, 1, 1 ) ;
	cvGetCentralMoment( &m, 2, 0 ) ;
	cvGetCentralMoment( &m, 0, 2 ) ;

	double thita = 180 * ( atan2( 2*m.mu11 , m.mu20 - m.mu02 ) / (2*3.1415926) ) ;
	double stheta = (thita >= 0 ? 90-thita : -(90+thita) );
	////////fileOut3 << "......................................\n" << endl;
	////////fileOut3 << "(x,y) = " << "(" << centerX*2.0 << "," << centerY*2.0 << ")" << endl;
	////////fileOut3 << "scale = " << r << endl;
	////////fileOut3 << "angle = " << stheta + 90.0 << endl;

	cvCircle(backproj, cvPoint(centerX, centerY), 3, cvScalar(125), -1);
	prtPline(centerY, centerX, (stheta/180)*3.1415926, sml_bkpj);
#ifdef _DEBUG
	cvNamedWindow("backproj",1);
	cvShowImage("backproj",backproj);
#endif

	CvRNG rng = cvRNG( rand() );

	cvRandArr( &rng, mx, CV_RAND_NORMAL, cvScalar(centerX) /* mean */, cvScalar(X_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Blob.MinX()) , cvScalar(Blob.MaxX()) );
	cvRandArr( &rng, my, CV_RAND_NORMAL, cvScalar(centerY) /* mean */, cvScalar(Y_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Blob.MinY()) , cvScalar(Blob.MaxY()) );
	cvRandArr( &rng, mr, CV_RAND_NORMAL, cvScalar(r/*0.5*min(temp.size.width, temp.size.height)/MODEL_R*/) /* mean */, cvScalar(R_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );
	cvRandArr( &rng, mtheta, CV_RAND_NORMAL, cvScalar(stheta + 90.0) /* mean */, cvScalar(THETA_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );

	lastArea = bestArea;
	lastScore = maxScore;
	lastLoc = cvPoint(centerX, centerY);

	return true;

	
}

//cvFindContour with CvSeq implementation version 0
//using small scale image operation
//Choose the contour based on largest size
//draw particles after cvMoments
bool likelihood::ConnectedComponent(IplImage* backproj, CvMat* mx, CvMat* my, CvMat* mr, CvMat* mtheta)
{
	int c, count ;
	int value, r_sum, c_sum;

	cvResize(backproj, sml_bkpj, 1);

    bp_contour = 0 ;

    cvFindContours( sml_bkpj, bp_storage, &bp_contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ) ;
    
    //count = 255 ;

	double max_area = 0, cur_area = 0;
	CvSeq* main_contour = NULL;
	cvZero(sml_CC_out);
	int contour_num = 0;
	for( ; bp_contour != 0 ; bp_contour = bp_contour->h_next )
    {
		contour_num++;

		cur_area = abs(cvContourArea(bp_contour));
//		fileOut3 << "contour area = " << cur_area << endl;
		if (cur_area > max_area)
		{
			main_contour = bp_contour;
			max_area = cur_area;
		}
    }
	if (contour_num == 0)
	{
		return false;
	}
    CvScalar color = cvScalar(255);
	cvZero(sml_bkpj);
   	cvDrawContours( sml_bkpj, main_contour, color, color, -1, CV_FILLED, 8 ) ;
	double r = sqrt(max_area*4.0/(CV_PI*ellipse_ratio))/MODEL_R;
	
	CvMoments m;
		
	cvMoments( sml_bkpj, &m, 0/*1*/ ) ;
	cvGetSpatialMoment( &m, 1, 0);
	cvGetSpatialMoment( &m, 0, 1);
	int centerX = m.m10/m.m00;
	int centerY = m.m01/m.m00;

	cvGetCentralMoment( &m, 1, 1 ) ;
	cvGetCentralMoment( &m, 2, 0 ) ;
	cvGetCentralMoment( &m, 0, 2 ) ;

	double thita = 180 * ( atan2( 2*m.mu11 , m.mu20 - m.mu02 ) / (2*3.1415926) ) ;
	double stheta = (thita >= 0 ? 90-thita : -(90+thita) );
	////////fileOut3 << "......................................\n" << endl;
	////////fileOut3 << "(x,y) = " << "(" << centerX*2.0 << "," << centerY*2.0 << ")" << endl;
	////////fileOut3 << "scale = " << r << endl;
	////////fileOut3 << "angle = " << stheta + 90.0 << endl;

	cvCircle(sml_bkpj, cvPoint(centerX, centerY), 3, cvScalar(125), -1);
	prtPline(centerY, centerX, (stheta/180)*3.1415926, sml_bkpj);
#ifdef _DEBUG
	cvNamedWindow("sml_bkpj",1);
	cvShowImage("sml_bkpj",sml_bkpj);
#endif
	CvRNG rng = cvRNG( rand() );

	cvRandArr( &rng, mx, CV_RAND_NORMAL, cvScalar(centerX*2.0) /* mean */, cvScalar(X_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Blob.MinX()) , cvScalar(Blob.MaxX()) );
	cvRandArr( &rng, my, CV_RAND_NORMAL, cvScalar(centerY*2.0) /* mean */, cvScalar(Y_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Blob.MinY()) , cvScalar(Blob.MaxY()) );
	cvRandArr( &rng, mr, CV_RAND_NORMAL, cvScalar(r/*0.5*min(temp.size.width, temp.size.height)/MODEL_R*/) /* mean */, cvScalar(R_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );
	cvRandArr( &rng, mtheta, CV_RAND_NORMAL, cvScalar(stheta + 90.0) /* mean */, cvScalar(THETA_DEV) /* standard deviation */);//CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );

	return true;

	
}

CvPoint likelihood::interactionPoint( CvPoint pt1, float angle1, CvPoint pt2, float angle2 )
{
	CvPoint point;
	double x, y;
	if( (int)(angle1-angle2)%180 == 0 ) {
		x = y = 10000;
	}
	else if( (int)angle1%90 == 0 && ((int)angle1/90)%2 == 1 ) {
		x = pt1.x;
		y = -tan(angle2*CV_PI/180)*(x - pt2.x) + pt2.y;
	}
	else if( (int)angle2%90 == 0 && ((int)angle2/90)%2 == 1) {
		x = pt2.x;
		y = -tan(angle1*CV_PI/180)*(x - pt1.x) + pt1.y;
	}
	else{
		x = (tan(angle1*CV_PI/180)*pt1.x - tan(angle2*CV_PI/180)*pt2.x + pt1.y - pt2.y )/(tan(angle1*CV_PI/180)-tan(angle2*CV_PI/180));
		y = -tan(angle1*CV_PI/180)*(x - pt1.x) + pt1.y;
	}

	point = cvPoint( cvRound(x), cvRound(y) );
	//cout << "point1: " << pt1.x << "\t" << pt1.y << "\tangle1: " << angle1 << endl;
	//cout << "point2: " << pt2.x << "\t" << pt2.y << "\tangle2: " << angle2 << endl;
	//cout << "point: " << point.x << "\t" << point.y << endl << endl;

	return point;
}