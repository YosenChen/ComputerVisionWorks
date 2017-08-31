#ifndef ABM_H
#define ABM_H


#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <iostream>
#include <fstream>
using namespace std;

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvcam.h>
#include <cmath>
#include <stdlib.h>

#define MAX_KERNEL_NUM 3
#define CANDIDATE_NUM 1
#define WIDTH 320
#define HEIGHT 240
#define PI 3.1415926
#define TH_GMM 0.0398942
#define ALPHA 0.2
#define INIT_HIGH_DEV 10
#define INIT_LOW_WEIGHT 0.5
#define MIN_DEV 4
#define MAGNIFY_FACTOR 2000


#define ALL_EXISTED_UNMATCH 1 //unmatch_Cnt == knlNum
#define UN_INITIALIZED 2 //knlNum == 0
#define FLAG_INIT -1


//IplImage * scoreImg = cvCreateImage(cvSize(WIDTH,HEIGHT),8,1);


struct GKernel
{
	double dev;
	double mean;
	double weight;
};

int compare (const void * a, const void * b);

class ABM
{
public:
	ABM();
	~ABM();
	ABM(const ABM & abm);
	ABM & operator=(const ABM & rhs);
	void Evaluation(IplImage * src);
	double Score(IplImage * src, int x, int y);
	void Update(IplImage * src);
	void show();


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
			for (int i=0; i<MAX_KERNEL_NUM; i++)
				kernel[i].dev=kernel[i].mean=kernel[i].weight=0;
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

	GMM** ABM_Matrix;


};
#endif

