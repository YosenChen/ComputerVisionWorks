#ifndef CAMSHIFTH
#define CAMSHIFTH

#include <iostream>
#include <ctime>
#include <windows.h>
#include <process.h> //_beginthreadex
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "highgui.h"
#include "likelihood.h"
#include "face_detect.h"
#include "TrackingResult.h"
#include "EnumType.h"
#include <vector>
#include "Component.h"
#include "Client.hpp"

#define purifyFactor 0.333333333//can see deeper!?
#define PI 3.14159265f
#define of_x 5.00
#define of_y 5.00
#define ROIdim 120.00
#define intepolation_m 3.00
#define intepolation_n 2.5
#define SatTh 45

//particle parameters
#define PARTICLE_NUM 100//150.00/*30.00*//*10.00*/ //需與SIS同步 stdafx.h同步
#define ADD_PARTICLE_NUM /*10.00*/25.00 //需與SIS同步 stdafx.h同步
//#define SIS_RATIO 0.5 //需與SIS、SIR同步 stdafx.h同步

#define MODEL_R 30.00  //需與model同步 stdafx.h同步
#define X_DEV 6.00//15.00/*6*/
#define Y_DEV 4.00//10.00/*6*/
#define R_DEV 0.05//20/MODEL_R
#define THETA_DEV 0.00//6.00//12.00//degree
#define LOW_THD 50.00
#define HIGH_THD 100.00
#define R_MAX 40.00 //需與stdafx.h同步
#define R_MIN 15.00  //需與stdafx.h同步
#define ELLIPSE_RATIO 1.3//2.5/*1.7*/  //需與stdafx.h同步
#define RANGE_MIN 0
#define RANGE_MAX 255
#define Cr_BINS 25
#define Cb_BINS 25
#define COLOR_WEIGHT 1.2
#define HAIR_WEIGHT 0.6
#define EDGE_WEIGHT 1.0
#define TEXTURE_WEIGHT 1.0
#define ANGLE_WEIGHT 0.3
#define SCALE_WEIGHT 0.3
#define POSX_WEIGHT 0.3
#define POSY_WEIGHT 0.3
#define OCCLUSION_CTS_FACTOR 1.5
#define JOINTDIST_WEIGHT 0.2
#define DETECT_NEW -10
#define NOT_DETECTED -20
#define TRACKED -1
#define FACETEST_WIDTH 70 //與face_detect.h 同步
#define FACETEST_HEIGHT 90 //與face_detect.h 同步
#define SKIN_COLOR_THD 70
#define SKIN_BLOB_AREA_THD 250
#define TEMPLATE_WIDTH 32
#define TEMPLATE_HEIGHT 32
#define JOINT_X_DIST 60
#define JOINT_Y_DIST 240

//Multi-target tracking
#define MAX_TARGET_NUM 5
#define MAX_GROUP_NUM 2

#define CAM1 0
#define CAM2 1
#define CAM3 2 


using namespace std;

class C_WinSocketClient;
class C_PM;
class VisionTranslate;

struct CamIterationReturn
{
	int val;
	CvPoint targetCenter;
	CvRect targetROI;
};
struct ParticleStatus
{
	double x, y, r, theta;
	int index;
};
struct EdgeScore
{
	double upperright, upperleft, downright, downleft, total;
};
//enum OVERLAP_DIRECTION
//{
//	RIGHT_LEFT,
//	LEFT_RIGHT,
//	UPPER_DOWN,
//	DOWN_UPPER,
//	UPPER_RIGHT,
//	UPPER_LEFT,
//	DOWN_RIGHT,
//	DOWN_LEFT
//};
class CAMShift
{
     private:
		 static const float START_STILL_SECOND_THRESHOLD;
		 //parameters of Particle Filter
		 CvRNG rng;
		 CvMat *X, *X_rand, *Y, *Y_rand, *R, *R_rand, *Theta, *Theta_rand;
		 CvMat *sub_X, *sub_Y, *sub_R, *sub_Theta, *sub_Weight;
		 CvMat *add_X, *add_Y, *add_R, *add_Theta, *add_X_rand, *add_Y_rand, *add_R_rand, *add_Theta_rand;
		 CvMat *add_new_X, *add_new_Y, *add_new_R, *add_new_Theta;
		 CvMat* Weight1, *Weight2, *Weight3, *Weight1_temp, *Weight2_temp;
		 CvMat *tmp_X, *tmp_Y, *tmp_R, *tmp_Theta, *tmp_add_X, *tmp_add_Y, *tmp_add_R, *tmp_add_Theta;
public:
		 likelihood* LH;
		 IPCClientMgr* IPCTrackingClient;
private:
		 face_detect* frontalFaceTest;
		 face_detect* profileFaceTest;
		 bool init_flag;
		 int iteration_cnt;
		 CvHistogram *refHist, *JointHist, *refHairHist;
		 //CvHistogram* targetBodyHist_CrCb[MAX_TARGET_NUM];
		 //CvHistogram* targetBodyHist_H[MAX_TARGET_NUM];
		 static const int NOM_FOR_INIT_STATE_USING_CAMSHIFT;
		 void normalizeWeight();
		 IplImage *test_img, *sample_img;
		 IplImage *sample_hsv, *sample_hue, *sample_sat, *sample_val;
		 IplImage *sample_YCrCb, *sample_Y, *sample_Cr, *sample_Cb;
		 IplImage *sample_img2, *sample_YCrCb2, *sample_Y2, *sample_Cr2, *sample_Cb2;
		 bool detect_flag;
	public:
		 IplImage* hist_mask;
		 CameraLine3D targetLines[MAX_TARGET_NUM];
		 CameraLine3D targetGndLines[MAX_TARGET_NUM];
		 Position3D targetPos[MAX_TARGET_NUM];
		 double targetHeight[MAX_TARGET_NUM];
		 int cam_id;


	public:
		 class Target
		 {
		 public:
			 int tarIndex;
			 int ptIndex1;
			 int ptIndex2;
			 int ptNum;
			 ParticleStatus tgMAP;
			 bool isAlive;
			 ParticleStatus init_state;
			 bool SIS_flag;
			 ParticleStatus preStatus[2];
			 Target* JointLeaves[MAX_TARGET_NUM];
			 Target* JointRoot;
			 CAMShift* MomSelf;
			 bool isInGroup;
			 Target():isAlive(false),tarIndex(-1),SIS_flag(false),isInGroup(false)
			 {
				 for (int h=0; h<MAX_TARGET_NUM; h++)
					 JointLeaves[h] = NULL;
				 JointRoot = NULL;
				 MomSelf = NULL;
			 }
			 Target(ParticleStatus init, int id)
			 {
				 tgMAP = init_state = init;
				 isAlive = true;
				 tarIndex = id;
				 SIS_flag = true;
				 preStatus[1]=preStatus[0]=init;
				 ptNum = (PARTICLE_NUM)/MAX_TARGET_NUM;
				 ptIndex1 = ptNum*id;
				 ptIndex2 = ptNum*(id+1);
				 for (int h=0; h<MAX_TARGET_NUM; h++)
					 JointLeaves[h] = NULL;
				 JointRoot = NULL;
				 MomSelf = NULL;
				 isInGroup = false;
			 }
			 Target(const Target & tg)
			 {
				 tarIndex = tg.tarIndex;
				 ptIndex1 = tg.ptIndex1;
				 ptIndex2 = tg.ptIndex2;
				 ptNum = tg.ptNum;
				 tgMAP = tg.tgMAP;
				 isAlive = tg.isAlive;
				 init_state = tg.init_state;
				 SIS_flag = tg.SIS_flag;
				 preStatus[1] = tg.preStatus[1];
				 preStatus[0] = tg.preStatus[0];
				 for (int h=0; h<MAX_TARGET_NUM; h++)
					 JointLeaves[h] = tg.JointLeaves[h];
				 JointRoot = tg.JointRoot;
				 MomSelf = tg.MomSelf;
				 isInGroup = tg.isInGroup;
			 }
			 Target & operator=(const Target & rhs)
			 {
				 //dtor is empty
				 tarIndex = rhs.tarIndex;
				 ptIndex1 = rhs.ptIndex1;
				 ptIndex2 = rhs.ptIndex2;
				 ptNum = rhs.ptNum;
				 tgMAP = rhs.tgMAP;
				 isAlive = rhs.isAlive;
				 init_state = rhs.init_state;
				 SIS_flag = rhs.SIS_flag;
				 preStatus[1] = rhs.preStatus[1];
				 preStatus[0] = rhs.preStatus[0];
				 for (int h=0; h<MAX_TARGET_NUM; h++)
					 JointLeaves[h] = rhs.JointLeaves[h];
				 JointRoot = rhs.JointRoot;
				 MomSelf = rhs.MomSelf;
				 isInGroup = rhs.isInGroup;

				 return *this;
			 }
			 ~Target(){}
			 void delTarget()
			 {
				 isAlive = false;
				 SIS_flag = false;
				 isInGroup = false;
				 //neighbor ptrs are destroyed outside
			 }

		 private:

		 };
		 EdgeScore edge4Segment[PARTICLE_NUM];

	private:
		 bool targetAllAlive(int k);
		 bool targetAllDead(int k);
		 int targeAliveNum(int k);
		 //YCrCb space
		 IplImage *image_YCrCb;
	public:
		 IplImage *image_Cr, *image_Cb, *image_Y;
	private:
		 IplImage *gray, *p_gray, *prev_gray, *image_B, *image_G, *image_R;

		 //for SobelEdge
		 IplImage *sobel, *temp_sobel, *graysmooth, *edgetemp, *dx, *dy, /**moving_gray,*/ *prev_moving_blob, *moving_blob;
		 IplImage *sml_gray, *sml_sobel, *s_temp_sobel, *sml_temp, *s_dx, *s_dy, *s_blob, *s_mv_blob, *sml_backproj, *sml_CntCmp;

		 //CvHistogram *hist;
		 /*static */fstream fileOut2;
		 int mLosingCount;
		 int mIterationCount;
		 int mFlagIteration[6];
		 static const int MAX_LOSING_COUNT;
		 static const int MAX_THRESHOLD;
		 static const int MIN_THRESHOLD;
		 static const int STANDARD_THRESHOLD;
		 int losingCnt;
		 int noMotionCnt;
		 int maxIt;
		 double targetSat;
		 int mMinArea;
		 int mMaxArea;
		 int ThreadNr;
		 double pr_mapX, pr_mapY, pr_mapR, pr_mapTheta, mapX, mapY, mapR;
	public:
		 Target targetList[MAX_TARGET_NUM];
		 BodyInfo targetInfo[MAX_TARGET_NUM];
	private:
		 bool MeasureIsDone[MAX_TARGET_NUM];
		 bool DepthOrderDone[MAX_TARGET_NUM];
		 Target* ptr[MAX_TARGET_NUM];
		 int targetLosingCnt[MAX_TARGET_NUM];
		 Target* order[MAX_GROUP_NUM][MAX_TARGET_NUM];
		 int leafctr[MAX_GROUP_NUM];
		 bool isBlockOcc[MAX_TARGET_NUM][NUM_OF_BLOCKS];
		 double mapTheta;
		 CvFont orderFont;
         		 
		int mPrePosX; 
		int mPrePosY;
		float mPreRot;
		//float mPreArea; 
		C_WinSocketClient *mClient;
		bool mStartFlag;
		float mStandardArea;
		//IplImage* morphImage;
		IplConvKernel *ck, *erosion, *bigErosion, *smlErosion, *connectionElement, *CAM1_detect_dilation;
		double mTimeDiff;

		//SampleStatus mPreSampleStatus;
		//bool mCanDetectBreak;
		//bool mIsBreak;
		string *mPresentedNormalString;
        string *mPresentedControlString;

		static CAMShift* mSingletonInstancePtr;
		CamIterationReturn setupReturn(CvPoint &ROIcenter, int returnValue);

		 CAMShift( /*C_PM* cPM,*/ int iWidth=320, int iHeight=240, int cam_index=NONDEFINED );
		 ~CAMShift();

		//void resetTranslateParameter();
		//void feedSamplesIntoPMBreak(float posX, 
		//							 float posY, 
		//							 float angle, 
		//							 clock_t& startStillTimeStamp, 
		//							 clock_t& startMoveTimeStamp );
		// void feedSampleIntoPM(float posX, float posY, float angle);

		 int reInterpret( float angle ) const;

     public:
		 //CAMShift( string &presentedNormalString, string &presentedControlString );
         
		 CvBox2D getTrack_box() {return track_box;}
		 IplImage *backproject, *histimg, *hsv, *hue, *sat, *val, *mask, *crtMask, *crtHistimg, *half_backproject, *safeMask, *ROImask,
			 *edgeResponse, *extend_mask;
		 IplImage* histimg_H[MAX_TARGET_NUM];
		 IplImage *purifiedHistimg;
		 CvRect track_window;
		 CvBox2D track_box;
		 CvConnectedComp track_comp;
		 int vmin, vmax, smin, width, height, hdims;
		 CvHistogram *hist, *crtHist;
		 C_PM *mPM;
		 CvSeq* detect_contour;
		 CvMemStorage* detect_storage;
		 vector <CvSeq*> search_contours;
		 int PrevJointCnt;
		 int efficientSeg();

		 void initCamShiftChecking(IplImage * image, IplImage * fgMask, CvRect target );
		 bool initCamShiftChecking1(IplImage * image, IplImage * fgMask, CvRect target );
		 void init( C_WinSocketClient *client, string *presentedNormalString, string *presentedControlString );
		 void reInit();
		 CvScalar hsv2rgb( float );
		 void sendControlSignal( double timeDiff, SENDING_MODE mode );
		 void sendControlSignalExtracted(int angle, bool &pShouldReturn);
		 ParticleStatus detectObject(IplImage * image);
		 ParticleStatus detectObject_importanceSampling(IplImage * image);


		 //functions of SIR Particle Filter
		 void Predict();
		 void JointStatusCheck();
		 bool JointStatusChange();
		 void DepthOrderCheck();
		 void BodyOccCheck();
		 /*ParticleStatus*/void Measure( IplImage * Cr, IplImage * Cb, IplImage* src/*, double prev_angle, double prev_scale*/);
		 /*ParticleStatus*/void Measure1( IplImage * Cr, IplImage * Cb, IplImage* src/*, double prev_angle, double prev_scale*/);
		 double JointColorMeasure(IplImage* src_Cr, IplImage* src_Cb, int leaf_cnt, Target* self_ptr, int t);
		 double JointEdgeMeasure(IplImage* src, IplImage* sb, int leaf_cnt, Target* self_ptr, int t);
		 double JointAngleCts(int leaf_cnt, Target* self_ptr, int t);
		 double JointScaleCts(int leaf_cnt, Target* self_ptr, int t);
		 double JointPosXCts(int leaf_cnt, Target* self_ptr, int t);
		 double JointPosYCts(int leaf_cnt, Target* self_ptr, int t);
		 double JointDistMAX(int leaf_cnt, Target* self_ptr, int t);
		 void JointSetWeight(double weight, int leaf_cnt, Target* self_ptr, int t);

		 void Resampling();
		 void Resampling1();
		 void Resampling(CvBox2D init_box);
		 void Resampling(Target tg);
		 ParticleStatus MAP();
		 bool getPF_Init_flag() {return init_flag;}
		 void SobelEdge();

		 //for big
		 void SobelEdge1();
		 ////for big
		 void DrawResult(/*ParticleStatus ps, */IplImage *image);
		 void DrawIntegratedObservation(int obs_Id, IplImage* image, int TargetID, int level);
		 void DrawSingleObservation(int obs_Id, IplImage* image);
		 void setUpSupportingClass(IplImage *image);
		 void drawModelPattern(IplImage * src, double x, double y, double r, double theta, CvScalar color, int thickness);
		 void drawMAPModelPattern(IplImage * src, double x, double y, double r, double theta, CvScalar color, int thickness);
		 void reverseAngleSelector(ParticleStatus ps);
		 //functions of SIR/SIS Particle Filter

		 //function for Multi-target tracking
		 void deleteTarget(int index);
		 void losingTrackDetector(int h, IplImage* image);
		 void losingTrackDetectorByFaceDetector(int h, IplImage* image);
		 bool faceDetector(int x, int y, IplImage* image);

		 CamIterationReturn iteration_init(IplImage *image, IplImage* &fgmask, CvRect ROI);
		 CamIterationReturn iteration(IplImage *, IplImage * &, CvRect ROI );
		 int getRotAng(float agl);
		 int getPosX(int x);
		 int getPosY(int y);
		 int getScale(double original, double current);
		 CvScalar scaleColor(const int scale);
		 void addThread( void );
		 unsigned parallelImageProcess( void *pMyID );


		static CAMShift* getSingletonPtr( /*C_PM* cPM=0,*/ int width=320, int height=240, int camNo=CAM1  )
		{
			//fileOut2.open("error report2.txt",ios::out);

			if( mSingletonInstancePtr == 0 )
			{
				mSingletonInstancePtr = new CAMShift( /*cPM,*/ width, height, camNo );
			}

			return mSingletonInstancePtr;
		}


		static void	destroyInstance()
		{  
			delete mSingletonInstancePtr;
			mSingletonInstancePtr=0;  
		}  
};

#endif