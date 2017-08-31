//05/10/19
#include "ClassOpenCVMat.h"

ClassOpenCVMat::ClassOpenCVMat()
{
	mpMat = 0;
	mpUCArray = 0;
}
ClassOpenCVMat::~ClassOpenCVMat()
{
	delete [] mpUCArray;
}

void ClassOpenCVMat::mCreateUCMat(int iWidth,int iHeight)
{
	if( mpUCArray == 0 )
	{
		mpUCArray = new unsigned char[iWidth*iHeight];
		mpMat = cvInitMatHeader(&mMat,iHeight,iWidth,CV_8UC1,mpUCArray);

		miWidth = iWidth;
		miHeight = iHeight;
	}
}


void ClassOpenCVMat::mCreateRGBUCMat(int iWidth,int iHeight)
{
	if( mpUCArray == 0 )
	{
		mpUCArray = new unsigned char[iWidth*iHeight*3];
		mpMat = cvInitMatHeader(&mMat,iHeight,iWidth,CV_8UC3,mpUCArray);
		miWidth = iWidth;
		miHeight = iHeight;
	}
}