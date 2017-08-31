#ifndef TARGETDATABASE_H
#define TARGETDATABASE_H


#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <iostream>
#include <fstream>
using namespace std;
#include <map>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvcam.h>
#include <cmath>
#include <stdlib.h>
#include "Component.h"

#define STABLE_MATCH_NUM 0//25//50
#define MAX_KERNEL_NUM 3
#define CANDIDATE_NUM 1
#define WIDTH 320
#define HEIGHT 240
#define PI 3.1415926
#define TH_GMM 0.0398942
#define ALPHA 0.3//0.2
#define INIT_HIGH_DEV 0.3//10
#define INIT_LOW_WEIGHT 0.3//0.1
#define MIN_DEV 0.15//4
#define MAGNIFY_FACTOR 2000

#define DB_SCREEN_WIDTH 450
#define DB_SCREEN_HEIGHT 120


#define ALL_EXISTED_UNMATCH 1 //unmatch_Cnt == knlNum
#define UN_INITIALIZED 2 //knlNum == 0
#define FLAG_INIT -1

#define DEFAULT_H 155

//IplImage * databaseImg = cvCreateImage(cvSize(WIDTH,HEIGHT),8,1);

enum DEMO_STATUS
{
	CANDIDATE,
	INPUT1,
	INPUT2,
	INPUT3
};

class GKernel
{
public:
	double dev;
	CvHistogram* K_hist;
	CvHistogram* K_histH;
	double weight;
	bool cam1match;
	bool cam2match;
	bool cam3match;

	GKernel();
	~GKernel();
	GKernel(const GKernel & gk);
	GKernel & operator=(const GKernel & rhs);
};

int compare (const void * a, const void * b);
bool operator<(const Pair & left, const Pair & right);
bool operator>(const Pair & left, const Pair & right);
//bool operator<(const Pair left, const Pair & right);
//bool operator>(const Pair & left, const Pair right);
class targetDatabase
{
public:
	targetDatabase();
	~targetDatabase();
	targetDatabase(const targetDatabase & td);
	targetDatabase & operator=(const targetDatabase & rhs);
	//////////void Evaluation(IplImage * src);
	int matchObject(BodyInfo* ob1, BodyInfo* ob2, int ObjLv, int & matchLv, int ob1_id, int ob2_id);
	int matchObject_powWeight(BodyInfo* ob1, BodyInfo* ob2, int ObjLv, int & matchLv, int ob1_id, int ob2_id, double objH);
	double updateCreateGKernel(CvHistogram* objHist, CvHistogram* objHistH, int tarId, int bkId, int & kerId, int camNo);
	double GKernelScoreH(CvHistogram* objHistH, int tarId, int bkId, double & bestDist);
	double GKernelScore_powWeight_H(CvHistogram* objHistH, int tarId, int bkId, double & bestLogDist);
	void updateHistCrCb_H(CvHistogram* in_Hist, CvHistogram* org_Hist, CvHistogram* fnl_Hist, CvHistogram* in_HistH, CvHistogram* org_HistH, CvHistogram* fnl_HistH, double alpha, double matchDist);
	double targetObjectComparison(int tarId, const BodyInfo* ob1, const BodyInfo* ob2, int & level, double & best_color_score, int & best_block_index, double & bestKernelScore);
	double targetObjectComparison_powWeight_H(int tarId, const BodyInfo* ob1, const BodyInfo* ob2, int & level, double & best_color_log_score, int & best_block_index, double & bestKernelLogScore);
	int targetObjectLikelihoodThresholding(double color_score, int best_color_index, double best_color_dist, int Lv, double bestKernelScore);
	int targetObjectLikelihoodThresholding_powWeight(int targetID, double color_log_score, int best_color_index, double best_color_log_dist, int Lv, double bestKernelLogScore, double objH);
	void updateCreateDatabase(int & matchObject_return, BodyInfo* ob1, BodyInfo* ob2, int ob1_id, int ob2_id, double objH);
	void showDatabaseDetail();
	void KernelUpdateDemo(int targetLabel, int block_id, CvHistogram* cam1obs_hist, CvHistogram* cam2obs_hist);
	int histDisplay(IplImage* img, CvPoint startpt, CvHistogram* hist, DEMO_STATUS status, bool cam1match, bool cam2match, bool cam3match);
	int getTargetNum(){return targetNum;}
	//////////double Score(IplImage * src, int x, int y);
	//////////void Update(IplImage * src);
	//////////void show();


private:

	class GMM
	{
	public:
		int KnlNum;
		/*double dev[MAX_KERNEL_NUM];
		double mean[MAX_KERNEL_NUM];
		double weight[MAX_KERNEL_NUM];*/
		GKernel kernel[MAX_KERNEL_NUM];
		GMM()
		{
			KnlNum = 0;
			//for (int i=0; i<MAX_KERNEL_NUM; i++)
			//{
			//	kernel[i].dev=kernel[i].weight=0;
			//}
		}
		GMM(const GMM & gmm)
		{
			KnlNum = gmm.KnlNum;
			for (int i=0; i<MAX_KERNEL_NUM; i++)
			{
				/*dev[i] = gmm.dev[i];
				mean[i] = gmm.mean[i];
				weight[i] = gmm.weight[i];*/
				kernel[i] = gmm.kernel[i];
			}
		}
		GMM & operator=(const GMM & rhs)
		{
			//dtor is empty
			KnlNum = rhs.KnlNum;
			for (int i=0; i<MAX_KERNEL_NUM; i++)
			{
				/*dev[i] = rhs.dev[i];
				mean[i] = rhs.mean[i];
				weight[i] = rhs.weight[i];*/
				kernel[i] = rhs.kernel[i];
			}
			
			return *this;
		}
		~GMM(){}
	};

	GMM** targetDatabase_Matrix;
	IplImage* databaseImg;
	IplImage** candidateDemo;
	char** wTitleDemo;
	int* targetLvArr;
	int* targetCtsMatchCnt;
	int unmatchCnt;
	int firstEnteringPerson;
	CvFont font;
	CvFont bigFont;
	

public:
	map<Pair, int> objTarMapping; 
	double* targetHeight;
	int* labelAvail;
	int targetNum;
	IplImage** candidateDemo_Large;
	int peoplelist[20];
	int peoplecnt;

};
#endif

