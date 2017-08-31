#include <iostream>
using namespace std;
#include <iomanip>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "highgui.h"

#define FACETEST_WIDTH 70 //팒CAMShift.h 쪷쭮
#define FACETEST_HEIGHT 90 //팒CAMShift.h 쪷쭮

#define CAM1 0 //팒CAMShift.h쪷쭮
#define CAM2 1 //팒CAMShift.h쪷쭮
#define CAM3 2 //팒CAMShift.h쪷쭮

class face_detect
{
public:
	face_detect(int a, int cam_id);
	~face_detect();
	char* cascade_name;
	CvHaarClassifierCascade* cascade;
	CvHistogram* ref_hist, *torso_color;
	CvHistogram *detected_hist;
	IplImage *detected_img, *detected_hsv, *detected_hue;
	bool detect_and_draw(IplImage *img, int j, int cam_id);
	CvRect face_col,face, face_full, torso;
	bool loadCascadeOk;
	bool detect;
	CvPoint face_pos;
    CvRect *facetemp;	CvSeq* faces;
	CvScalar hsv2rgb(float hue);
private:
	CvMemStorage* storage;
	fstream fileOut4;
};