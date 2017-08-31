#ifndef __H_IMAGESOURCEWEBCAM__
#define __H_IMAGESOURCEWEBCAM__

#include "cvcam.h"
#include "cv.h"

class C_ImageSourceWebCam
{
private:
public:
	C_ImageSourceWebCam()
	{
		//sprintf(mType,"WebCam");
	}

	~C_ImageSourceWebCam();

	IplImage* grabImage();
	int cameras;


	void initialize( CvPoint grabSize );

};

#endif