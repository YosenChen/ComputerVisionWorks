#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvcam.h>
#include <cmath>
#include <stdlib.h>
#include "Component.h"

// define the message structures of each sensor data
// laser pose
class LaserPoseMgr{
public:
	double x;
	double y;
	double theta;
};
#define LASERPOSE_NAME "LaserPoseMgr"
#define LASERPOSE_FORMAT "{double, double, double}"


// small laser
#define SLASER_NUM 541
class SLaserMgr{
public:
	int range[SLASER_NUM];
};
#define SLASER_NAME "SLaserMgr"
#define SLASER_FORMAT "{[int:541]}"


// subgoal
class SubgoalMgr{
public:
	double x;
	double y;
	double theta;
};
#define SUBGOAL_NAME "SubgoalMgr"
#define SUBGOAL_FORMAT "{double, double, double}"

// odometry
class OdometryMgr{
public:
	double x;
	double y;
	double theta;
	double velocity;
	double angularVelocity;
};
#define ODOMETRY_NAME "OdometryMgr"
#define ODOMETRY_FORMAT "{double, double, double, double, double}"


// motor
class MotorMgr{
public:
	double lEncoder;
	double rEncoder;
};
#define MOTOR_NAME "MotorMgr"
#define MOTOR_FORMAT "{double, double}"

//SpeedMode
class SpeedModeMgr{
public:
	int x;
};
#define SPEEDMODE_NAME "SpeedModeMgr"
#define SPEEDMODE_FORMAT "{int}"

//HeadMotor
class HeadMotorMgr{
public:
	int x;
};
#define HEADMOTOR_NAME "HeadMotorMgr"
#define HEADMOTOR_FORMAT "{int}"

// Hand
class HandPosMgr{
public:
	int HPos;
};
#define HANDPOS_NAME "HandPosMgr"
#define HANDPOS_FORMAT "{int}"

// facial expression
class FacialExpMgr{
public:
	int state;
};
#define FACIAlEXP_NAME "FacialExpMgr"
#define FACIAlEXP_FORMAT "{int}"

// microphone--19
class Mic19Mgr{
public:
	int input;
	int Rchannel;
	int Lchannel;
};
#define MIC19_NAME "Mic19Mgr"
#define MIC19_FORMAT "{int, int, int}"

// microphone--20
class Mic20Mgr{
public:
	int input;
	int Rchannel;
	int Lchannel;
};
#define MIC20_NAME "Mic20Mgr"
#define MIC20_FORMAT "{int, int, int}"

// microphone--21
class Mic21Mgr{
public:
	int input;
	int Rchannel;
	int Lchannel;
};
#define MIC21_NAME "Mic21Mgr"
#define MIC21_FORMAT "{int, int, int}"

// microphone--22
class Mic22Mgr{
public:
	int input;
	int Rchannel;
	int Lchannel;
};
#define MIC22_NAME "Mic22Mgr"
#define MIC22_FORMAT "{int, int, int}"

class StateMgr{
public: 
	char status[256];
	//char webcontrol[256];

};

#define STATE_NAME "StateMgr"
#define STATE_FORMAT "{[char:256]}"

class GoalMgr{
public: 
	char room[256];
	double x;
	double y;
	double theta;
};
#define GOAL_NAME "GoalMgr"
#define GOAL_FORMAT "{ [char:256], double, double, double}"

class PeopleMgr{
public:
	double rank;
	double x;
	double y;
	double theta;
};
#define PEOPLE_NAME "PeopleMgr"
#define PEOPLE_FORMAT "{double, double, double, double}"

class WordMgr{
public:
	char keyword[256];
};
#define WORD_NAME "WordMgr"
#define WORD_FORMAT "{ [char:256]}"

class Detect{
public:
	int x;
	int y;
};
#define DETECT_NAME "Detect"
#define DETECT_FORMAT "{int, int}"

// loc state : LOC_INIT, LOC_OK, LOC_FAILURE
class LocState{
public:
	LocState(){ sprintf( state, "LOC_INIT"); }
	char state[16];
};
#define LOCSTATE_NAME "LocState"
#define LOCSTATE_FORMAT "{[char:16]}"

class SpeakMgr{
public:
	char words[256];
};
#define SPEAK_NAME "SpeakMgr"
#define SPEAK_FORMAT "{ [char:256]}"

class HistCrCbMgr_CAM1_OB1
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM1_OB1_NAME "HistCrCbMgr_CAM1_OB1"
#define HISTCRCB_CAM1_OB1_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM1_OB2
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM1_OB2_NAME "HistCrCbMgr_CAM1_OB2"
#define HISTCRCB_CAM1_OB2_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM1_OB3
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM1_OB3_NAME "HistCrCbMgr_CAM1_OB3"
#define HISTCRCB_CAM1_OB3_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM1_OB4
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM1_OB4_NAME "HistCrCbMgr_CAM1_OB4"
#define HISTCRCB_CAM1_OB4_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM1_OB5
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM1_OB5_NAME "HistCrCbMgr_CAM1_OB5"
#define HISTCRCB_CAM1_OB5_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM2_OB1
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM2_OB1_NAME "HistCrCbMgr_CAM2_OB1"
#define HISTCRCB_CAM2_OB1_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM2_OB2
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM2_OB2_NAME "HistCrCbMgr_CAM2_OB2"
#define HISTCRCB_CAM2_OB2_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM2_OB3
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM2_OB3_NAME "HistCrCbMgr_CAM2_OB3"
#define HISTCRCB_CAM2_OB3_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM2_OB4
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM2_OB4_NAME "HistCrCbMgr_CAM2_OB4"
#define HISTCRCB_CAM2_OB4_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM2_OB5
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM2_OB5_NAME "HistCrCbMgr_CAM2_OB5"
#define HISTCRCB_CAM2_OB5_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM3_OB1
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM3_OB1_NAME "HistCrCbMgr_CAM3_OB1"
#define HISTCRCB_CAM3_OB1_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM3_OB2
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM3_OB2_NAME "HistCrCbMgr_CAM3_OB2"
#define HISTCRCB_CAM3_OB2_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM3_OB3
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM3_OB3_NAME "HistCrCbMgr_CAM3_OB3"
#define HISTCRCB_CAM3_OB3_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM3_OB4
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM3_OB4_NAME "HistCrCbMgr_CAM3_OB4"
#define HISTCRCB_CAM3_OB4_FORMAT "{ double, [double:625]}"

class HistCrCbMgr_CAM3_OB5
{
public:
	double timeIndex;
	double binCrCb[Cr_BINS*Cb_BINS];
};
#define HISTCRCB_CAM3_OB5_NAME "HistCrCbMgr_CAM3_OB5"
#define HISTCRCB_CAM3_OB5_FORMAT "{ double, [double:625]}"


class test{
public:
	double re[20];
	double yy;
	int a;
	int b[16250];
	int k[5];
	double h;
	//int d;
	double g[5];
	int v;
	int y[9];
	//double s;
	double f[20];
	double j[3];
};
#define TEST_NAME "test"
#define TEST_FORMAT "{[double:20], double, int, [int:16250], [int:5], double, [double:5], int, [int:9], [double:20], [double:3]}" 

class BodyInfoMgr_CAM1_OB1
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM1_OB1_NAME "BodyInfoMgr_CAM1_OB1"
#define BODYINFO_CAM1_OB1_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM1_OB2
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM1_OB2_NAME "BodyInfoMgr_CAM1_OB2"
#define BODYINFO_CAM1_OB2_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM1_OB3
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM1_OB3_NAME "BodyInfoMgr_CAM1_OB3"
#define BODYINFO_CAM1_OB3_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM1_OB4
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM1_OB4_NAME "BodyInfoMgr_CAM1_OB4"
#define BODYINFO_CAM1_OB4_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM1_OB5
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM1_OB5_NAME "BodyInfoMgr_CAM1_OB5"
#define BODYINFO_CAM1_OB5_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"



class BodyInfoMgr_CAM2_OB1
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM2_OB1_NAME "BodyInfoMgr_CAM2_OB1"
#define BODYINFO_CAM2_OB1_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM2_OB2
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM2_OB2_NAME "BodyInfoMgr_CAM2_OB2"
#define BODYINFO_CAM2_OB2_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM2_OB3
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM2_OB3_NAME "BodyInfoMgr_CAM2_OB3"
#define BODYINFO_CAM2_OB3_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM2_OB4
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM2_OB4_NAME "BodyInfoMgr_CAM2_OB4"
#define BODYINFO_CAM2_OB4_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM2_OB5
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM2_OB5_NAME "BodyInfoMgr_CAM2_OB5"
#define BODYINFO_CAM2_OB5_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"



class BodyInfoMgr_CAM3_OB1
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM3_OB1_NAME "BodyInfoMgr_CAM3_OB1"
#define BODYINFO_CAM3_OB1_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM3_OB2
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM3_OB2_NAME "BodyInfoMgr_CAM3_OB2"
#define BODYINFO_CAM3_OB2_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM3_OB3
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM3_OB3_NAME "BodyInfoMgr_CAM3_OB3"
#define BODYINFO_CAM3_OB3_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM3_OB4
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM3_OB4_NAME "BodyInfoMgr_CAM3_OB4"
#define BODYINFO_CAM3_OB4_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"

class BodyInfoMgr_CAM3_OB5
{
public:
	double timeIndex;
	double IsHeightKnown;
	double height;
	double IsBodySeen[4];
	double binCrCb0[Cr_BINS*Cb_BINS];
	double binCrCb1[Cr_BINS*Cb_BINS];
	double binCrCb2[Cr_BINS*Cb_BINS];
	double binCrCb3[Cr_BINS*Cb_BINS];
	double Blocks0xywh[4];
	double Blocks1xywh[4];
	double Blocks2xywh[4];
	double Blocks3xywh[4];
};
#define BODYINFO_CAM3_OB5_NAME "BodyInfoMgr_CAM3_OB5"
#define BODYINFO_CAM3_OB5_FORMAT "{ double, double, double, [double:4], [double:625], [double:625], [double:625], [double:625], [double:4], [double:4], [double:4], [double:4]}"


class Pos3DMgr_CAM1_OB1
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM1_OB1_NAME "Pos3DMgr_CAM1_OB1"
#define POS3D_CAM1_OB1_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM1_OB2
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM1_OB2_NAME "Pos3DMgr_CAM1_OB2"
#define POS3D_CAM1_OB2_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM1_OB3
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM1_OB3_NAME "Pos3DMgr_CAM1_OB3"
#define POS3D_CAM1_OB3_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM1_OB4
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM1_OB4_NAME "Pos3DMgr_CAM1_OB4"
#define POS3D_CAM1_OB4_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM1_OB5
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM1_OB5_NAME "Pos3DMgr_CAM1_OB5"
#define POS3D_CAM1_OB5_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM2_OB1
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM2_OB1_NAME "Pos3DMgr_CAM2_OB1"
#define POS3D_CAM2_OB1_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM2_OB2
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM2_OB2_NAME "Pos3DMgr_CAM2_OB2"
#define POS3D_CAM2_OB2_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM2_OB3
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM2_OB3_NAME "Pos3DMgr_CAM2_OB3"
#define POS3D_CAM2_OB3_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM2_OB4
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM2_OB4_NAME "Pos3DMgr_CAM2_OB4"
#define POS3D_CAM2_OB4_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM2_OB5
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM2_OB5_NAME "Pos3DMgr_CAM2_OB5"
#define POS3D_CAM2_OB5_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM3_OB1
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM3_OB1_NAME "Pos3DMgr_CAM3_OB1"
#define POS3D_CAM3_OB1_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM3_OB2
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM3_OB2_NAME "Pos3DMgr_CAM3_OB2"
#define POS3D_CAM3_OB2_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM3_OB3
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM3_OB3_NAME "Pos3DMgr_CAM3_OB3"
#define POS3D_CAM3_OB3_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM3_OB4
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM3_OB4_NAME "Pos3DMgr_CAM3_OB4"
#define POS3D_CAM3_OB4_FORMAT "{double, double, double, double}"

class Pos3DMgr_CAM3_OB5
{
public:
	double timeIndex;
	double x;
	double y;
	double z;
};
#define POS3D_CAM3_OB5_NAME "Pos3DMgr_CAM3_OB5"
#define POS3D_CAM3_OB5_FORMAT "{double, double, double, double}"


class CamLineMgr_CAM1_OB1
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM1_OB1_NAME "CamLineMgr_CAM1_OB1"
#define CAMLINE_CAM1_OB1_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM1_OB2
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM1_OB2_NAME "CamLineMgr_CAM1_OB2"
#define CAMLINE_CAM1_OB2_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM1_OB3
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM1_OB3_NAME "CamLineMgr_CAM1_OB3"
#define CAMLINE_CAM1_OB3_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM1_OB4
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM1_OB4_NAME "CamLineMgr_CAM1_OB4"
#define CAMLINE_CAM1_OB4_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM1_OB5
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM1_OB5_NAME "CamLineMgr_CAM1_OB5"
#define CAMLINE_CAM1_OB5_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"


class CamLineMgr_CAM2_OB1
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM2_OB1_NAME "CamLineMgr_CAM2_OB1"
#define CAMLINE_CAM2_OB1_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM2_OB2
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM2_OB2_NAME "CamLineMgr_CAM2_OB2"
#define CAMLINE_CAM2_OB2_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM2_OB3
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM2_OB3_NAME "CamLineMgr_CAM2_OB3"
#define CAMLINE_CAM2_OB3_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM2_OB4
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM2_OB4_NAME "CamLineMgr_CAM2_OB4"
#define CAMLINE_CAM2_OB4_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM2_OB5
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM2_OB5_NAME "CamLineMgr_CAM2_OB5"
#define CAMLINE_CAM2_OB5_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"


class CamLineMgr_CAM3_OB1
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM3_OB1_NAME "CamLineMgr_CAM3_OB1"
#define CAMLINE_CAM3_OB1_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM3_OB2
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM3_OB2_NAME "CamLineMgr_CAM3_OB2"
#define CAMLINE_CAM3_OB2_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM3_OB3
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM3_OB3_NAME "CamLineMgr_CAM3_OB3"
#define CAMLINE_CAM3_OB3_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM3_OB4
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM3_OB4_NAME "CamLineMgr_CAM3_OB4"
#define CAMLINE_CAM3_OB4_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"

class CamLineMgr_CAM3_OB5
{
public:
	double timeIndex;
	double unit_dir_vec[3];
	double Stdpoint1[3];
	double Stdpoint2[3];
	double optic_center[3];
	double ref_plane_center[3];
	double ref_plane_dist;
	double unit_orth_up_vec[3];
	double unit_orth_right_vec[3];
	double cm2pixelRatio;
	double h2wRatio;
	double imageLoc[2];
	double central_dir_vec[3];
	double refplane_nor_dir_vec[3];
	double refplane_Stdpoint[3];
};
#define CAMLINE_CAM3_OB5_NAME "CamLineMgr_CAM3_OB5"
#define CAMLINE_CAM3_OB5_FORMAT "{double, [double:3], [double:3], [double:3], [double:3], [double:3], double, [double:3], [double:3], double, double, [double:2], [double:3], [double:3], [double:3]}"


class AliveStaMgr_CAM1
{
public:
	double timeIndex;
	double aliveArr[5];
};
#define ALIVESTA_CAM1_NAME "AliveStaMgr_CAM1"
#define ALIVESTA_CAM1_FORMAT "{double, [double:5]}"

class AliveStaMgr_CAM2
{
public:
	double timeIndex;
	double aliveArr[5];
};
#define ALIVESTA_CAM2_NAME "AliveStaMgr_CAM2"
#define ALIVESTA_CAM2_FORMAT "{double, [double:5]}"

class AliveStaMgr_CAM3
{
public:
	double timeIndex;
	double aliveArr[5];
};
#define ALIVESTA_CAM3_NAME "AliveStaMgr_CAM3"
#define ALIVESTA_CAM3_FORMAT "{double, [double:5]}"


class LabelIntegrateMgr_CAM1
{
public:
	double	timeIndex;
	double Alive[MAX_TARGET_NUM*3];
	double CAM1_id[MAX_TARGET_NUM*3];
	double CAM2_id[MAX_TARGET_NUM*3];
	double CAM3_id[MAX_TARGET_NUM*3];
};
#define LABELINTEGRATE_CAM1_NAME "LabelIntegrateMgr_CAM1"
#define LABELINTEGRATE_CAM1_FORMAT "{double, [double:15], [double:15], [double:15], [double:15]}"

class LabelIntegrateMgr_CAM2
{
public:
	double	timeIndex;
	double Alive[MAX_TARGET_NUM*3];
	double CAM1_id[MAX_TARGET_NUM*3];
	double CAM2_id[MAX_TARGET_NUM*3];
	double CAM3_id[MAX_TARGET_NUM*3];
};
#define LABELINTEGRATE_CAM2_NAME "LabelIntegrateMgr_CAM2"
#define LABELINTEGRATE_CAM2_FORMAT "{double, [double:15], [double:15], [double:15], [double:15]}"

class LabelIntegrateMgr_CAM3
{
public:
	double	timeIndex;
	double Alive[MAX_TARGET_NUM*3];
	double CAM1_id[MAX_TARGET_NUM*3];
	double CAM2_id[MAX_TARGET_NUM*3];
	double CAM3_id[MAX_TARGET_NUM*3];
};
#define LABELINTEGRATE_CAM3_NAME "LabelIntegrateMgr_CAM3"
#define LABELINTEGRATE_CAM3_FORMAT "{double, [double:15], [double:15], [double:15], [double:15]}"


#endif