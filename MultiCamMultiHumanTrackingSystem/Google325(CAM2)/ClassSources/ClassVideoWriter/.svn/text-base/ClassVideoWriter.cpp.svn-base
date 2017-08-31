#include "ClassVideoWriter.h"

ClassVideoWriter::ClassVideoWriter()
{
	mpImage=NULL;
	mpVideoWriter=NULL;
}

void ClassVideoWriter::mInitialize(int iWidth,int iHeight,int iNumChannel,char *strFileName,int iFps)
{
	mpImage=cvCreateImage( cvSize(iWidth,iHeight), 8, iNumChannel );
	mpVideoWriter = cvCreateVideoWriter( strFileName, -1, iFps, cvSize(iWidth,iHeight));
}

ClassVideoWriter::~ClassVideoWriter()
{
	if(mpImage)
		cvReleaseImage(&mpImage);

	if(mpVideoWriter)
		cvReleaseVideoWriter( &mpVideoWriter );
}


void ClassVideoWriter::mWriteColorBufferToVideo(CvMat *pMat)
{

	cvCopy(pMat,mpImage);

	cvWriteFrame( mpVideoWriter,mpImage );
}

void ClassVideoWriter::mWriteImageToVideo()
{
	cvWriteFrame( mpVideoWriter,mpImage );
}