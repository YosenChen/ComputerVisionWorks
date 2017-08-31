#ifndef COMPONENT_H
#define COMPONENT_H


#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
using namespace std;

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvcam.h>
#include <cmath>
#include <stdlib.h>

#define CAM1 0 //與CAMShift.h同步
#define CAM2 1 //與CAMShift.h同步
#define CAM3 2 //與CAMShift.h同步

#define CAM1_OPX 0.0
#define CAM1_OPY 0.0
#define CAM1_OPZ 220.0
#define CAM1_OP_LENGTH 10.0
#define CAM1_H2W_RATIO1 0.8889//0.6//0.8889//0.4
#define CAM1_H2W_RATIO2 1.0/*1.15*/

//#define CAM2_OPX 3.0
//#define CAM2_OPY 0.0
//#define CAM2_OPZ 3.0
//#define CAM2_OP_LENGTH 60
#define CAM1_FX 632.7966370963406500
#define CAM1_FY 648.8801540650551900
#define CAM1_CX 154.7436815289115100
#define CAM1_CY 111.9921779795692500
#define CAM1_K1 -0.0835197107806061
#define CAM1_K2 1.5506426574624039
#define CAM1_P1 2.7437800371015284e-003
#define CAM1_P2 3.0427032194704523e-004

#define CAM2_FX 439.6556325176259700
#define CAM2_FY 400.5645360515742900
#define CAM2_CX 154.9494781634536800
#define CAM2_CY 117.3807447017589300
#define CAM2_K1 -0.2713607631660430
#define CAM2_K2 0.0884194681618005
#define CAM2_P1 3.9067638751436613e-003
#define CAM2_P2 -1.2213417286338105e-003

#define CAM1_2FX 633.9391161899213800
#define CAM1_2FY 649.3799343246996600
#define CAM1_2CX 148.0804081387016700
#define CAM1_2CY 108.1521017502147400
#define CAM1_2K1 -0.0117709112642267
#define CAM1_2K2 0.2219732951548829
#define CAM1_2P1 -1.5453687347612274e-004
#define CAM1_2P2 -4.2426658093814972e-003

#define CAM2_2FX 439.5910733679539800
#define CAM2_2FY 400.3470267536740700
#define CAM2_2CX 158.1101134866712000
#define CAM2_2CY 117.3758459352682300
#define CAM2_2K1 0.0543967334434501
#define CAM2_2K2 -0.3731346091107116
#define CAM2_2P1 5.7302155880894297e-004
#define CAM2_2P2 2.2379046850979399e-003

#define CAM1_H2W_RATIO_AFTER_CALIB 1.15
#define CAM2_H2W_RATIO_AFTER_CALIB 1.1

#define IMG_WIDTH 320.0
#define IMG_HEIGHT 240.0

#define NONDEFINED -10000
#define SINGULAR_THD 0.1
#define IDENTICAL_THD 5.0
#define LARGE_IDENTICAL_THD 35//25.0
#define FACE_WIDTH 10.0
#define FACE_HEIGHT 10.0

#define NUM_OF_BLOCKS 4
#define DEFAULT_HEIGHT 155.0//150.0
#define BLOCK_WIDTH 40.0//20.0
#define TRANS_STRIPE_WIDTH 80.0

#define RANGE_MIN 0
#define RANGE_MAX 255
#define Cr_BINS 25
#define Cb_BINS 25

#define MAX_TARGET_NUM 5
//#define MAX_TARGET_NUM 6

enum RETURN_STATUS
{
	RETURN_DONE_NO_ERROR,
	RETURN_CONTRADICT,
	RETURN_ALREADY_DONE
};
//enum OBJ_CONFIDENCE
//{
//	OBJ_HIGH_CONF,
//	OBJ_MID_CONF,
//	OBJ_LOW_CONF,
//	OBJ_NON_DEFINED,
//	OBJ_HIGH_BLOCK
//};
//enum TARGET_CONFIDENCE
//{
//	TG_HIGH_CONF,
//	TG_MID_CONF,
//	TG_LOW_CONF,
//	TG_NON_DEFINED
//};

class Position3D
{
public:
	double x, y, z;
	Position3D(){}
	Position3D(const Position3D & pt){x = pt.x; y = pt.y; z = pt.z;}
	Position3D & operator=(const Position3D & rhs){x = rhs.x; y = rhs.y; z = rhs.z; return *this;}
	~Position3D(){}
	Position3D(double a, double b, double c){x = a; y = b; z = c;}
	Position3D operator+(const Position3D & rhs){Position3D result; result.x = Position3D::x+rhs.x; result.y = Position3D::y+rhs.y; result.z = Position3D::z+rhs.z; return result;}
	Position3D operator-(const Position3D & rhs){Position3D result; result.x = Position3D::x-rhs.x; result.y = Position3D::y-rhs.y; result.z = Position3D::z-rhs.z; return result;}
	bool operator==(const Position3D & rhs){return (x == rhs.x)&&(y == rhs.y)&&(z == rhs.z); }

};
class Pair
{
public:
	int i, j;
	Pair *next, *prev;

	Pair(){i = j = -1; prev = next = NULL;}
	Pair(const Pair & pr){i = pr.i; j = pr.j; next = pr.next; prev = pr.prev;}
	Pair & operator=(const Pair & rhs){i = rhs.i; j = rhs.j; next = rhs.next; prev = rhs.prev; return *this;}
	bool operator==(const Pair & rhs){return (i == rhs.i)&&(j == rhs.j)&&(next == rhs.next)&&(prev == rhs.prev);}
//	bool operator>(const Pair & rhs){return (i>rhs.i) ? true : (j>rhs.j) ? true : false ;}
//	bool operator<(const Pair & rhs){return (i<rhs.i) ? true : (j<rhs.j) ? true : false ;}
	~Pair(){}
	Pair(int ii, int ij, Pair* inext, Pair* iprev){i = ii; j = ij; next = inext; prev = iprev;}
	

};

#define MAGNITUDE(vec) sqrt(pow(vec.x, 2)+pow(vec.y, 2)+pow(vec.z, 2))

//helper function
Pair makePair(int ii, int ij, Pair* inext, Pair* iprev);
Position3D position(double x, double y, double z);
Position3D unit_vector(Position3D vec);
Position3D crossProduct(Position3D vec1, Position3D vec2);
double innerProduct(Position3D vec1, Position3D vec2);
double returnDegAngle(Position3D vec1, Position3D vec2);
Position3D inverseVector(Position3D vec);

class Plane
{
public:
	Plane();
	Plane(Position3D a, Position3D vec);
	~Plane();
	Plane(const Plane & plane);
	Plane & operator=(const Plane & rhs);

	//initialize
	bool initParamByPtVec(Position3D a, Position3D vec);

	//functionality
	//bool getPosFromXY(double x, double y, Position3D & pt);
	//bool getPosFromYZ(double y, double z, Position3D & pt);
	//bool getPosFromXZ(double x, double z, Position3D & pt);
	bool isPtOnPlane(Position3D pt) const;
	double getDisFromPt(Position3D pt) const;
	Position3D getProjPtFromPt(Position3D pt, double & dist) const;

public:
	bool IsInit;
	Position3D nor_dir_vec;
	Position3D Stdpoint;
};

class Line3D
{
public:
	Line3D();
	Line3D(Position3D a, Position3D b, bool dont_care);
	Line3D(Position3D a, Position3D vec);
	~Line3D();
	Line3D(const Line3D & line);
	Line3D & operator=(const Line3D & rhs);

	//initialize
	bool initParamBy2Pt(Position3D a, Position3D b);
	bool initParamByPtVec(Position3D a, Position3D vec);
	
	//functionality
	bool getPosFromX(double x, Position3D & pt) const;
	bool getPosFromY(double y, Position3D & pt) const;
	bool getPosFromZ(double z, Position3D & pt) const;
	bool isPtOnLine(Position3D pt) const;
	double getDisFromPt(Position3D pt) const;
	Position3D getProjPtFromPt(Position3D outerPt, double & dist) const;
	bool getLinePtOnPlane(Plane plane, Position3D & intersect) const;
	Position3D getMinDisPtFromLine(bool & valid, const Line3D line, Position3D & proj_pt) const;

public:
	Position3D unit_dir_vec;
	Position3D Stdpoint1;
	Position3D Stdpoint2;
	bool IsInit;
};


class CameraLine3D : public Line3D
{
public:
	Position3D optic_center;
	Position3D ref_plane_center;
	double ref_plane_dist;
	Position3D unit_orth_up_vec;
	Position3D unit_orth_right_vec;
	double cm2pixelRatio;
	double h2wRatio;
	CvPoint imageLoc;
	Position3D central_dir_vec;
	Plane refPlane;
	int camID;
	
	CameraLine3D();
	~CameraLine3D();
	CameraLine3D(const CameraLine3D & camline);
	CameraLine3D & operator=(const CameraLine3D & rhs);

	//initialize
	void initParamByImageLoc(CvPoint img_loc, Position3D op_ctr, Position3D refPlaneCtr, double c2p_ratio);
	void initParamByImageLoc_precise(CvPoint img_loc, Position3D op_ctr, Position3D refPlaneCtr, double c2p_ratio, int cam_id);
	void initParamBy3DPt(Position3D pt, Position3D op_ctr, Position3D refPlaneCtr, double c2p_ratio);
	void initParamBy3DPt_precise(Position3D pt, Position3D op_ctr, Position3D refPlaneCtr, double c2p_ratio, int cam_id);
	void setByImgLoc(CvPoint img_loc);
	void setByImgLoc_precise(CvPoint img_loc);
	void setBy3DPt(Position3D pt);
	void setBy3DPt_precise(Position3D pt);

public:
	
	
};

class BodyInfo
{
public:
	double height;
	CvHistogram *face;
	CvHistogram* body[NUM_OF_BLOCKS];
	CvRect Blocks[NUM_OF_BLOCKS];
	bool IsHeightKnown;
	bool IsBodySeen[NUM_OF_BLOCKS];
	

	BodyInfo();
	~BodyInfo();
	BodyInfo(const BodyInfo & bi);
	BodyInfo & operator=(const BodyInfo & rhs);

	//set block locatons
	bool LocateBlocksFromImage(const CameraLine3D * headline, CameraLine3D & groundline, bool h_is_got, double h = -10.0);
	bool LocateBlocksFrom3D(const CameraLine3D * headline, CameraLine3D & groundline, bool h_is_got, double h = -10.0);
	
	//locate blocks & measure info
	bool LocateAndMeasureInfo(const CameraLine3D * headline, CameraLine3D & groundline, bool h_is_got, double h, IplImage* src_Cr, IplImage* src_Cb, IplImage* msk);

	

	////update
	//void UpdateFaceHist(CvHistogram *fh);
	//void UpdateBodyHist(CvHistogram *bh, int i);
	//void UpdateHeight(double h);
	//void UpdateAndShow(CameraLine3D * headline, IplImage * Cr, IplImage * Cb, IplImage* src, bool IsInGroup);

};


//Geometry match function
bool CheckLineIntersect(const CameraLine3D* line1, const CameraLine3D* line2, Position3D & intersect, Position3D & line1pt, Position3D & line2pt);


//Appearance comparison function
double AppearanceComparison(const BodyInfo* ob1, const BodyInfo* ob2, int & matchCnt, int & level, double & best_color_score, int & best_block_index);
double AppearanceComparison_powWeight(const BodyInfo* ob1, const BodyInfo* ob2, int & matchCnt, int & level, double & best_color_log_score, int & best_block_index);

class List
{
public:
	vector<Pair> pairList;
	double totalScore;
	int length;

	List(){pairList.clear(); length = 0; totalScore = 0;}
	List(const List & lt){pairList = lt.pairList; length = lt.length; totalScore = lt.totalScore;}
	~List(){pairList.clear();}
	List & operator=(const List & rhs){pairList.clear(); pairList = rhs.pairList; length = rhs.length; totalScore = rhs.totalScore; return *this;}

	List(List* lt, Pair pr, double pr_score){pairList.clear(); pairList = lt->pairList; length = lt->length + 1; totalScore = lt->totalScore + pr_score; pairList.push_back(pr);}
	void addNewElement(Pair pr, double pr_score){length++; totalScore += pr_score; pairList.push_back(pr);}
	
};

class Rule
{
public:
	bool* cam1Valid;
	bool* cam2Valid;
	
	Rule(){cam1Valid = new bool[MAX_TARGET_NUM]; cam2Valid = new bool[MAX_TARGET_NUM]; for(int i=0; i<MAX_TARGET_NUM; i++){cam1Valid[i]=cam2Valid[i]=true;}}
	Rule(const Rule & rl){cam1Valid = new bool[MAX_TARGET_NUM]; cam2Valid = new bool[MAX_TARGET_NUM]; for(int i=0; i<MAX_TARGET_NUM; i++){cam1Valid[i]=rl.cam1Valid[i]; cam2Valid[i]=rl.cam2Valid[i];}}
	~Rule(){if(cam1Valid){delete [] cam1Valid; cam1Valid = NULL;} if(cam2Valid){delete [] cam2Valid; cam2Valid = NULL;} }
	Rule & operator=(const Rule & rhs){if(cam1Valid){delete [] cam1Valid;} if(cam2Valid){delete [] cam2Valid;} cam1Valid = new bool[MAX_TARGET_NUM]; cam2Valid = new bool[MAX_TARGET_NUM]; for(int i=0; i<MAX_TARGET_NUM; i++){cam1Valid[i]=rhs.cam1Valid[i]; cam2Valid[i]=rhs.cam2Valid[i];} return *this;}

	Rule(Rule* rl, Pair pr){cam1Valid = new bool[MAX_TARGET_NUM]; cam2Valid = new bool[MAX_TARGET_NUM]; for(int i=0; i<MAX_TARGET_NUM; i++){cam1Valid[i]=rl->cam1Valid[i]; cam2Valid[i]=rl->cam2Valid[i];} cam1Valid[pr.i] = cam2Valid[pr.j] = false;}
	void addNewElement(Pair pr){cam1Valid[pr.i] = cam2Valid[pr.j] = false;}
	bool checkValidPairFromRule (int cam1_i, int cam2_j) const {return cam1Valid[cam1_i] && cam2Valid[cam2_j];}
};

class ObjectInfo
{
public:
	BodyInfo objectBodyInfo;
	int indexInfo[2];
	int conf;

	ObjectInfo(){indexInfo[CAM1]=indexInfo[CAM2]=NONDEFINED; conf = 0;}
	ObjectInfo(const ObjectInfo & oi){objectBodyInfo = oi.objectBodyInfo; indexInfo[CAM1] = oi.indexInfo[CAM1]; indexInfo[CAM2] = oi.indexInfo[CAM2]; conf = oi.conf;}
	~ObjectInfo(){}
	ObjectInfo & operator=(const ObjectInfo & rhs){objectBodyInfo = rhs.objectBodyInfo; indexInfo[CAM1] = rhs.indexInfo[CAM1]; indexInfo[CAM2] = rhs.indexInfo[CAM2]; conf = rhs.conf; return *this;}
	ObjectInfo(BodyInfo* biptr, int obsIndex_cam1, int obsIndex_cam2, int obj_conf/* = OBJ_NON_DEFINED*/){objectBodyInfo = *biptr; indexInfo[CAM1] = obsIndex_cam1; indexInfo[CAM2] = obsIndex_cam2; conf = obj_conf;}

};

ObjectInfo createObjectInfo(BodyInfo* biptr, int obsIndex_cam1, int obsIndex_cam2, int obj_conf = 0);

class ObjectSet
{
public:
	vector<ObjectInfo> objectInfoList;
	int objectNum;

	ObjectSet(){objectNum = 0;}

	void addNewObject(ObjectInfo* oi){objectInfoList.push_back(*oi); objectNum++; }
	bool indexValidator(int obsIndex_cam1, int obsIndex_cam2){vector<ObjectInfo>::iterator it; for(it=objectInfoList.begin(); it!=objectInfoList.end(); it++){ if((it->indexInfo[CAM1]==obsIndex_cam1) || (it->indexInfo[CAM2]==obsIndex_cam2)) return false;} if((it->indexInfo[CAM1]==obsIndex_cam1) || (it->indexInfo[CAM2]==obsIndex_cam2)) return false; return true;}
};

//functions that deal with consistent indices for observation integration table
//return: RETURN_DONE_NO_ERROR, RETURN_CONTRADICT, RETURN_ALREADY_DONE
RETURN_STATUS observationIntegrate(int** confMap, int obsIndex_cam1, int obsIndex_cam2, int Lv, bool* cam1Alive, bool* cam2Alive);
RETURN_STATUS observationDisappear(int** confMap, int cam_id, int obsIndex_camX, bool* cam1Alive, bool* cam2Alive);
RETURN_STATUS observationShowIndep(int** confMap, int cam_id, int obsIndex_camX, bool* cam1Alive, bool* cam2Alive);
void printConfMapOnConsole(int** confMap, bool* cam1Alive, bool* cam2Alive);

//construct all pair configurations from confidence map
void findAllPairMode(int t, Rule rl, List* lt, double** scoreMap, int** confMap, int* pairNumCtr, double* bestScore, vector<List*>* pairModeList);

//if want to print points, just set firstPt = secondPt
void printCalibLine_3Dto2D(Position3D pt1, Position3D pt2, CameraLine3D cameraLine, IplImage* srcImg);
void printCalibLine_2Dto3D(CvPoint pt1, CvPoint pt2, CameraLine3D cameraLine, IplImage* srcImg);


//calculate standard deviation and mean for each iteration
void calcMeanStdDev(double newSample, int & sampleNum, double & newMean, double & newStdDev, double oldMean, double oldStdDev);

#endif