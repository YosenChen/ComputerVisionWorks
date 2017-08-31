#pragma once

//#include <blob.h>
//#include <BlobResult.h>

//------------------------- back project-------- -------------------------//
//  使用 YCrCb color space 計算 Cr & Cb 之 color histogram
//  建立參考 color histogram 和目前影像的 Cr & Cb image 的相似程度
//  在setColorMap中輸入參考的顏色
//
//  使用順序:
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