#include "../ClassOpenCVMat/ClassOpenCVMat.h"
#include "highgui.h"

class ClassVideoWriter
{
public:
	ClassVideoWriter();
	~ClassVideoWriter();

	IplImage* mpImage;


	void   mInitialize(int iWidth,int iHeight,int iNumChannel,char *strFileName,int iFps);
	void   mWriteColorBufferToVideo(CvMat *pMat);
	void   mWriteImageToVideo();

private:

	CvVideoWriter *mpVideoWriter;

};