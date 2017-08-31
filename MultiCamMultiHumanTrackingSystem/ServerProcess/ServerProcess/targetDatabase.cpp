#include "targetDatabase.h"

#define GAUSSIAN(val, m, devi) exp(-pow((val-m),2)/(2*devi*devi))/(pow(2*PI, 0.5)*devi)

//fstream fileOut2;
//IplImage * databaseImg = cvCreateImage(cvSize(DB_SCREEN_WIDTH,DB_SCREEN_HEIGHT),8,1);
bool operator<(const Pair & left, const Pair & right)
{
	return (left.i<right.i) ? true : (left.j<right.j) ? true : false ;
}
bool operator>(const Pair & left, const Pair & right)
{
	return (left.i>right.i) ? true : (left.j>right.j) ? true : false ;
}
//bool operator<(const Pair left, const Pair & right)
//{
//	return (left.i<right.i) ? true : (left.j<right.j) ? true : false ;
//}
//bool operator>(const Pair & left, const Pair right)
//{
//	return (left.i>right.i) ? true : (left.j>right.j) ? true : false ;
//}
int compare (const void * a, const void * b)
{
	if ((((GKernel*)a)->dev) == 0 && (((GKernel*)b)->dev) == 0) return 0;
	if ((((GKernel*)a)->dev) == 0) return 1;
	if ((((GKernel*)b)->dev) == 0) return -1;

	return (((((GKernel*)b)->weight)/(((GKernel*)b)->dev) - (((GKernel*)a)->weight)/(((GKernel*)a)->dev) > 0) ? 1 : -1);
}

GKernel::GKernel()
{
	int hdims = Cr_BINS;//16;
	float hranges_arr[] = {0,180};
    float* hranges = hranges_arr;
	float * Cr_ranges, * Cb_ranges;
	Cr_ranges = new float[2];
	Cb_ranges = new float[2];
	Cr_ranges[0] = Cb_ranges[0] = RANGE_MIN;
	Cr_ranges[1] = Cb_ranges[1] = RANGE_MAX;

	int hist_size[] = { Cr_BINS, Cb_BINS };
	float * ranges[] = { Cr_ranges, Cb_ranges };

	K_hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	K_histH = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );

	delete [] Cr_ranges;
	delete [] Cb_ranges;

	dev = 0;
	weight = 0;
	cam1match = cam2match = cam3match = false;
}
GKernel::~GKernel()
{
	cvReleaseHist(&K_hist);
	cvReleaseHist(&K_histH);
}
GKernel::GKernel(const GKernel &gk)
{
	int hdims = Cr_BINS;//16;
	float hranges_arr[] = {0,180};
    float* hranges = hranges_arr;
	float * Cr_ranges, * Cb_ranges;
	Cr_ranges = new float[2];
	Cb_ranges = new float[2];
	Cr_ranges[0] = Cb_ranges[0] = RANGE_MIN;
	Cr_ranges[1] = Cb_ranges[1] = RANGE_MAX;

	int hist_size[] = { Cr_BINS, Cb_BINS };
	float * ranges[] = { Cr_ranges, Cb_ranges };

	K_hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	K_histH = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
	cvCopyHist(gk.K_hist, &K_hist);
	cvCopyHist(gk.K_histH, &K_histH);

	delete [] Cr_ranges;
	delete [] Cb_ranges;

	dev = gk.dev;
	weight = gk.weight;	
	cam1match = gk.cam1match;
	cam2match = gk.cam2match;
	cam3match = gk.cam3match;
}
GKernel & GKernel::operator=(const GKernel & rhs)
{
	cvCopyHist(rhs.K_hist, &K_hist);
	cvCopyHist(rhs.K_histH, &K_histH);
	dev = rhs.dev;
	weight = rhs.weight;
	cam1match = rhs.cam1match;
	cam2match = rhs.cam2match;
	cam3match = rhs.cam3match;

	return * this;
}


targetDatabase::targetDatabase()
{
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 0.8, 0.8, 0, 1);
	cvInitFont(&bigFont, CV_FONT_HERSHEY_PLAIN, 1.4, 1.4, 0, 2);
	//targetDatabase_Matrix = new GMM[HEIGHT][WIDTH];
	targetDatabase_Matrix = new GMM*[MAX_TARGET_NUM*3];
	targetLvArr = new int[MAX_TARGET_NUM*3];
	labelAvail = new int[MAX_TARGET_NUM*3];
	targetCtsMatchCnt = new int[MAX_TARGET_NUM*3];
	candidateDemo = new IplImage*[MAX_TARGET_NUM*3];
	targetHeight = new double[MAX_TARGET_NUM*3];
	candidateDemo_Large = new IplImage*[MAX_TARGET_NUM*3];
	for (int i=0; i<MAX_TARGET_NUM*3; i++)
	{
		targetDatabase_Matrix[i] = new GMM[NUM_OF_BLOCKS];
		targetHeight[i] = DEFAULT_H;
		candidateDemo[i] = cvCreateImage(cvSize(30 + 45*5, 30 + 45*4 + 20), 8, 3);
		candidateDemo_Large[i] = cvCreateImage(cvSize((30 + 45*5)*2, (30 + 45*4 + 20)*2), 8, 3);
		cvZero(candidateDemo[i]);
		cvZero(candidateDemo_Large[i]);
		targetLvArr[i] = 0;
		labelAvail[i] = 1;
		targetCtsMatchCnt[i] = 0;
	}
	unmatchCnt = 0;
	//fileOut2.open("error report.txt",ios::out);
	databaseImg = cvCreateImage(cvSize(DB_SCREEN_WIDTH,DB_SCREEN_HEIGHT),8,1);
	cvZero(databaseImg);
	targetNum = 0;
	firstEnteringPerson = 0;

	wTitleDemo = new char*[MAX_TARGET_NUM*3];
	for (int i=0; i<MAX_TARGET_NUM*3; i++)
	{
		wTitleDemo[i] = new char[30];
		sprintf(wTitleDemo[i], "target ID: %d", i);
		//cvNamedWindow(wTitleDemo[i], 1);
	}



	//multiple re-entering
	peoplelist[0] = 0;
	peoplelist[1] = 0;
	peoplelist[2] = 1;
	peoplelist[3] = 2;
	peoplelist[4] = 2;
	peoplelist[5] = 1;
	peoplelist[6] = 2;
	peoplelist[7] = 0;
	peoplelist[8] = -1; // end
	peoplecnt = 0;

}
targetDatabase::~targetDatabase()
{
	if (targetDatabase_Matrix!=NULL)
	{
		for (int i=0; i<MAX_TARGET_NUM*3; i++)
		{
			if (targetDatabase_Matrix[i]!=NULL)
			{
				delete [] targetDatabase_Matrix[i];
				targetDatabase_Matrix[i] = NULL;
				delete [] wTitleDemo[i];
				wTitleDemo[i] = NULL;
			}

		}
		delete [] targetDatabase_Matrix;
		targetDatabase_Matrix = NULL;

		delete [] wTitleDemo;
		wTitleDemo = NULL;

		delete [] targetHeight;
		targetHeight = NULL;

		delete [] targetLvArr;
		targetLvArr = NULL;

		delete [] labelAvail;
		labelAvail = NULL;

		delete [] targetCtsMatchCnt;
		targetCtsMatchCnt = NULL;
	}
	cvReleaseImage(&databaseImg);
	cvReleaseImage(candidateDemo);
	cvReleaseImage(candidateDemo_Large);
	targetNum = 0;
	firstEnteringPerson = 0;
}

targetDatabase::targetDatabase(const targetDatabase & td)
{
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 0.8, 0.8, 0, 1);
	cvInitFont(&bigFont, CV_FONT_HERSHEY_PLAIN, 1.4, 1.4, 0, 2);
	//alpha = abm.alpha;
	//targetDatabase_Matrix = new GMM[HEIGHT][WIDTH];
	targetDatabase_Matrix = new GMM*[MAX_TARGET_NUM*3];
	candidateDemo = new IplImage*[MAX_TARGET_NUM*3];
	candidateDemo_Large = new IplImage*[MAX_TARGET_NUM*3];
	targetLvArr = new int[MAX_TARGET_NUM*3];
	labelAvail = new int[MAX_TARGET_NUM*3];
	targetHeight = new double[MAX_TARGET_NUM*3];
	targetCtsMatchCnt = new int[MAX_TARGET_NUM*3];
	for (int i=0; i<MAX_TARGET_NUM*3; i++)
	{
		targetDatabase_Matrix[i] = new GMM[NUM_OF_BLOCKS];
		candidateDemo[i] = cvCreateImage(cvSize(30 + 45*5, 30 + 45*4 + 20), 8, 3);
		candidateDemo_Large[i] = cvCreateImage(cvSize((30 + 45*5)*2, (30 + 45*4 + 20)*2), 8, 3);
	}


	for (int i=0; i<MAX_TARGET_NUM*3; i++)
	{
		cvCopy(td.candidateDemo[i], candidateDemo[i]);
		cvCopy(td.candidateDemo_Large[i], candidateDemo_Large[i]);
		targetLvArr[i] = td.targetLvArr[i];
		labelAvail[i] = td.labelAvail[i];
		targetHeight[i] = td.targetHeight[i];
		targetCtsMatchCnt[i] = td.targetCtsMatchCnt[i];
		for (int j=0; j<NUM_OF_BLOCKS; j++)
		{
			targetDatabase_Matrix[i][j] = td.targetDatabase_Matrix[i][j];
		}
	}
	
	unmatchCnt = td.unmatchCnt;
	databaseImg = cvCreateImage(cvSize(DB_SCREEN_WIDTH,DB_SCREEN_HEIGHT),8,1);
	cvCopy(td.databaseImg, databaseImg);

	targetNum = td.targetNum;
	firstEnteringPerson = td.firstEnteringPerson;

	wTitleDemo = new char*[MAX_TARGET_NUM*3];
	for (int i=0; i<MAX_TARGET_NUM*3; i++)
	{
		wTitleDemo[i] = new char[30];
		sprintf(wTitleDemo[i], "target ID: %d", i);
		//cvNamedWindow(wTitleDemo[i], 1);
	}


	//multiple re-entering
	peoplelist[0] = 0;
	peoplelist[1] = 0;
	peoplelist[2] = 1;
	peoplelist[3] = 2;
	peoplelist[4] = 2;
	peoplelist[5] = 1;
	peoplelist[6] = 2;
	peoplelist[7] = 0;
	peoplelist[8] = -1; // end
	peoplecnt = 0;
}
targetDatabase & targetDatabase::operator=(const targetDatabase & rhs)
{
	//dtor
	if (targetDatabase_Matrix!=NULL)
	{
		for (int i=0; i<MAX_TARGET_NUM*3; i++)
		{
			if (targetDatabase_Matrix[i]!=NULL)
			{
				delete [] targetDatabase_Matrix[i];
				targetDatabase_Matrix[i] = NULL;
			}
			cvZero(candidateDemo[i]);
			cvZero(candidateDemo_Large[i]);

		}
		delete [] targetDatabase_Matrix;
		targetDatabase_Matrix = NULL;

		delete [] targetLvArr;
		targetLvArr = NULL;
		delete [] labelAvail;
		labelAvail = NULL;
		delete [] targetHeight;
		targetHeight = NULL;
		delete [] targetCtsMatchCnt;
		targetCtsMatchCnt = NULL;
	}
	cvZero(databaseImg);

	//cctor
	//alpha = rhs.alpha;
	//targetDatabase_Matrix = new GMM[HEIGHT][WIDTH];
	targetDatabase_Matrix = new GMM*[MAX_TARGET_NUM*3];
	targetLvArr = new int[MAX_TARGET_NUM*3];
	labelAvail = new int[MAX_TARGET_NUM*3];
	targetHeight = new double[MAX_TARGET_NUM*3];
	targetCtsMatchCnt = new int[MAX_TARGET_NUM*3];
	for (int i=0; i<MAX_TARGET_NUM*3; i++)
	{
		targetDatabase_Matrix[i] = new GMM[NUM_OF_BLOCKS];
	}
	
	for (int i=0; i<MAX_TARGET_NUM*3; i++)
	{
		targetLvArr[i] = rhs.targetLvArr[i];
		labelAvail[i] = rhs.labelAvail[i];
		targetHeight[i] = rhs.targetHeight[i];
		targetCtsMatchCnt[i] = rhs.targetCtsMatchCnt[i];
		cvCopy(rhs.candidateDemo[i], candidateDemo[i]);
		cvCopy(rhs.candidateDemo_Large[i], candidateDemo_Large[i]);
		for (int j=0; j<NUM_OF_BLOCKS; j++)
		{
			targetDatabase_Matrix[i][j] = rhs.targetDatabase_Matrix[i][j];
		}
	}
	cvCopy(rhs.databaseImg, databaseImg);
	unmatchCnt = rhs.unmatchCnt;

	targetNum = rhs.targetNum;
	firstEnteringPerson = rhs.firstEnteringPerson;

	//multiple re-entering
	peoplelist[0] = 0;
	peoplelist[1] = 0;
	peoplelist[2] = 1;
	peoplelist[3] = 2;
	peoplelist[4] = 2;
	peoplelist[5] = 1;
	peoplelist[6] = 2;
	peoplelist[7] = 0;
	peoplelist[8] = -1; // end
	peoplecnt = 0;

	//
	return *this;
}

//return value: 
//0,1,2,3,4,5,..., 14 (positive integer) matched target index, can check matchLv
//-1: unmatched, create new target
//-2: unmatched, but didn't create new target(low confidence data)
//-3: unmatched, fully occupied
int targetDatabase::matchObject(BodyInfo* ob1, BodyInfo* ob2, int ObjLv, int & matchLv, int ob1_id, int ob2_id)
{
	double bestColorScore;
	int bestColorIndex;
	int level;
	double totalScore;
	int bestMatchedTarID = -1;
	int bestMatchedLevel = 0;
	int bestMatchedTHDScore = 0;
	double bestKernelScore = 1000;
	int tempTHDScore;

	map<Pair, int>::iterator iter;
	iter = objTarMapping.find(makePair(ob1_id, ob2_id, NULL, NULL));
	if (iter != objTarMapping.end()) 
	{
		matchLv = ObjLv;
		return objTarMapping[makePair(ob1_id, ob2_id, NULL, NULL)];
	}

	for (int i=0; i<targetNum; i++)
	{

		totalScore = targetObjectComparison(i, ob1, ob2, level, bestColorScore, bestColorIndex, bestKernelScore);
		//cout << "totalScore: " << 100.0*totalScore << endl;
		if ((tempTHDScore = targetObjectLikelihoodThresholding(totalScore, bestColorIndex, bestColorScore, level, bestKernelScore)) && (totalScore != -1))
		{
			cout << "tempTHDScore: " << tempTHDScore << endl;
			
			//object-target can label the same!
			//store pointer with best level best thd score
			if (bestMatchedLevel < level)
			{
				if (bestMatchedTHDScore < tempTHDScore)
				{
					bestMatchedLevel = level;
					bestMatchedTHDScore = tempTHDScore;
					bestMatchedTarID = i;
				}
			}
			
		}
	}

	if (targetNum == 0)//empty database
	{
		if (ObjLv >= NUM_OF_BLOCKS/2) //object level is no less than 2, eg. 2,3,4
		{
			//cout << "add new" << endl;
			return -1;
		}
		else //low object level
			return -2;
	}
	else if (bestMatchedLevel == 0) //noempty database but no match 
	{
		if (targetNum == MAX_TARGET_NUM*3)//full database
		{
			//kill the person entering first in current database
			return -3;
		}
		else //database not full, might create new target
		{
			if (ObjLv >= NUM_OF_BLOCKS/2) //object level is no less than 2, eg. 2,3,4
			{
				return -1;
			}
			else //low object level
				return -2;

		}
	}
	else
		;

	//match with target database
	//cout << "match with target database: " << bestMatchedTarID << endl;
	matchLv = bestMatchedLevel;
	return bestMatchedTarID;

}
//return value: 
//0,1,2,3,4,5,..., 14 (positive integer) matched target index, can check matchLv
//-1: unmatched, create new target
//-2: unmatched, but didn't create new target(low confidence data)
//-3: unmatched, fully occupied
int targetDatabase::matchObject_powWeight(BodyInfo* ob1, BodyInfo* ob2, int ObjLv, int & matchLv, int ob1_id, int ob2_id, double objH)
{

	double bestColorLogScore;
	int bestColorIndex;
	int level;
	double totalLogScore;
	int bestMatchedTarID = -1;
	int bestMatchedLevel = 0;
	int bestMatchedTHDScore = 0;
	double bestKernelLogScore = 1000;
	int tempTHDScore;

	map<Pair, int>::iterator iter;
	iter = objTarMapping.find(makePair(ob1_id, ob2_id, NULL, NULL));
	if (iter != objTarMapping.end()) 
	{
		matchLv = ObjLv;
		return objTarMapping[makePair(ob1_id, ob2_id, NULL, NULL)];
	}


//////////////////////////////////////
//////	////////multiple re-entering
//////	//////peoplelist[0] = 0;
//////	//////peoplelist[1] = 0;
//////	//////peoplelist[2] = 1;
//////	//////peoplelist[3] = 2;
//////	//////peoplelist[4] = 2;
//////	//////peoplelist[5] = 1;
//////	//////peoplelist[6] = 2;
//////	//////peoplelist[7] = 0;
//////	//////peoplelist[8] = 0; // end
//////	
//////	//interpret multiple re-entering
//////	peoplelist[0] = -1;
//////	peoplelist[1] = 0;
//////	peoplelist[2] = -1;
//////	peoplelist[3] = -1;
//////	peoplelist[4] = 2;
//////	peoplelist[5] = 1;
//////	peoplelist[6] = 2;
//////	peoplelist[7] = 0;
//////	peoplelist[8] = 0; // end
//////
//////
//////	int res = peoplelist[peoplecnt];
//////	peoplecnt++;
//////	return res;
////////////////////////////////////////




	for (int i=0; i<targetNum; i++)
	{
		if (!labelAvail[i]) continue;

		totalLogScore = targetObjectComparison_powWeight_H(i, ob1, ob2, level, bestColorLogScore, bestColorIndex, bestKernelLogScore);
		cout << "target: " << i << "...................." 
			<< "\n bestColorLogScore = " << bestColorLogScore 
			<< "\n bestColorIndex = " << bestColorIndex 
			<< "\n bestKernelLogScore = " << bestKernelLogScore
			<< "\n totalLogScore = " << totalLogScore << endl;
		//cout << "totalScore: " << 100.0*totalScore << endl;
		if ((tempTHDScore = targetObjectLikelihoodThresholding_powWeight(i, totalLogScore, bestColorIndex, bestColorLogScore, level, bestKernelLogScore, objH)) && (totalLogScore != -1))
		{
			cout << "target: " << i <<" tempTHDScore: " << tempTHDScore << endl;
			
			//object-target can label the same!
			//store pointer with best level best thd score
			if (bestMatchedLevel < level)
			{
				if (bestMatchedTHDScore < tempTHDScore)
				{
					bestMatchedLevel = level;
					bestMatchedTHDScore = tempTHDScore;
					bestMatchedTarID = i;
				}
			}
			
		}
	}

	if (targetNum == 0)//empty database
	{
		if (ObjLv >= NUM_OF_BLOCKS/2) //object level is no less than 2, eg. 2,3,4
		{
			//cout << "add new" << endl;
			unmatchCnt++;
			return -1;
		}
		else //low object level
			return -2;
	}
	else if (bestMatchedLevel == 0) //noempty database but no match 
	{
		if (targetNum == MAX_TARGET_NUM*3)//full database
		{
			//kill the person entering first in current database
			unmatchCnt++;
			return -3;
		}
		else //database not full, might create new target
		{
			if (ObjLv >= NUM_OF_BLOCKS/2) //object level is no less than 2, eg. 2,3,4
			{
				unmatchCnt++;
				return -1;
			}
			else //low object level
				return -2;

		}
	}
	else
		;

	//match with target database
	//cout << "match with target database: " << bestMatchedTarID << endl;
	matchLv = bestMatchedLevel;
	targetCtsMatchCnt[bestMatchedTarID]++;
	return bestMatchedTarID;

}

void targetDatabase::updateCreateDatabase(int & matchObject_return, BodyInfo* ob1, BodyInfo* ob2, int ob1_id, int ob2_id, double objH)
{
	if ((matchObject_return == -1) && (unmatchCnt>= STABLE_MATCH_NUM)) //create new, not full
	{
		unmatchCnt = 0;
		for (int u=0; u<MAX_TARGET_NUM*3; u++)
		{
			targetCtsMatchCnt[u] = 0;
		}
		cout << "ADD NEW ID: " << targetNum << endl;

		objTarMapping[makePair(ob1_id, ob2_id, NULL, NULL)] = targetNum;
		labelAvail[targetNum] = 0;
		matchObject_return = targetNum;

		int KID;
		targetLvArr[targetNum] = 0;
		targetHeight[targetNum] = objH;
		for (int j=0; j<NUM_OF_BLOCKS; j++)
		{
			CvPoint startpt = cvPoint(15 + 0*45, 30 + j*45 + 20);
			cvRectangle(candidateDemo[targetNum], cvPoint(startpt.x-7, startpt.y-7), cvPoint(startpt.x+Cr_BINS+7, startpt.y+Cb_BINS+7), CV_RGB(0, 0, 0), -1);
			startpt = cvPoint(15 + 1*45, 30 + j*45 + 20);
			cvRectangle(candidateDemo[targetNum], cvPoint(startpt.x-7, startpt.y-7), cvPoint(startpt.x+Cr_BINS+7, startpt.y+Cb_BINS+7), CV_RGB(0, 0, 0), -1);
			
			for (int g=0; g<MAX_KERNEL_NUM; g++)
			{
				GKernel* GKptr = &targetDatabase_Matrix[targetNum][j].kernel[g];
				GKptr->cam3match = GKptr->cam2match = GKptr->cam1match = false;
			}

			if (ob1->IsBodySeen[j])
			{
				updateCreateGKernel(ob1->body[j], ob1->bodyH[j], targetNum, j, KID, CAM1);
			}
			if (ob2->IsBodySeen[j])
			{
				updateCreateGKernel(ob2->body[j], ob2->bodyH[j], targetNum, j, KID, CAM2);
			}
			if (ob1->IsBodySeen[j] || ob2->IsBodySeen[j]) 
			{
				targetLvArr[targetNum]++;
			}
			KernelUpdateDemo(targetNum, j, (ob1->IsBodySeen[j]) ? ob1->body[j] : NULL, (ob2->IsBodySeen[j]) ? ob2->body[j] : NULL);
		}
		targetNum ++;
	}
	else if ((matchObject_return == -3) && (unmatchCnt >=  STABLE_MATCH_NUM)) //kill first
	{
		unmatchCnt = 0;
		for (int u=0; u<MAX_TARGET_NUM*3; u++)
		{
			targetCtsMatchCnt[u] = 0;
		}
		//kill	
		targetLvArr[firstEnteringPerson] = 0;
		targetHeight[firstEnteringPerson] = objH;
		for (int h=0; h<NUM_OF_BLOCKS; h++)
		{
			targetDatabase_Matrix[firstEnteringPerson][h].KnlNum = 0;
		}

		//create new

		objTarMapping[makePair(ob1_id, ob2_id, NULL, NULL)] = firstEnteringPerson;
		labelAvail[firstEnteringPerson] = 0;
		matchObject_return = firstEnteringPerson;


		int KID;
		for (int j=0; j<NUM_OF_BLOCKS; j++)
		{
			for (int g=0; g<MAX_KERNEL_NUM; g++)
			{
				GKernel* GKptr = &targetDatabase_Matrix[firstEnteringPerson][j].kernel[g];
				GKptr->cam3match = GKptr->cam2match = GKptr->cam1match = false;
			}

			CvPoint startpt = cvPoint(15 + 0*45, 30 + j*45 + 20);
			cvRectangle(candidateDemo[firstEnteringPerson], cvPoint(startpt.x-7, startpt.y-7), cvPoint(startpt.x+Cr_BINS+7, startpt.y+Cb_BINS+7), CV_RGB(0, 0, 0), -1);
			startpt = cvPoint(15 + 1*45, 30 + j*45 + 20);
			cvRectangle(candidateDemo[firstEnteringPerson], cvPoint(startpt.x-7, startpt.y-7), cvPoint(startpt.x+Cr_BINS+7, startpt.y+Cb_BINS+7), CV_RGB(0, 0, 0), -1);

			if (ob1->IsBodySeen[j])
			{
				updateCreateGKernel(ob1->body[j], ob1->bodyH[j], firstEnteringPerson, j, KID, CAM1);
			}
			if (ob2->IsBodySeen[j])
			{
				updateCreateGKernel(ob2->body[j], ob2->bodyH[j], firstEnteringPerson, j, KID, CAM2);
			}
			if (ob1->IsBodySeen[j] || ob2->IsBodySeen[j]) 
			{
				targetLvArr[firstEnteringPerson]++;
			}
			KernelUpdateDemo(firstEnteringPerson, j, (ob1->IsBodySeen[j]) ? ob1->body[j] : NULL, (ob2->IsBodySeen[j]) ? ob2->body[j] : NULL);

		}
		firstEnteringPerson ++;
		firstEnteringPerson = firstEnteringPerson%(MAX_TARGET_NUM*3);

	}
	else if ((matchObject_return >= 0) && 
		((targetCtsMatchCnt[matchObject_return] >=  STABLE_MATCH_NUM)||(objTarMapping[makePair(ob1_id, ob2_id, NULL, NULL)] == matchObject_return))) //update old
	{
		objTarMapping[makePair(ob1_id, ob2_id, NULL, NULL)] = matchObject_return;
		labelAvail[matchObject_return] = 0;
		
		if (abs(objH - targetHeight[matchObject_return]) > 4.0) return;
		
		targetHeight[matchObject_return] = targetHeight[matchObject_return]*0.8 + objH*0.2;

		targetCtsMatchCnt[matchObject_return] = 0;
		for (int u=0; u<MAX_TARGET_NUM*3; u++)
		{
			targetCtsMatchCnt[u] = 0;
		}

		//cout << "update old: " << matchObject_return << endl;
		int tempLv = 0;
		int KID;
		for (int j=0; j<NUM_OF_BLOCKS; j++)
		{
			for (int g=0; g<MAX_KERNEL_NUM; g++)
			{
				GKernel* GKptr = &targetDatabase_Matrix[matchObject_return][j].kernel[g];
				GKptr->cam3match = GKptr->cam2match = GKptr->cam1match = false;
			}

			CvPoint startpt = cvPoint(15 + 0*45, 30 + j*45 + 20);
			cvRectangle(candidateDemo[matchObject_return], cvPoint(startpt.x-7, startpt.y-7), cvPoint(startpt.x+Cr_BINS+7, startpt.y+Cb_BINS+7), CV_RGB(0, 0, 0), -1);
			startpt = cvPoint(15 + 1*45, 30 + j*45 + 20);
			cvRectangle(candidateDemo[matchObject_return], cvPoint(startpt.x-7, startpt.y-7), cvPoint(startpt.x+Cr_BINS+7, startpt.y+Cb_BINS+7), CV_RGB(0, 0, 0), -1);

			if (ob1->IsBodySeen[j]) 
			{
				updateCreateGKernel(ob1->body[j], ob1->bodyH[j], matchObject_return, j, KID, CAM1);
			}
			if (ob2->IsBodySeen[j])
			{
				updateCreateGKernel(ob2->body[j], ob2->bodyH[j], matchObject_return, j, KID, CAM2);
			}
			if (ob1->IsBodySeen[j] || ob2->IsBodySeen[j])
			{
				tempLv++;
			}
			KernelUpdateDemo(matchObject_return, j, (ob1->IsBodySeen[j]) ? ob1->body[j] : NULL, (ob2->IsBodySeen[j]) ? ob2->body[j] : NULL);
		}
		if (tempLv > targetLvArr[matchObject_return]) targetLvArr[matchObject_return] = tempLv;
	}
	else 
		;

}

void targetDatabase::updateHistCrCb_H(CvHistogram* in_Hist, CvHistogram* org_Hist, CvHistogram* fnl_Hist, CvHistogram* in_HistH, CvHistogram* org_HistH, CvHistogram* fnl_HistH, double alpha, double matchDist)
{
	double value;
	double lo = alpha*(1.0-matchDist);
	for (int id0=0; id0<Cr_BINS; id0++)
	{
		value = (1.0-lo)*(*(cvGetHistValue_1D(org_HistH, id0))) + lo*(*(cvGetHistValue_1D(in_HistH, id0))); 
		*(cvGetHistValue_1D(fnl_HistH, id0)) = value;
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			value = (1.0-lo)*(*(cvGetHistValue_2D(org_Hist, id0, id1))) + lo*(*(cvGetHistValue_2D(in_Hist, id0, id1))); 
			*(cvGetHistValue_2D(fnl_Hist, id0, id1)) = value;
		}
	}
	cvNormalizeHist(fnl_Hist, 1);
	cvNormalizeHist(fnl_HistH, 1);
}
double targetDatabase::updateCreateGKernel(CvHistogram* objHist, CvHistogram* objHistH, int tarId, int bkId, int & kerId, int camNo)
{
	if (targetDatabase_Matrix[tarId][bkId].KnlNum == 0) //no kernel is initialized, create one
	{
		GMM* gmmptr = &targetDatabase_Matrix[tarId][bkId];
		gmmptr->kernel[0].dev = INIT_HIGH_DEV;
		gmmptr->kernel[0].weight = INIT_LOW_WEIGHT;
		gmmptr->kernel[0].cam1match = (camNo == CAM1) ? true : gmmptr->kernel[0].cam1match;
		gmmptr->kernel[0].cam2match = (camNo == CAM2) ? true : gmmptr->kernel[0].cam2match;
		gmmptr->kernel[0].cam3match = (camNo == CAM3) ? true : gmmptr->kernel[0].cam3match;
		cvCopyHist(objHist, &(gmmptr->kernel[0].K_hist));
		cvCopyHist(objHistH, &(gmmptr->kernel[0].K_histH));
		gmmptr->KnlNum = 1;
		kerId = 0;

		gmmptr->kernel[0].weight = 1.0;
		return -1;
	}
	
	kerId = -1;
	double matchDist;
	double bestCompareDist = 10000;
	for (int i=0; i<targetDatabase_Matrix[tarId][bkId].KnlNum; i++)
	{
		GKernel* ker = &targetDatabase_Matrix[tarId][bkId].kernel[i];
		double dist = cvCompareHist(ker->K_histH, objHistH, CV_COMP_BHATTACHARYYA);
		if (dist < bestCompareDist)
		{
			bestCompareDist = dist;
		}
		if ((dist < /*2.0*/2.7*ker->dev) && (kerId == -1))
		{
			//matched!
			kerId = i;
			matchDist = dist;
			updateHistCrCb_H(objHist, ker->K_hist, ker->K_hist, objHistH, ker->K_histH, ker->K_histH, 0.05, dist);
			double tmpdev = sqrt((1.0-0.05*(1.0-dist))*(ker->dev)*(ker->dev) + (0.05*(1.0-dist))*dist*dist);
			ker->dev = (tmpdev > MIN_DEV) ? tmpdev : MIN_DEV;
			ker->weight = (1.0-0.05)*(ker->weight) + 0.05*1.0;
			ker->cam1match = (camNo == CAM1) ? true : ker->cam1match;
			ker->cam2match = (camNo == CAM2) ? true : ker->cam2match;
			ker->cam3match = (camNo == CAM3) ? true : ker->cam3match;
		}
		else
		{
			//unmatched
			double tmpw = (1.0-0.05)*(ker->weight);
			ker->weight = (tmpw > INIT_LOW_WEIGHT) ? tmpw : INIT_LOW_WEIGHT;

		}
	}

	if (kerId == -1 && (targetDatabase_Matrix[tarId][bkId].KnlNum == MAX_KERNEL_NUM))
	{
		//if ((bestCompareDist > 0.55/*0.35*/) && (targetDatabase_Matrix[tarId][bkId].KnlNum >= 3)) return -2;


		//none of kernels match, kernel all used, so kill last, least ordering one
		qsort(targetDatabase_Matrix[tarId][bkId].kernel, MAX_KERNEL_NUM, sizeof(GKernel), compare);
		GMM* gmmptr = &targetDatabase_Matrix[tarId][bkId];
		gmmptr->kernel[MAX_KERNEL_NUM-1].dev = INIT_HIGH_DEV;
		gmmptr->kernel[MAX_KERNEL_NUM-1].weight = INIT_LOW_WEIGHT;
		gmmptr->kernel[MAX_KERNEL_NUM-1].cam1match = (camNo == CAM1) ? true : gmmptr->kernel[MAX_KERNEL_NUM-1].cam1match;
		gmmptr->kernel[MAX_KERNEL_NUM-1].cam2match = (camNo == CAM2) ? true : gmmptr->kernel[MAX_KERNEL_NUM-1].cam2match;
		gmmptr->kernel[MAX_KERNEL_NUM-1].cam3match = (camNo == CAM3) ? true : gmmptr->kernel[MAX_KERNEL_NUM-1].cam3match;
		cvCopyHist(objHist, &(gmmptr->kernel[MAX_KERNEL_NUM-1].K_hist));
		cvCopyHist(objHistH, &(gmmptr->kernel[MAX_KERNEL_NUM-1].K_histH));
		gmmptr->KnlNum = MAX_KERNEL_NUM;
		kerId = MAX_KERNEL_NUM-1;
	
		//now we normalize the weight by KnlNum
		double totalWeight = 0;
		for (int u=0; u<targetDatabase_Matrix[tarId][bkId].KnlNum; u++)
		{
			totalWeight += targetDatabase_Matrix[tarId][bkId].kernel[u].weight;
		}
		for (int u=0; u<targetDatabase_Matrix[tarId][bkId].KnlNum; u++)
		{
			targetDatabase_Matrix[tarId][bkId].kernel[u].weight /= totalWeight;
		}

		return -1;
	}
	else if (kerId == -1)   
	{
		//if ((bestCompareDist > 0.55/*0.35*/) && (targetDatabase_Matrix[tarId][bkId].KnlNum >= 3)) return -2;


		//none of kernels match, but some kernels are unoccupied, use them
		GMM* gmmptr = &targetDatabase_Matrix[tarId][bkId];
		gmmptr->kernel[gmmptr->KnlNum].dev = INIT_HIGH_DEV;
		gmmptr->kernel[gmmptr->KnlNum].weight = INIT_LOW_WEIGHT;
		gmmptr->kernel[gmmptr->KnlNum].cam1match = (camNo == CAM1) ? true : gmmptr->kernel[gmmptr->KnlNum].cam1match;
		gmmptr->kernel[gmmptr->KnlNum].cam2match = (camNo == CAM2) ? true : gmmptr->kernel[gmmptr->KnlNum].cam2match;
		gmmptr->kernel[gmmptr->KnlNum].cam3match = (camNo == CAM3) ? true : gmmptr->kernel[gmmptr->KnlNum].cam3match;
		cvCopyHist(objHist, &(gmmptr->kernel[gmmptr->KnlNum].K_hist));
		cvCopyHist(objHistH, &(gmmptr->kernel[gmmptr->KnlNum].K_histH));
		kerId = gmmptr->KnlNum;
		gmmptr->KnlNum = gmmptr->KnlNum+1;

		qsort(targetDatabase_Matrix[tarId][bkId].kernel, targetDatabase_Matrix[tarId][bkId].KnlNum, sizeof(GKernel), compare);

		//now we normalize the weight by KnlNum
		double totalWeight = 0;
		for (int u=0; u<targetDatabase_Matrix[tarId][bkId].KnlNum; u++)
		{
			totalWeight += targetDatabase_Matrix[tarId][bkId].kernel[u].weight;
		}
		for (int u=0; u<targetDatabase_Matrix[tarId][bkId].KnlNum; u++)
		{
			targetDatabase_Matrix[tarId][bkId].kernel[u].weight /= totalWeight;
		}

		return -1;
	}
	else
	{
		//matched, kerId is ready
		qsort(targetDatabase_Matrix[tarId][bkId].kernel, targetDatabase_Matrix[tarId][bkId].KnlNum, sizeof(GKernel), compare);
		
		//now we normalize the weight by KnlNum
		double totalWeight = 0;
		for (int u=0; u<targetDatabase_Matrix[tarId][bkId].KnlNum; u++)
		{
			totalWeight += targetDatabase_Matrix[tarId][bkId].kernel[u].weight;
		}
		for (int u=0; u<targetDatabase_Matrix[tarId][bkId].KnlNum; u++)
		{
			targetDatabase_Matrix[tarId][bkId].kernel[u].weight /= totalWeight;
		}


		return matchDist;
	}

}
double targetDatabase::GKernelScoreH(CvHistogram* objHistH, int tarId, int bkId, double & bestDist)
{
	if (targetDatabase_Matrix[tarId][bkId].KnlNum == 0) return -1;
	
	bestDist = 1000;
	double weightedDist = 0;
	for (int i=0; i<targetDatabase_Matrix[tarId][bkId].KnlNum; i++)
	{
		GKernel* ker = &targetDatabase_Matrix[tarId][bkId].kernel[i];
		double dist = cvCompareHist(ker->K_histH, objHistH, CV_COMP_BHATTACHARYYA);
		weightedDist += (ker->weight)*dist;
		if (bestDist > dist)
		{
			bestDist = dist;
		}
	}

	return weightedDist;
}

double targetDatabase::GKernelScore_powWeight_H(CvHistogram* objHistH, int tarId, int bkId, double & bestLogDist)
{
	if (targetDatabase_Matrix[tarId][bkId].KnlNum == 0) return 1;
	
	bestLogDist = 1000;
	double weightedLogDist = 0;
	for (int i=0; i<targetDatabase_Matrix[tarId][bkId].KnlNum; i++)
	{
		GKernel* ker = &targetDatabase_Matrix[tarId][bkId].kernel[i];
		double logDist = log(cvCompareHist(ker->K_histH, objHistH, CV_COMP_BHATTACHARYYA));
		weightedLogDist += (ker->weight)*logDist;
		if (bestLogDist > logDist)
		{
			bestLogDist = logDist;
		}
	}

	return weightedLogDist;
}

double targetDatabase::targetObjectComparison(int tarId, const BodyInfo* ob1, const BodyInfo* ob2, int & level, double & best_color_score, int & best_block_index, double & bestKernelScore)
{
	double total_score = 0;
	level = 0;
	best_color_score = 10000;
	best_block_index = -1;
	int matchCnt = 0;
	double totalBestKernelDist = 0;


	for (int k=0; k<targetLvArr[tarId]; k++)
	{
		if (ob1->IsBodySeen[k] && ob2->IsBodySeen[k])
		{
			level++;
			cvNormalizeHist(ob1->body[k], 1);
			cvNormalizeHist(ob2->body[k], 1);
			double bestKernelDist;
			double dist = GKernelScoreH(ob1->bodyH[k], tarId, k, bestKernelDist);
			if (dist>=0) 
			{
				total_score += dist;//pow(dist, 2);
				totalBestKernelDist += bestKernelDist;
				matchCnt++;
			}
			if (dist < best_color_score)
			{
				best_color_score = dist;
				best_block_index = k;
			}


			dist = GKernelScoreH(ob2->bodyH[k], tarId, k, bestKernelDist);
			if (dist>=0) 
			{
				total_score += dist;//pow(dist, 2);
				totalBestKernelDist += bestKernelDist;
				matchCnt++;
			}
			if (dist < best_color_score)
			{
				best_color_score = dist;
				best_block_index = k;
			}

		}
		else if (ob1->IsBodySeen[k])
		{
			//only compare ob1
			level++;
			cvNormalizeHist(ob1->body[k], 1);
			double bestKernelDist;
			double dist = GKernelScoreH(ob1->bodyH[k], tarId, k, bestKernelDist);
			if (dist>=0) 
			{
				total_score += dist;//pow(dist, 2);
				totalBestKernelDist += bestKernelDist;
				matchCnt++;
			}

			if (dist < best_color_score)
			{
				best_color_score = dist;
				best_block_index = k;
			}
		}
		else if (ob2->IsBodySeen[k])
		{
			//only compare ob2
			level++;
			cvNormalizeHist(ob2->body[k], 1);
			double bestKernelDist;
			double dist = GKernelScoreH(ob2->bodyH[k], tarId, k, bestKernelDist);
			if (dist>=0) 
			{
				total_score += dist;//pow(dist, 2);
				totalBestKernelDist += bestKernelDist;
				matchCnt++;
			}

			if (dist < best_color_score)
			{
				best_color_score = dist;
				best_block_index = k;
			}
		}
		else
		{
			//either ob1, ob2 don't compare
			;
		}
	}

	//if (NUM_OF_BLOCKS - matchCnt <= 1) level = OBJ_HIGH_CONF;
	//else if (NUM_OF_BLOCKS - matchCnt == 2) level = OBJ_MID_CONF;
	//else level = OBJ_LOW_CONF;
	if (matchCnt == 0) return -1;


	bestKernelScore = totalBestKernelDist/matchCnt;
	cout << "bestKernelScore = " << bestKernelScore << endl;
	return total_score/matchCnt;//sqrt(total_score/matchCnt);
}

double targetDatabase::targetObjectComparison_powWeight_H(int tarId, const BodyInfo* ob1, const BodyInfo* ob2, int & level, double & best_color_log_score, int & best_block_index, double & bestKernelLogScore)
{
	double total_log_score = 0;
	level = 0;
	best_color_log_score = 10000;
	best_block_index = -1;
	int matchCnt = 0;
	double totalBestKernelLogDist = 0;


	for (int k=0; ((k<targetLvArr[tarId])&&(k<2)); k++)
	{
		if (ob1->IsBodySeen[k] && ob2->IsBodySeen[k])
		{
			level++;
			cvNormalizeHist(ob1->bodyH[k], 1);
			cvNormalizeHist(ob2->bodyH[k], 1);
			double bestKernelLogDist;
			double logDist = GKernelScore_powWeight_H(ob1->bodyH[k], tarId, k, bestKernelLogDist);
			if (logDist<=0) 
			{
				total_log_score += logDist;
				totalBestKernelLogDist += bestKernelLogDist;
				matchCnt++;
			}
			if (logDist < best_color_log_score)
			{
				best_color_log_score = logDist;
				best_block_index = k;
			}


			logDist = GKernelScore_powWeight_H(ob2->bodyH[k], tarId, k, bestKernelLogDist);
			if (logDist<=0) 
			{
				total_log_score += logDist;
				totalBestKernelLogDist += bestKernelLogDist;
				matchCnt++;
			}
			if (logDist < best_color_log_score)
			{
				best_color_log_score = logDist;
				best_block_index = k;
			}

		}
		else if (ob1->IsBodySeen[k])
		{
			//only compare ob1
			level++;
			cvNormalizeHist(ob1->bodyH[k], 1);
			double bestKernelLogDist;
			double logDist = GKernelScore_powWeight_H(ob1->bodyH[k], tarId, k, bestKernelLogDist);
			if (logDist<=0) 
			{
				total_log_score += logDist;
				totalBestKernelLogDist += bestKernelLogDist;
				matchCnt++;
			}

			if (logDist < best_color_log_score)
			{
				best_color_log_score = logDist;
				best_block_index = k;
			}
		}
		else if (ob2->IsBodySeen[k])
		{
			//only compare ob2
			level++;
			cvNormalizeHist(ob2->bodyH[k], 1);
			double bestKernelLogDist;
			double logDist = GKernelScore_powWeight_H(ob2->bodyH[k], tarId, k, bestKernelLogDist);
			if (logDist<=0) 
			{
				total_log_score += logDist;
				totalBestKernelLogDist += bestKernelLogDist;
				matchCnt++;
			}

			if (logDist < best_color_log_score)
			{
				best_color_log_score = logDist;
				best_block_index = k;
			}
		}
		else
		{
			//either ob1, ob2 don't compare
			;
		}
	}

	//if (NUM_OF_BLOCKS - matchCnt <= 1) level = OBJ_HIGH_CONF;
	//else if (NUM_OF_BLOCKS - matchCnt == 2) level = OBJ_MID_CONF;
	//else level = OBJ_LOW_CONF;
	if (matchCnt == 0) return -1;


	bestKernelLogScore = totalBestKernelLogDist/matchCnt;
	//cout << "bestKernelLogScore = " << bestKernelLogScore << endl;
	return total_log_score/matchCnt;
}

int targetDatabase::targetObjectLikelihoodThresholding(double color_score, int best_color_index, double best_color_dist, int Lv, double bestKernelScore)
{
	int total;
	total = ((color_score > /*0.3*/0.7) ? 0 : 5);
	total += ((color_score < /*0.3*/0.7) && (Lv >= 3)) ? 4 : 0;
	total += ((best_color_index <= 1) ? 3 : 0);
	total += ((best_color_dist > /*0.2*/0.6) ? 0 : 5);
	total += ((best_color_dist < /*0.2*/0.6) || (color_score < /*0.3*/0.7)) ? 3 : 0;
	total += (bestKernelScore < 0.45) ? 11 : 0;
	total += (bestKernelScore < 0.55) ? 4 : 0;

	if (total >= 13) 
	{
//		objConfMap[g][f] = Lv;// = OBJ_HIGH_CONF;
//		occupiedList_cam1[g] = occupiedList_cam2[f] = true;
	}
	
	return (total >= 11) ? total : 0;
}

int targetDatabase::targetObjectLikelihoodThresholding_powWeight(int targetID, double color_log_score, int best_color_index, double best_color_log_dist, int Lv, double bestKernelLogScore, double objH)
{
	int total;
	total = ((color_log_score > /*0.3*/log(0.4)) ? 0 : 5);
	total += ((color_log_score < /*0.3*/log(0.4)) && (Lv >= 2)) ? 4 : 0;
	//total += ((best_color_index <= 1) ? 3 : 0);
	total += ((best_color_log_dist > /*0.2*/log(0.35)) ? 0 : 5);
	//total += ((best_color_log_dist < /*0.2*/log(0.25)) || (color_log_score < /*0.3*/log(0.35))) ? 3 : 0;
	total += (bestKernelLogScore < log(0.2)) ? 6 : 0;
	total += (bestKernelLogScore < log(0.3)) ? 4 : 0;
	if (bestKernelLogScore < -10000) total += 11;
	if (bestKernelLogScore < -1) total += 5;
	//total += (abs(targetHeight[targetID] - objH) < 5) ? 5 : 0;
	total += (abs(targetHeight[targetID] - objH) < 3) ? 3 : 0;
	if (abs(targetHeight[targetID] - objH) > 9) total = 0;
	if (abs(targetHeight[targetID] - objH) > 6) total -= 5;

	if (total >= 18) 
	{
//		objConfMap[g][f] = Lv;// = OBJ_HIGH_CONF;
//		occupiedList_cam1[g] = occupiedList_cam2[f] = true;
	}
	
	return (total >= 16) ? total : 0;
}

void targetDatabase::showDatabaseDetail()
{
	cvZero(databaseImg);
	for (int t=0; t<MAX_TARGET_NUM*3/*targetNum*/; t++)
	{
		for (int q=0; q<NUM_OF_BLOCKS/*targetLvArr[t]*/; q++)
		{
			cvRectangle(databaseImg, cvPoint(30*t, 30*q), cvPoint(30*(t+1), 30*(q+1)), cvScalar(60*targetDatabase_Matrix[t][q].KnlNum), -1);
		}
	}
	cvNamedWindow("DatabaseDetail", 1);
	cvShowImage("DatabaseDetail", databaseImg);
}

void targetDatabase::KernelUpdateDemo(int targetLabel, int block_id, CvHistogram* cam1obs_hist, CvHistogram* cam2obs_hist)
{
	GKernel* GKptr = targetDatabase_Matrix[targetLabel][block_id].kernel;
	int kNum = targetDatabase_Matrix[targetLabel][block_id].KnlNum;
	for (int i=0; i<kNum; i++)
	{
		histDisplay(candidateDemo[targetLabel], cvPoint(30 + (i+2)*45, 30 + block_id*45 + 20), GKptr[i].K_hist, CANDIDATE, GKptr[i].cam1match, GKptr[i].cam2match, GKptr[i].cam3match);
	}

	if (cam1obs_hist) 
	{
		if ((histDisplay(candidateDemo[targetLabel], cvPoint(15 + 0*45, 30 + block_id*45 + 20), cam1obs_hist, INPUT1, true, true, true) < 0) && (cam2obs_hist))
		{
			histDisplay(candidateDemo[targetLabel], cvPoint(15 + 0*45, 30 + block_id*45 + 20), cam2obs_hist, INPUT1, true, true, true);
		}
	}
	else
	{
		//CvPoint startpt = cvPoint(30 + 0*45, 30 + block_id*45);
		//cvRectangle(candidateDemo[targetLabel], cvPoint(startpt.x-7, startpt.y-7), cvPoint(startpt.x+Cr_BINS+7, startpt.y+Cb_BINS+7), CV_RGB(0, 0, 0), -1);
	}

	if (cam2obs_hist) 
	{
		if ((histDisplay(candidateDemo[targetLabel], cvPoint(15 + 1*45, 30 + block_id*45 + 20), cam2obs_hist, INPUT2, true, true, true) < 0) && (cam1obs_hist))
		{
			histDisplay(candidateDemo[targetLabel], cvPoint(15 + 0*45, 30 + block_id*45 + 20), cam1obs_hist, INPUT2, true, true, true);
		}
	}
	else
	{
		//CvPoint startpt = cvPoint(30 + 1*45, 30 + block_id*45);
		//cvRectangle(candidateDemo[targetLabel], cvPoint(startpt.x-7, startpt.y-7), cvPoint(startpt.x+Cr_BINS+7, startpt.y+Cb_BINS+7), CV_RGB(0, 0, 0), -1);
	}

	cvLine(candidateDemo[targetLabel], cvPoint(10, 30 + 45*3 + 20 - 10), cvPoint(30 + 5*45 - 10, 30 + 45*3 + 20 - 10), CV_RGB(100, 120, 100), 1);
	cvLine(candidateDemo[targetLabel], cvPoint(10, 30 + 45*2 + 20 - 10), cvPoint(30 + 5*45 - 10, 30 + 45*2 + 20 - 10), CV_RGB(100, 120, 100), 1);
	cvLine(candidateDemo[targetLabel], cvPoint(10, 30 + 45*1 + 20 - 10), cvPoint(30 + 5*45 - 10, 30 + 45*1 + 20 - 10), CV_RGB(100, 120, 100), 1);

	cvLine(candidateDemo[targetLabel], cvPoint(15 + 45*1 - 10, 20 + 20), cvPoint(15 + 45*1 - 10, 30 + 45*4 + 20 - 10), CV_RGB(150, 100, 150), 1);
	cvLine(candidateDemo[targetLabel], cvPoint(15 + 45*2 - 10, 20 + 20), cvPoint(15 + 45*2 - 10, 30 + 45*4 + 20 - 10), CV_RGB(150, 100, 150), 1);
	//cvLine(candidateDemo[targetLabel], cvPoint(23 + 45*2 - 10, 20 + 20), cvPoint(23 + 45*2 - 10, 30 + 45*4 + 20 - 10), CV_RGB(250, 50, 250), 3);
	cvLine(candidateDemo[targetLabel], cvPoint(30 + 45*2 - 10, 20 + 20), cvPoint(30 + 45*2 - 10, 30 + 45*4 + 20 - 10), CV_RGB(150, 100, 150), 1);
	cvLine(candidateDemo[targetLabel], cvPoint(30 + 45*3 - 10, 20 + 20), cvPoint(30 + 45*3 - 10, 30 + 45*4 + 20 - 10), CV_RGB(150, 100, 150), 1);
	cvLine(candidateDemo[targetLabel], cvPoint(30 + 45*4 - 10, 20 + 20), cvPoint(30 + 45*4 - 10, 30 + 45*4 + 20 - 10), CV_RGB(150, 100, 150), 1);
	
	cvNamedWindow(wTitleDemo[targetLabel],1);
	cvResize(candidateDemo[targetLabel], candidateDemo_Large[targetLabel]);

	char number[20];
	sprintf(number, "TARGET HEIGHT: %d", (int)(targetHeight[targetLabel]*1.15));
	cvPutText(candidateDemo_Large[targetLabel], number, cvPoint(15*2-10, 21), &bigFont, CV_RGB(0,255,100));
	sprintf(number, "CAM1 HIST");
	cvPutText(candidateDemo_Large[targetLabel], number, cvPoint(15*2-10, (15 + 20)*2), &font, CV_RGB(0,255,0));
	sprintf(number, "CAM2 HIST");
	cvPutText(candidateDemo_Large[targetLabel], number, cvPoint((15 + 45)*2-10, (15 + 20)*2), &font, CV_RGB(20,100,255));
	sprintf(number, "Candidate#1");
	cvPutText(candidateDemo_Large[targetLabel], number, cvPoint((30 + 45*2)*2-13, (15 + 20)*2), &font, CV_RGB(255,255,255));
	sprintf(number, "Candidate#2");
	cvPutText(candidateDemo_Large[targetLabel], number, cvPoint((30 + 45*3)*2-13, (15 + 20)*2), &font, CV_RGB(255,255,255));
	sprintf(number, "Candidate#3");
	cvPutText(candidateDemo_Large[targetLabel], number, cvPoint((30 + 45*4)*2-13, (15 + 20)*2), &font, CV_RGB(255,255,255));


	cvShowImage(wTitleDemo[targetLabel], candidateDemo_Large[targetLabel]);
}
int targetDatabase::histDisplay(IplImage* img, CvPoint startpt, CvHistogram* hist, DEMO_STATUS status, bool cam1match, bool cam2match, bool cam3match)
{
	//cvCircle(img, cvPoint(20,40), 4, CV_RGB(255,255,255), -1);
	cvRectangle(img, cvPoint(startpt.x-8, startpt.y-8), cvPoint(startpt.x+Cr_BINS+8, startpt.y+Cb_BINS+8), CV_RGB(0, 0, 0), -1);

	double sumCheck = 0;
	for (int id0=0; id0<Cr_BINS; id0++)
	{
		for (int id1=0; id1<Cb_BINS; id1++)
		{
			int val = ((*(cvGetHistValue_2D(hist, id0, id1)))>0) ? 255 : 0;
			sumCheck += val;

			if (!val) continue;

			if ((status == CANDIDATE) && (cam1match || cam2match || cam3match))//yellow
				cvCircle(img, cvPoint(startpt.x + id0, startpt.y + id1), 1, CV_RGB(val, val, 0), -1);
			else if (status == CANDIDATE)//white
				cvCircle(img, cvPoint(startpt.x + id0, startpt.y + id1), 1, CV_RGB(val, val, val), -1);
			else if (status == INPUT1)//green
				cvCircle(img, cvPoint(startpt.x + id0, startpt.y + id1), 1, CV_RGB(0, val, 0), -1);
			else if (status == INPUT2)//blue
				cvCircle(img, cvPoint(startpt.x + id0, startpt.y + id1), 1, CV_RGB(0, 0, val), -1);
			else if (status == INPUT3)//purple
				cvCircle(img, cvPoint(startpt.x + id0, startpt.y + id1), 1, CV_RGB(val, 0, val), -1);
			else
				;
			
		}
	}

	if (!sumCheck) 
	{
		return -1;
	}

	if (status == CANDIDATE)
	{
		if (cam3match) cvRectangle(img, cvPoint(startpt.x-7, startpt.y-7), cvPoint(startpt.x+Cr_BINS+7, startpt.y+Cb_BINS+7), CV_RGB(255, 0, 255), 2);
		if (cam2match) cvRectangle(img, cvPoint(startpt.x-5, startpt.y-5), cvPoint(startpt.x+Cr_BINS+5, startpt.y+Cb_BINS+5), CV_RGB(0, 0, 255), 2);
		if (cam1match) cvRectangle(img, cvPoint(startpt.x-3, startpt.y-3), cvPoint(startpt.x+Cr_BINS+3, startpt.y+Cb_BINS+3), CV_RGB(0, 255, 0), 2);
		
		/*if (!(cam1match||cam2match||cam3match)) */cvRectangle(img, cvPoint(startpt.x-2, startpt.y-2), cvPoint(startpt.x+Cr_BINS+2, startpt.y+Cb_BINS+2), CV_RGB(255, 255, 255), 1);
	}
	else if (status == INPUT1)
		cvRectangle(img, cvPoint(startpt.x-3, startpt.y-3), cvPoint(startpt.x+Cr_BINS+3, startpt.y+Cb_BINS+3), CV_RGB(0, 255, 0), 1);
	else if (status == INPUT2)
		cvRectangle(img, cvPoint(startpt.x-5, startpt.y-5), cvPoint(startpt.x+Cr_BINS+5, startpt.y+Cb_BINS+5), CV_RGB(0, 0, 255), 1);
	else if (status == INPUT3)
		cvRectangle(img, cvPoint(startpt.x-7, startpt.y-7), cvPoint(startpt.x+Cr_BINS+7, startpt.y+Cb_BINS+7), CV_RGB(255, 0, 255), 1);
	else
		;

	return 0;
}

//////////void targetDatabase::Evaluation(IplImage * src)
//////////{
//////////	//cout<<"haha";
//////////	//cvZero(scoreImg);
//////////	double score = 0;
//////////	for (int i=0; i<HEIGHT; i++)//y
//////////	{
//////////		for (int j=0; j<WIDTH; j++)//x
//////////		{
//////////			score = Score(src,/*x*/ j,/*y*/ i);
//////////			/*if (j%50 == 1) 
//////////				j=j;*/
//////////			//if (score > TH_GMM)	
//////////				cvSetReal2D(scoreImg, /*row*/HEIGHT-1-i, /*col*/j, score);
//////////		}
//////////		/*if (i%50 == 1)
//////////			i=i;*/
//////////	}
//////////	fileOut2<<"................start................................"<<endl;
//////////	fileOut2<<"score = "<<Score(src, 160, 120)<<endl;
//////////	fileOut2<<"Kernel Num = "<<targetDatabase_Matrix[119][160].KnlNum<<endl;
//////////	for (int u=0; u<MAX_KERNEL_NUM; u++)
//////////	{
//////////		fileOut2<<"dev["<<u<<"] = "<<targetDatabase_Matrix[119][160].kernel[u].dev<<endl;
//////////		fileOut2<<"weight["<<u<<"] = "<<targetDatabase_Matrix[119][160].kernel[u].weight<<endl;
//////////		fileOut2<<"mean["<<u<<"] = "<<targetDatabase_Matrix[119][160].kernel[u].mean<<endl;
//////////	}
//////////	fileOut2<<".................end........................................."<<endl;
//////////
//////////}
//////////
//////////void targetDatabase::show()
//////////{
//////////	//cvEqualizeHist(scoreImg, scoreImg);
//////////	cvNamedWindow("targetDatabase", 1);
//////////	cvShowImage("targetDatabase", scoreImg);
//////////}
//////////
//////////double targetDatabase::Score(IplImage * src, int x, int y)
//////////{
//////////	double score = 0;
//////////	double weight, mean, dev;
//////////
//////////	for (int h=0; h</*MAX_KERNEL_NUM*/CANDIDATE_NUM; h++)
//////////	{
//////////		if (targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].dev!=0)
//////////		{
//////////			//score += targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].weight[h]*GAUSSIAN(CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x), 
//////////			//	targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].mean[h], targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].dev[h]);
//////////			weight = targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].weight;
//////////			mean = targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].mean;
//////////			dev = targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].dev;
//////////			score += weight * ( GAUSSIAN(cvGetReal2D(src,/*row*/ HEIGHT-1-y ,/*col*/ x), mean, dev) );//CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x), mean, dev) );
//////////		}
//////////		else
//////////		{
//////////			break;
//////////		}
//////////	}
//////////	//save the time of EqualizeHist
//////////	return MAGNIFY_FACTOR*score;
//////////}
//////////
//////////void targetDatabase::Update(IplImage * src)
//////////{
//////////	double dv, dist, weight, mean;
//////////	for (int y=0; y<HEIGHT; y++)
//////////	{
//////////		for (int x=0; x<WIDTH; x++)
//////////		{
//////////			int unmatch_Cnt = 0;
//////////			for (int h=0; h<MAX_KERNEL_NUM; h++)
//////////			{
//////////				dv = targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].dev;
//////////				if (dv!=0)
//////////				{
//////////					//this kernel is already set up
//////////					dist = targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].mean-cvGetReal2D(src,/*row*/ HEIGHT-1-y,/*col*/ x);//CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x);
//////////					if ((dist < 2.5*dv) && (dist > -2.5*dv))
//////////					{
//////////						//match
//////////						//update weight, mean and dev
//////////						weight = targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].weight;
//////////						mean = targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].mean;
//////////						targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].weight = (1-ALPHA)*weight + ALPHA;
//////////						targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].mean = (1-ALPHA)*mean + ALPHA*cvGetReal2D(src,/*row*/ HEIGHT-1-y,/*col*/ x);//CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x);//
//////////						
//////////						//avoid dev conv to zero
//////////						if (dv >= MIN_DEV)
//////////							targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].dev = pow((1-ALPHA)*dv*dv+ALPHA*dist*dist, 0.5);
//////////						else
//////////							targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].dev = MIN_DEV;
//////////
//////////					}
//////////					else
//////////					{
//////////						//unmatched
//////////						//only update weight
//////////						unmatch_Cnt++;
//////////						weight = targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].weight;
//////////						targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].weight = (1-ALPHA)*weight;
//////////					}
//////////
//////////				}
//////////				else
//////////				{
//////////					//reach the end of available kernels 
//////////					//update kernel number
//////////					targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].KnlNum = h;
//////////					break;
//////////				}
//////////				targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].KnlNum = MAX_KERNEL_NUM;
//////////
//////////			}
//////////			if (targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].KnlNum == unmatch_Cnt)//all existing kernels don't match.
//////////			{
//////////				if (unmatch_Cnt == MAX_KERNEL_NUM)//full
//////////				{
//////////					//replace the least trusted kernel with a new one
//////////					qsort(targetDatabase_Matrix[HEIGHT-1-y][x].kernel, MAX_KERNEL_NUM, sizeof(GKernel), compare);
//////////					//replace kernel
//////////					targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[MAX_KERNEL_NUM-1].dev = INIT_HIGH_DEV;
//////////					targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[MAX_KERNEL_NUM-1].mean = cvGetReal2D(src,/*row*/ HEIGHT-1-y,/*col*/ x);//CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x);
//////////					targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[MAX_KERNEL_NUM-1].weight = INIT_LOW_WEIGHT;
//////////				}
//////////				else
//////////				{
//////////					//create an extra kernel
//////////					targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].KnlNum = unmatch_Cnt + 1; 
//////////					targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[unmatch_Cnt].dev = INIT_HIGH_DEV;
//////////					targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[unmatch_Cnt].mean = cvGetReal2D(src, /*row*/HEIGHT-1-y,/*col*/ x);//CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x);
//////////					targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[unmatch_Cnt].weight = INIT_LOW_WEIGHT;
//////////
//////////				}
//////////			}
//////////			else //ranking all existing kernels
//////////			{
//////////				qsort(targetDatabase_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel, MAX_KERNEL_NUM/*targetDatabase_Matrix[HEIGHT-1-y][x].KnlNum*/, sizeof(GKernel), compare);
//////////			}
//////////
//////////		}
//////////	}	
//////////}
