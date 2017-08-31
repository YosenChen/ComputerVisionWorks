// stdafx.h : �i�b�����Y�ɤ��]�t�зǪ��t�� Include �ɡA
// �άO�g�`�ϥΫo�ܤ��ܧ󪺱M�ױM�� Include �ɮ�
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

// TODO: �b���Ѧұz���{���һݭn����L���Y
#define REF_COLOR_MAP "skin.bmp"

#define RECORD_FILE_NUM 5 //�̤j�i�H���v���ƥ�

#define WIDTH 320.00
#define HEIGHT 240.00

#define ADD_PARTICLE_NUM /*10.00*/25.00 //�ݻPSIS�P�B stdafx.h�P�B
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
#define Min_r 15.00/*20*/  //�ݤj�󵥩�circle_normal_sample_number  error��message���ܩ���ϬM�o�Ӱ��D �u�O�A�ۤv�����H
#define Max_r 40.00/*HEIGHT/10*/
#define MODEL_R 30.00  //�ݻPmodel�P�B
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
#define variance_x 2.0	//predict�ɪ�variance
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

#define MAX_W1 0.2		//��쪫�骺���e   video5.avi
//#define MAX_W1 0.5		//��쪫�骺���e   test2.avi
#define MAX_W2 0.3		//������������e

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
