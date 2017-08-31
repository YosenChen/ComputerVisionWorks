#pragma once

//#include <blob.h>
//#include <BlobResult.h>

//------------------------- back project-------- -------------------------//
//  �ϥ� YCrCb color space �p�� Cr & Cb �� color histogram
//  �إ߰Ѧ� color histogram �M�ثe�v���� Cr & Cb image ���ۦ��{��
//  �bsetColorMap����J�ѦҪ��C��
//
//  �ϥζ���:
//  	setHistogram();
//		setColorMap( char* filename );
//		backProject( src, dst );
//
//------------------------------------------------------------------------//
class BackProj
{
public:
	BackProj();
	~BackProj();
	void setColorMap( char * filename, CvHistogram * set_hist );
	void backProject( IplImage * Cr, IplImage * Cb, IplImage * dst, CvHistogram * cmp_hist );
	void drawParticles( IplImage * src, CvMat * x, CvMat * y, CvMat * r );
	void UpdateHist( IplImage * Cr, IplImage * Cb, CvRect ROI_rect, CvHistogram * update_hist );

private:
	IplConvKernel * ck;

	CBlobResult blobs;
	double totalArea;
};