#include "stdafx.h"
#include "backProj.h"

BackProj::BackProj()
{

	ck = cvCreateStructuringElementEx( 11, 11, 0, 0, CV_SHAPE_ELLIPSE, NULL );
}
BackProj::~BackProj()
{
	cvReleaseStructuringElement( & ck );
}

//計算參考影像的 Cr, Cb channel 的histogram
void BackProj::setColorMap( char * filename, CvHistogram * set_hist )
{
	IplImage * mapImage = cvLoadImage(filename,1);
	if( mapImage ) {
		IplImage * mapImage_Y = cvCreateImage(cvGetSize(mapImage),8,1);
		IplImage * mapImage_Cr = cvCreateImage(cvGetSize(mapImage),8,1);
		IplImage * mapImage_Cb = cvCreateImage(cvGetSize(mapImage),8,1);

		cvCvtColor(mapImage, mapImage, CV_BGR2YCrCb);
		cvSplit(mapImage,mapImage_Y, mapImage_Cr, mapImage_Cb, NULL );
		IplImage * planes[] = {mapImage_Cr, mapImage_Cb};

		cvCalcHist( planes, set_hist, 0, NULL );
		float max_val = 0.0f;
		cvGetMinMaxHistValue( set_hist, 0, &max_val, 0, 0 );
		cvConvertScale( set_hist->bins, set_hist->bins, max_val ? 255. / max_val : 0., 0 );

		cvReleaseImage(&mapImage);
		cvReleaseImage(&mapImage_Y);
		cvReleaseImage(&mapImage_Cr);
		cvReleaseImage(&mapImage_Cb);
	}
	else{
		cout << "No Input Skin Color Image!" << endl;
		system( "pause" );
	}
	cvReleaseImage( & mapImage );
}
void BackProj::UpdateHist( IplImage * Cr, IplImage * Cb, CvRect ROI_rect, CvHistogram * update_hist )
{
	cvSetImageROI( Cr, ROI_rect );
	cvSetImageROI( Cb, ROI_rect );
	
	IplImage * planes[] = { Cr, Cb };
	cvCalcHist( planes, update_hist, 0, NULL);
	float max_val = 0.0f;
	cvGetMinMaxHistValue(update_hist, 0 , &max_val, 0, 0);
	cvConvertScale( update_hist->bins, update_hist->bins, max_val? 255./max_val : 0. , 0);

	cvResetImageROI( Cr );
	cvResetImageROI( Cb );
}
//計算目前影像和參考影像的顏色相似程度
//使用之前建立好的 color histogram, 和 目前影像做比對
void BackProj::backProject( IplImage * Cr, IplImage * Cb, IplImage * dst, CvHistogram * cmp_hist )
{
	CvRect center;
	center.x = WIDTH/3;			center.y = 0;
	center.height = HEIGHT/2;	center.width = WIDTH/3;

	IplImage * morphImage = cvCreateImage( cvSize(WIDTH,HEIGHT), 8, 1 );
	cvZero( dst );
	
	cvSetImageROI( dst, center );
	cvSetImageROI( Cr, center );
	cvSetImageROI( Cb, center );
	cvSetImageROI( morphImage, center );

	IplImage * planes[] = {Cr, Cb};
	cvCalcBackProject( planes, morphImage, cmp_hist );

	cvDilate( morphImage, dst, ck, 1 );
	cvFlip( dst, morphImage, -1 );
	cvDilate( morphImage, dst, ck, 1 );

	cvThreshold( dst, morphImage, 150, 255, CV_THRESH_BINARY );
	cvFlip( morphImage, dst, -1 );

	cvResetImageROI( dst );
	cvResetImageROI( Cr );
	cvResetImageROI( Cb );
	cvResetImageROI( morphImage );

	cvReleaseImage( &morphImage );
}

void BackProj::drawParticles( IplImage * src, CvMat * x, CvMat * y, CvMat * r )
{
	CBlob Blob;
	int num, pfnum, count = 0;
	totalArea = 0;
	CvRNG rng = cvRNG( rand() );

	// Add a black boundary to avoid the blob adjecent to boundary will not be detected !!!
	cvRectangle( src, cvPoint(0,0), cvPoint(WIDTH,HEIGHT), CV_RGB(0, 0, 0), 10, 8, 0 );

	blobs = CBlobResult( src, NULL, 100, true );
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, Min_r * Min_r * CV_PI * 0.4 );
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, Max_r * Max_r * CV_PI * 6 );
	num = blobs.GetNumBlobs();
	//for( int i = 0; i < num; i++ )
	//{
	//	Blob = blobs.GetBlob(i);
	//	Blob.FillBlob( src, cvScalar(128) );
	//}
	//cvShowImage( "ConnectedComponent", src );
	//cvWaitKey(10000);

	if( num == 0 ){
		cvRandArr( &rng, x, CV_RAND_UNI, cvScalar(0) , cvScalar(WIDTH) );
		cvRandArr( &rng, y, CV_RAND_UNI, cvScalar(0) , cvScalar(HEIGHT) );
		cvRandArr( &rng, r, CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );
	}
	else {
		for( int i = 0; i < num; i++ ) {
			// Get the blob info
			Blob = blobs.GetBlob(i);
			if( Blob.Area() < Min_r * Min_r * CV_PI && Blob.Area() > Min_r * Min_r * CV_PI * 0.4 )
				totalArea += (Blob.Area()/2);
			else
				totalArea += Blob.Area();
		}

		for( int i=0; i<num; i++ ){
			Blob = blobs.GetBlob(i);
			if( Blob.Area() < Min_r * Min_r * CV_PI && Blob.Area() > Min_r * Min_r * CV_PI * 0.4 )
				pfnum = (Blob.Area()/2) / totalArea * base_particles_number;
			else
				pfnum = Blob.Area() / totalArea * base_particles_number;

			if( num-i == 1 )
				pfnum = base_particles_number - count;

			if(pfnum > 1) {
				CvMat * X_rand = cvCreateMat( 1, pfnum, CV_32FC1 );
				CvMat * Y_rand = cvCreateMat( 1, pfnum, CV_32FC1 );
				CvMat * R_rand = cvCreateMat( 1, pfnum, CV_32FC1 );

				cvGetCols( x, X_rand, count, count + pfnum-1 );
				cvGetCols( y, Y_rand, count, count + pfnum-1 );
				cvGetCols( r, R_rand, count, count + pfnum-1 );

				cvRandArr( &rng, X_rand, CV_RAND_UNI, cvScalar(Blob.MinX()) , cvScalar(Blob.MaxX()) );
				cvRandArr( &rng, Y_rand, CV_RAND_UNI, cvScalar(Blob.MinY()) , cvScalar(Blob.MaxY()) );
				cvRandArr( &rng, R_rand, CV_RAND_UNI, cvScalar(Min_r) , cvScalar(Max_r) );

				count += pfnum;
				cvReleaseMat( & X_rand );
				cvReleaseMat( & Y_rand );
				cvReleaseMat( & R_rand );
			}
			if(pfnum == 1) {
				cvmSet( x, 0, count, (Blob.MinX() + Blob.MaxX())/2 );
				cvmSet( y, 0, count, (Blob.MinY() + Blob.MaxY())/2 );
				cvmSet( r, 0, count, (Min_r + Max_r)/2 );
				count ++;
			}
		}
	}
}