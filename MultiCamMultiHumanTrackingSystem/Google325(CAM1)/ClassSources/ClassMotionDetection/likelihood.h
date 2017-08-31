#pragma once


#include <iostream>
#include <fstream>
using namespace std;
#include <time.h>
#include <iomanip>
#include <math.h>
#include <stdlib.h>

#include <cvcam.h>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

//#include <blob.h>
//#include <BlobResult.h>


#define circle_edge_sample_number 40 //팒stdafx.h쪷쭮
#define MAX_TARGET_NUM 5 //팒CAMShift.h 쪷쭮

#define CAM1 0 //팒CAMShift.h쪷쭮
#define CAM2 1 //팒CAMShift.h쪷쭮
#define CAM3 2 //팒CAMShift.h쪷쭮


//#define FPS

using namespace std;

class likelihood
{
	public:
		
		double R[circle_edge_sample_number];
		double MAP_R[circle_edge_sample_number*10];
		int ref_tex_cnt[MAX_TARGET_NUM];

		likelihood();
		likelihood(double elp_ratio);
		~likelihood();

		void setEllipse_ratio(double elp_ratio);

		void UpdateRefHistogram( float x, float y, float r, IplImage* srcImage_Cr, IplImage* srcImage_Cb );
		void UpdateRefHistogram( CvBox2D box, CvRect bound, IplImage* srcImage_Cr, IplImage* srcImage_Cb, CvHistogram * update_hist );
		double MeasureColor( IplImage* scrImage_Cr, IplImage* scrImage_Cb, double x, double y, double r, double theta, CvHistogram * cmp_hist, IplImage* src );
		double MeasureColor1( IplImage* scrImage_Cr, IplImage* scrImage_Cb, const double x, const double y, const double r, const double theta, CvHistogram * cmp_hist, IplImage* src, IplImage* msk );
		double MeasureColor2( IplImage* scrImage_Cr, IplImage* scrImage_Cb, const double x, const double y, const double r, const double theta, CvHistogram * cmp_hist, IplImage* src, IplImage* msk );
		double MeasureHairColor(IplImage* scrImage_Cr, IplImage* scrImage_Cb, const double x, const double y, const double r, CvHistogram * cmp_hist, IplImage* src, IplImage* msk);
		double MeasureBoxColor( IplImage * srcImage_Cr, IplImage * srcImage_Cb, CvBox2D box, CvHistogram * cmp_hist, CvRect rect );
		double MeasureBoxArea( CvBox2D upper_box, CvBox2D fore_box );
		double MeasureRectColor( IplImage * srcImage_Cr, IplImage * srcImage_Cb, int x, int y, int width, CvHistogram const * cmp_hist );
		void UpdateRefTexture( double x, double y, double r, IplImage* scrImage_gray, int target_ID );
		double MeasureTexture( float x, float y, float r, float theta, IplImage* scrImage_gray, int target_ID );
		double MeasureTexture( IplImage * src, IplImage * texture );
		double MeasureEllipseEdge( double x, double y, double r, double theta, IplImage *edgeImage, IplImage* src);
		double MeasureEllipseEdge1( const double x, const double y, const double r, const double theta, IplImage *edgeImage, IplImage* src );
		double MeasureEllipseEdge2( const double x, const double y, const double r, const double theta, IplImage *edgeImage, IplImage* src );
		double MeasureEllipseEdge4( double x, double y, double r, double theta, IplImage *edgeImage, IplImage* src, double & upperright, double & upperleft, double & downright, double & downleft, int cam_id);
		double MeasureLine( IplImage * edgeImage, CvPoint pt1, CvPoint pt2, double dx, double dy );
		double MeasureBoxEdge( IplImage * edgeImage, CvBox2D box, int length );
		double MeasureRectEdge( double x, double y, double r, double alpha, IplImage * edgeImage );
//		double Backproj(IplImage* scrImage_Cr, IplImage* scrImage_Cb, CvMat* add_X, CvMat* add_Y, CvMat* add_R, CvMat* add_Theta, IplImage* src, CvHistogram /*const*/ * cmp_hist, bool & detected, CvPoint prev_loc);
		double Backproj1(IplImage* scrImage_Cr, IplImage* scrImage_Cb, CvMat* add_X, CvMat* add_Y, CvMat* add_R, CvMat* add_Theta, IplImage* src, CvHistogram /*const*/ * cmp_hist, bool & detected, CvPoint prev_loc);

		//for big
		double Backproj2(IplImage* scrImage_Cr, IplImage* scrImage_Cb, CvMat* add_X, CvMat* add_Y, CvMat* add_R, CvMat* add_Theta, IplImage* src, CvHistogram /*const*/ * cmp_hist, bool & detected, CvPoint prev_loc);

		void prtPline(int r, int c, double angle, IplImage* img);
		bool ConnectedComponent(IplImage* backproj, CvMat*, CvMat*, CvMat*, CvMat*);
		bool ConnectedComponent1(IplImage* backproj, CvMat* mx, CvMat* my, CvMat* mr, CvMat* mtheta);

		//for big
		bool ConnectedComponent2(IplImage* backproj, CvMat* mx, CvMat* my, CvMat* mr, CvMat* mtheta);


		int getTexture_cnt() {return texture_count; }
		void setTexture_cnt(int val) {texture_count = val;}
		double getEllipse_ratio() {return ellipse_ratio;}
		double getRotatedX(double x0, double y0, int i);
		double getRotatedY(double x0, double y0, int i);
		double MAPgetRotatedX(double x0, double y0, int i);
		double MAPgetRotatedY(double x0, double y0, int i);
		//for ConnectedComponent
		double lastArea, lastScore;
		CvPoint lastLoc;


	private:
		double HomogenousRegion(int x_min, int x_max, int y_min, int y_max, CvHistogram * ref_hist, IplImage * scrImage_Cr, IplImage* scrImage_Cb );
		void BoundSelection( int &action, CvRect out_selection, CvRect in_selection );
		void CompareColorHist( double& distance, int action, CvHistogram * ref_hist, CvRect selection, IplImage* scrImage_Cr, IplImage* scrImage_Cb );
		void SampleEllipseContour();
		void ConstructSINCOSR();
		CvPoint interactionPoint( CvPoint pt1, float angle1, CvPoint pt2, float angle2 );

		CvMat * M_circleedgesamples;
public:
		IplImage * ref_texture[MAX_TARGET_NUM];
		IplImage * prev_ref_texture[MAX_TARGET_NUM];
private:
		IplImage * scale_img;
		CvHistogram * ref_hist, * current_hist;

		IplImage *colormask, *bottle, *gray;
		fstream fileOut3;
		IplImage *bp_img, *morphImage, *src_copy;
		IplImage *morph_half, *src_half, *temp_half, *sml_bkpj, *sml_CC_out, *sml_CC, *CC_out;
		IplConvKernel *ck, *erosion, *bigErosion, *smlErosion;
		CvMat *X_rand, *Y_rand, *R_rand, *Theta_rand;
//		CBlobResult blobs;
		double totalArea;
		CvConnectedComp track_comp;

		//for ConnectedComponent
		CvSeq* main_contour;
		CvMemStorage*	bp_storage ;
		CvSeq*	bp_contour ;
		//measure color with CvBox2D mask
		CvPoint2D32f pt[4];
		CvPoint* pt1;
		int npt1;

		double sqrt_2;
		int texture_count;
		double ellipse_ratio;

		double COS[circle_edge_sample_number];
		double SIN[circle_edge_sample_number];
		double MAP_COS[circle_edge_sample_number*10];
		double MAP_SIN[circle_edge_sample_number*10];

		int dist_arr[circle_edge_sample_number];
		

#ifdef FPS
	int fps_counter;
	double fps_1, fps_2, fps_3, fps_4, fps_5, fps_6;
	clock_t temp1, temp2;
#endif

};
