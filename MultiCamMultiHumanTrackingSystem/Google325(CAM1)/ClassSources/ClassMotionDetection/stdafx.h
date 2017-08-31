// stdafx.h : 可在此標頭檔中包含標準的系統 Include 檔，
// 或是經常使用卻很少變更的專案專用 Include 檔案
//

#pragma once

#include <iostream>
using namespace std;
#include <tchar.h>
#include <cstdio>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "cvcam.h"

// TODO: 在此參考您的程式所需要的其他標頭
#define REF_COLOR_MAP "skin.bmp"

#define RECORD_FILE_NUM 5 //最大可以錄影的數目

#define WIDTH 320.00
#define HEIGHT 240.00

#define ADD_PARTICLE_NUM /*10.00*/25.00 //需與SIS同步 stdafx.h同步
#define MAX_TARGET_NUM 5

#define X_DEV 6.00//15.00/*6*/
#define Y_DEV 4.00//10.00/*6*/
#define R_DEV 0.03f//20/MODEL_R
#define THETA_DEV 0.00//12.00//degree

// the bound of states
#define Min_x 0.00
#define Max_x WIDTH
#define Min_y 0.00
#define Max_y HEIGHT
#define Min_r 15.00/*20*/  //需大於等於circle_normal_sample_number  error的message都很忠實反映這個問題 只是你自己偏不信
#define Max_r 40.00/*HEIGHT/10*/
#define MODEL_R 30.00  //需與model同步
#define ELLIPSE_RATIO 1.3//2.5/*1.7*//*1.3*/
#define RANGE(x) ( x<WIDTH/8 ? 1 : x>(WIDTH-WIDTH/8) ? 1 : 0 )


///////////////////////////////////////////
/////     particle number %5 == 0     /////
///////////////////////////////////////////
// particle
#define base_particles_number 30
#define CLUSTER_NUM 2

//#define torso_particles_number 20
#define arm_particles_number 30
#define hand_particles_number 50

#define RIGHT true
#define LEFT false
#define UPPER true
#define LOWER false

#define update_freq 10
// edge
// the number of sample points on the circle contour
#define circle_edge_sample_number 40
// the number of sample points on the normal vector of each sample on the circle contour
#define circle_normal_sample_number 14		// even
#define line_edge_sample_number 10
#define line_normal_sample_number 10

//base
#define variance_x 2.0	//predict時的variance
#define variance_y 2.0
#define variance_r 2.0
#define variance_alpha CV_PI/72
//arm
#define variance_angle0 CV_PI/15
#define variance_angle1 CV_PI/25
#define variance_angle2 CV_PI/25
#define variance_angle3 CV_PI/25

// likelihood weight
#define C_color 1.0
#define C_edge 1.0
#define C_texture 1.0
#define C_mov 1.0
#define C_angle 1.0
#define C_hand 1.0

#define MAX_W1 0.2		//抓到物體的門檻   video5.avi
//#define MAX_W1 0.5		//抓到物體的門檻   test2.avi
#define MAX_W2 0.3		//物體消失的門檻

// color histogram
#define Cr_BINS 25
#define Cb_BINS 25
#define RANGE_MIN 0
#define RANGE_MAX 255

// texture
#define UPDATE_FREQ 10
#define TEMPLATE_WIDTH 32
#define TEMPLATE_HEIGHT 32

#define preWeight 0.3

//optical flow
#define TIME 2
#define MAX_COUNT 500
#define DISTHRESHOLD 1
