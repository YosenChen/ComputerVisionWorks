#include "face_detect.h"

face_detect::face_detect(int a, int cam_id)
{
	fileOut4.open("error report4.txt",ios::out);

	cvNamedWindow("Reference Histogram",1);
	if (cam_id == CAM1)
		if(!a)
			//cascade_name = "C:\\Program Files\\OpenCV\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
			cascade_name = ".\\haarcascades\\haarcascade_frontalface_alt.xml";
		else 
			//cascade_name = "C:\\Program Files\\OpenCV\\data\\haarcascades\\haarcascade_profileface.xml";
			cascade_name = ".\\haarcascades\\haarcascade_profileface.xml";
	else //cam_id == CAM2
		if(!a)
			//cascade_name = "C:\\Program Files\\OpenCV\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
			cascade_name = ".\\haarcascades\\haarcascade_frontalface_alt_COPY.xml";
		else 
			//cascade_name = "C:\\Program Files\\OpenCV\\data\\haarcascades\\haarcascade_profileface.xml";
			cascade_name = ".\\haarcascades\\haarcascade_profileface_COPY.xml";


	loadCascadeOk = true;
	cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
	if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
		loadCascadeOk = false;
    }

	storage = cvCreateMemStorage(0);
	detect = false;

	int hdims = 16;
	float hranges_arr[] = {0,180};
    float* hranges = hranges_arr;
	ref_hist = cvCreateHist(1,&hdims,CV_HIST_ARRAY, &hranges, 1);
	detected_hist = cvCreateHist(1,&hdims,CV_HIST_ARRAY, &hranges, 1);


	detected_img = cvCreateImage(cvSize(FACETEST_WIDTH,FACETEST_HEIGHT),8,3);
	detected_hsv = cvCreateImage(cvSize(FACETEST_WIDTH,FACETEST_HEIGHT),8,3);
	detected_hue = cvCreateImage(cvSize(FACETEST_WIDTH,FACETEST_HEIGHT),8,1);

	torso_color = cvCreateHist(1,&hdims,CV_HIST_ARRAY, &hranges, 1);
	IplImage* skin = 0, *hsv = 0, *hue = 0;
	IplImage* histimg = cvCreateImage(cvSize(320,200),8,3);
	if (cam_id == CAM1)
	{
		fileOut4 << "fd green CAM1 = " << cam_id << endl;
		skin = cvLoadImage("green_CAM1.bmp"/*"Skin.jpg"*/,1);
	}
	else //cam_id == CAM2
	{
		fileOut4 << "fd green CAM2 = " << cam_id << endl;
		skin = cvLoadImage("green_CAM2.bmp"/*"Skin.jpg"*/,1);
	}
	hsv = cvCreateImage(cvGetSize(skin),8,3);
	hue = cvCreateImage(cvGetSize(skin),8,1);
	cvCvtColor(skin,hsv,CV_BGR2HSV);
	cvSplit(hsv,hue,0,0,0);
	cvCalcHist(&hue,ref_hist,0,0);
    float max_val = 0.0f;
	cvZero(histimg);
	cvGetMinMaxHistValue( ref_hist, 0, &max_val, 0, 0 );
    cvConvertScale( ref_hist->bins, ref_hist->bins, max_val ? 255. / max_val : 0., 0 );
	int bin_w = histimg->width / hdims;
    for( int i = 0; i < hdims; i++ )
    {
		int val = cvRound( cvGetReal1D(ref_hist->bins,i)*histimg->height/255 );
        CvScalar color = hsv2rgb(i*180.f/hdims);
        cvRectangle( histimg, cvPoint(i*bin_w,histimg->height),
                     cvPoint((i+1)*bin_w,histimg->height - val),
                     color, -1, 8, 0 );
	}
	cvShowImage("Reference Histogram",histimg);

    cvNormalizeHist(ref_hist,1);
	cvReleaseImage(&skin);
	cvReleaseImage(&hsv);
	cvReleaseImage(&hue);
	cvReleaseImage(&histimg);
}
bool face_detect::detect_and_draw(IplImage *image, int j, int cam_id)
{
	//fileOut4 << "detect_and_draw CAM ID = " << cam_id << endl;
	float max_val = 0.f;
	int size = 20;
	if(j < 0)
	{
		size = 10;
	}
	double dist = 0;

	cvResize(image, detected_img);
	if(image->origin == IPL_ORIGIN_TL)
		;
	else
		cvFlip(detected_img, detected_img, 0);

    int scale = 1;

	cvCvtColor(detected_img,detected_hsv,CV_BGR2HSV);
	cvSplit(detected_hsv,detected_hue,0,0,0);


    bool result = false;
    CvPoint pt1, pt2;
    int i,maxi,temparea,facex,facey,facewidth,faceheight;
	CvRect temprect;
	maxi = -1;

	CvSeq* first_face = NULL;

    cvClearMemStorage( storage );
	cvNamedWindow("face_detect",1);

    if( cascade )
    {
        first_face = faces = cvHaarDetectObjects( detected_img, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(size, size) );
		temparea = 0;
        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
            facetemp = (CvRect*)cvGetSeqElem( faces, i );
            pt1.x = facetemp->x*scale;
            pt2.x = (facetemp->x+facetemp->width)*scale;
            pt1.y = facetemp->y*scale;
            pt2.y = (facetemp->y+facetemp->height)*scale;
			if(facetemp->width * facetemp->height >= temparea /*&& facetemp->width*facetemp->height < 1600*/)
			{
				temparea = facetemp->width * facetemp->height;
				maxi = i;
			}
        }
		if(maxi >=0 && temparea>2000 && cam_id == CAM1)
		{
			fileOut4 << "detect_and_draw CAM1 = " << cam_id << endl;
			facetemp = (CvRect*)cvGetSeqElem( faces, maxi );
			face_full = *facetemp;
			pt1.x = facetemp->x*scale+facetemp->width/5;
            pt2.x = (facetemp->x+facetemp->width)*scale-facetemp->width/5;
			pt1.y = facetemp->y*scale+facetemp->height/2;
            pt2.y = (facetemp->y+facetemp->height)*scale-facetemp->height/5;
			cvRectangle( detected_img, pt1, pt2, CV_RGB(255,0,0), 1, 8, 0 );
			facex=facetemp->x+facetemp->width/5;
			facey=facetemp->y*scale+facetemp->height/2;
			facewidth = 4*facetemp->width/5;
			faceheight = 3*facetemp->height/10;
			temprect.x = facex;
			temprect.y = facey;
			temprect.width=facewidth;
			temprect.height=faceheight;
			face_col = temprect;
			detect = result = true;
			cvRectangle( detected_img, pt1, pt2, CV_RGB(255,0,0), 1, 8, 0 );
			pt1.x = facetemp->x;
			pt1.y = facetemp->y;
			pt2.x = facetemp->x+facetemp->width;
			pt2.y = facetemp->y+facetemp->height;
			face_pos.x = (pt1.x+pt2.x)/2;
			face_pos.y = (pt1.y+pt2.y)/2;
			face.x = (pt1.x+pt2.x)/2-20;
			face.y = (pt1.y+pt2.y)/2-20;
			face.width = 24;
			face.height = 30;
			if(j>=0)
			{
				cvSetImageROI(detected_hue,face_col);
			    cvCalcHist(&detected_hue,detected_hist,0,0);
			    cvNormalizeHist(detected_hist,1);
			    dist = cvCompareHist(detected_hist,ref_hist,CV_COMP_BHATTACHARYYA);
				if(dist < 0.5)
				{
					detect = true;
					result = true;
				}

				//I add 20100105
				else
					detect = result = false;
			}
			cvRectangle(detected_img,pt1,pt2,CV_RGB(0,0,255),1,8,0);
			
			//////I add
			////if (abs((pt1.x+pt2.x)/2 - image->width/2) > image->width/4 || abs((pt1.y+pt2.y)/2 - image->height/2) > image->height/4)
			////	detect = result = false;
		}
		else if(maxi >=0 && cam_id == CAM2)//This is for CAM2 setting
		{
			fileOut4 << "detect_and_draw CAM2 = " << cam_id << endl;
			facetemp = (CvRect*)cvGetSeqElem( faces, maxi );
			face_full = *facetemp;
			pt1.x = facetemp->x*scale+facetemp->width/5;
            pt2.x = (facetemp->x+facetemp->width)*scale-facetemp->width/5;
			pt1.y = facetemp->y*scale+facetemp->height/2;
            pt2.y = (facetemp->y+facetemp->height)*scale-facetemp->height/5;
			cvRectangle( detected_img, pt1, pt2, CV_RGB(255,0,0), 1, 8, 0 );
			facex=facetemp->x+facetemp->width/5;
			facey=facetemp->y*scale+facetemp->height/2;
			facewidth = 4*facetemp->width/5;
			faceheight = 3*facetemp->height/10;
			temprect.x = facex;
			temprect.y = facey;
			temprect.width=facewidth;
			temprect.height=faceheight;
			face_col = temprect;
			detect = result = true;
			cvRectangle( detected_img, pt1, pt2, CV_RGB(255,0,0), 1, 8, 0 );
			pt1.x = facetemp->x;
			pt1.y = facetemp->y;
			pt2.x = facetemp->x+facetemp->width;
			pt2.y = facetemp->y+facetemp->height;
			face_pos.x = (pt1.x+pt2.x)/2;
			face_pos.y = (pt1.y+pt2.y)/2;
			face.x = (pt1.x+pt2.x)/2-20;
			face.y = (pt1.y+pt2.y)/2-20;
			face.width = 24;
			face.height = 30;
			if(j>=0)
			{
				cvSetImageROI(detected_hue,face_col);
			    cvCalcHist(&detected_hue,detected_hist,0,0);
			    cvNormalizeHist(detected_hist,1);
			    dist = cvCompareHist(detected_hist,ref_hist,CV_COMP_BHATTACHARYYA);
				if(dist < 0.5)
				{
					detect = true;
					result = true;
				}

				//I add 20100105
				else
					detect = result = false;
			}
			cvRectangle(detected_img,pt1,pt2,CV_RGB(0,0,255),1,8,0);
			
			//////I add
			////if (abs((pt1.x+pt2.x)/2 - image->width/2) > image->width/4 || abs((pt1.y+pt2.y)/2 - image->height/2) > image->height/4)
			////	detect = result = false;
		}
		else
		{
			detect = false;
			result = false;
		}
		/*
		if(result == true)
		{
			torso.x = face_full.x;
			torso.width = torso.x +  face_full.width < image->width ?  face_full.width : image->width - torso.x;
			torso.y = face_full.y+ 3* face_full.height/2 < image->height ?  face_full.y+ 3* face_full.height/2 : image->height;
			torso.height = torso.y + face_full.height/3 < image->height ? face_full.height/3 : image->height-torso.y;
			if(torso.height)
			{
				cvSetImageROI(hue,torso);
			    cvNormalizeHist(torso_color,1);
			    cvResetImageROI(hue);
			    pt1.x = torso.x; pt1.y = torso.y;
			    pt2.x = torso.x + torso.width; pt2.y = torso.y+torso.height;
			    cvRectangle(img,pt1,pt2,CV_RGB(128,0,128),1,8,0);
			}
		}
		else torso_color = NULL;
		*/
		torso_color = NULL;
	
    }

	if (detect)	cvShowImage("face_detect",detected_img);
	//cvReleaseData(&faces);

	for (; first_face->h_next != NULL; ) 
	{
		CvSeq* current = first_face;
		first_face = first_face->h_next;
		cvClearSeq(current);
	}
	cvClearSeq(first_face);	

	return result;
}
CvScalar face_detect::hsv2rgb( float hue )
{
    int rgb[3], p, sector;
    static const int sector_data[][3]=
        {{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
    hue *= 0.033333333333333333333333333333333f;
    sector = cvFloor(hue);
    p = cvRound(255*(hue - sector));
    p ^= sector & 1 ? 255 : 0;

    rgb[sector_data[sector][0]] = 255;
    rgb[sector_data[sector][1]] = 0;
    rgb[sector_data[sector][2]] = p;

    return cvScalar(rgb[2], rgb[1], rgb[0],0);
}
face_detect::~face_detect()
{
	cvReleaseImage(&detected_hsv);
	cvReleaseImage(&detected_img);
	cvReleaseImage(&detected_hue);
	cvReleaseHist(&detected_hist);
	cvReleaseHist(&ref_hist);
}