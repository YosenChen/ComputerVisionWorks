//---------------------------------------------------------------------------
#ifndef TargetDetectH
#define TargetDetectH
//---------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

class TargetDetect{
private:
	IplImage * srcImage, * bgImage, * fgMask;
	CvHistogram * skinHist1, * skinHist2;
	IplImage * hsv, * hue, * mmask, * backproject, * backproject2;

	int vmin, vmax, smin;

	int targetnum;
	CvRect * boundBox;

public:
	TargetDetect(IplImage * = 0, IplImage * = 0, IplImage * = 0, CvRect * = 0, int = 0);
	~TargetDetect();

	void ForegroundSegment( IplImage *, IplImage *, IplImage * );				//�����e������
	void RemoveSkinColor( IplImage *, IplImage * );
	void ConnectedComponent( IplImage * );	//��X�ŦX�j�p��connected component

	void CalcHistogram( IplImage * , CvHistogram * );
	void ShowHistogram( CvHistogram * );
	CvScalar hsv2rgb( float );

	CvRect motionBox;
	int motionNZero, boundNZero;

};
#endif
 