//06/03/23
#pragma once
#include "cv.h"

class ClassOpenCVMat
{
public:
	ClassOpenCVMat();
	~ClassOpenCVMat();

	CvMat          mMat;
	CvMat          *mpMat;
	unsigned char  *mpUCArray;
	int            miWidth;
	int            miHeight;

	void    mCreateUCMat(int iWidth,int iHeight);
	void    mCreateRGBUCMat(int iWidth,int iHeight);
};