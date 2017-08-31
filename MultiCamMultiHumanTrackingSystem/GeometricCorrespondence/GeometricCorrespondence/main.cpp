#include <stdio.h>
#include <iostream>
using namespace std;
#include <conio.h>
#include <tchar.h>
#include <windows.h>
#include <time.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvcam.h>

#include "Component.h"



int main(int argc, _TCHAR* argv[])
{
	cout << "hello" << endl;
	Position3D opticPt(0,0,220);
	Position3D B(155,227,140);
	Position3D C(168,218,140);
	Position3D D(142,236,140);

	CameraLine3D camline;
	camline.initParamBy3DPt(B, opticPt, B, 0.392338);
	cout << "B(" << camline.imageLoc.x << "," << camline.imageLoc.y << ")\n";


	camline.setBy3DPt(C);
	cout << "C(" << camline.imageLoc.x << "," << camline.imageLoc.y << ")\n";
	camline.setBy3DPt(D);
	cout << "D(" << camline.imageLoc.x << "," << camline.imageLoc.y << ")\n";

	camline.setByImgLoc(cvPoint(160, 120));
	Plane refplane(camline.ref_plane_center, camline.central_dir_vec);
	Position3D refplanept;
	camline.getLinePtOnPlane(refplane, refplanept);
	cout << "B(" << (int)refplanept.x << "," << (int)refplanept.y << "," << (int)refplanept.z << ")\n";
	camline.getLinePtOnPlane(camline.refPlane, refplanept);
	cout << "B(" << (int)refplanept.x << "," << (int)refplanept.y << "," << (int)refplanept.z << ")\n";

	camline.setByImgLoc(cvPoint(200,120));
	camline.getLinePtOnPlane(camline.refPlane, refplanept);
	cout << "D(" << (int)refplanept.x << "," << (int)refplanept.y << "," << (int)refplanept.z << ")\n";

	camline.setByImgLoc(cvPoint(120,120));
	camline.getLinePtOnPlane(camline.refPlane, refplanept);
	cout << "C(" << (int)refplanept.x << "," << (int)refplanept.y << "," << (int)refplanept.z << ")\n";

	camline.setByImgLoc(cvPoint(160,160));
	camline.getLinePtOnPlane(camline.refPlane, refplanept);
	cout << "E(" << (int)refplanept.x << "," << (int)refplanept.y << "," << (int)refplanept.z << ")\n";

	camline.setByImgLoc(cvPoint(160,80));
	camline.getLinePtOnPlane(camline.refPlane, refplanept);
	cout << "F(" << (int)refplanept.x << "," << (int)refplanept.y << "," << (int)refplanept.z << ")\n";

	Line3D line(camline);
	cout << "line Stdpoint1(" << line.Stdpoint1.x << "," << line.Stdpoint1.y << "," << line.Stdpoint1.z << ")\n";

	CameraLine3D camline2(camline);
	camline2.getLinePtOnPlane(camline2.refPlane, refplanept);
	cout << "F(" << (int)refplanept.x << "," << (int)refplanept.y << "," << (int)refplanept.z << ")\n";

	CameraLine3D camlines[2];
	camlines[1] = camlines[0] = camline;
	refplanept.x = 10000;
	camlines[1].getLinePtOnPlane(camlines[1].refPlane, refplanept);
	cout << "F(" << (int)refplanept.x << "," << (int)refplanept.y << "," << (int)refplanept.z << ")\n";

	Position3D pt1(0,0,0);
	Position3D vec1(1,0,0);
	Position3D pt2(100,100,100);
	Position3D vec2(0,1,0);
	Line3D lineA(pt1, vec1);
	Line3D lineB(pt2, vec2);
	cout << "lineA.getDisFromPt(pt2) = " << lineA.getDisFromPt(pt2) << endl;
	bool valid;
	Position3D temppt2;
	Position3D temppt1 = lineA.getMinDisPtFromLine(valid, lineB, temppt2);
	if (valid)
	{
		cout << "temppt1(" << (int)temppt1.x << "," << (int)temppt1.y << "," << (int)temppt1.z << ")\n";
		cout << "temppt2(" << (int)temppt2.x << "," << (int)temppt2.y << "," << (int)temppt2.z << ")\n";
	}
	cout << "Min dist = " << lineA.getDisFromPt(temppt2) << endl;

	Position3D temppt3(temppt1);
	Position3D temppt4(temppt2);
	Position3D temppt5 = temppt3 + temppt4;
	cout << "temppt5(" << (int)temppt5.x << "," << (int)temppt5.y << "," << (int)temppt5.z << ")\n";
	temppt5 = temppt5 - temppt4;
	cout << "temppt5(" << (int)temppt5.x << "," << (int)temppt5.y << "," << (int)temppt5.z << ")\n";
	temppt5 = temppt5 + temppt4;
	cout << "temppt5(" << (int)temppt5.x << "," << (int)temppt5.y << "," << (int)temppt5.z << ")\n";
	temppt5 = temppt5 - temppt4 - temppt4 - temppt4;
	cout << "temppt5(" << (int)temppt5.x << "," << (int)temppt5.y << "," << (int)temppt5.z << ")\n";
	cout << "position(" << (int)position(100,200,300).x << "," << (int)position(100,200,300).y << "," << (int)position(100,200,300).z << ")\n";

	CameraLine3D cl;
	cl.initParamByImageLoc(cvPoint(165, 148), opticPt, B, 0.392338);
	cl.getPosFromZ(180, temppt1);
	cl.setBy3DPt(temppt1);
	cout << "imageLoc(" << cl.imageLoc.x << "," << cl.imageLoc.y << ")\n";

	cl.initParamBy3DPt(position(200, 180, 180), opticPt, B, 0.392338);
	cout << "imageLoc(" << cl.imageLoc.x << "," << cl.imageLoc.y << ")\n";
	cl.getPosFromZ(160, temppt1);
	cl.setBy3DPt(temppt1);
	cout << "imageLoc(" << cl.imageLoc.x << "," << cl.imageLoc.y << ")\n";


	double** colorScoreMap = new double*[MAX_TARGET_NUM];
	int** objConfMap = new int*[MAX_TARGET_NUM];
	int** matchCntMap = new int*[MAX_TARGET_NUM];
	Pair** pairMap = new Pair*[MAX_TARGET_NUM];

	for (int b=0; b<MAX_TARGET_NUM; b++)
	{
		colorScoreMap[b] = new double[MAX_TARGET_NUM];
		objConfMap[b] = new int[MAX_TARGET_NUM];
		matchCntMap[b] = new int[MAX_TARGET_NUM];
		pairMap[b] = new Pair[MAX_TARGET_NUM];

	}

	//initialize the observation integration table
	for (int q=0; q<MAX_TARGET_NUM; q++)
	{
		for (int r=0; r<MAX_TARGET_NUM; r++)
		{
			colorScoreMap[q][r] = NONDEFINED;
			objConfMap[q][r] = 0;
			matchCntMap[q][r] = NONDEFINED;
			pairMap[q][r] = makePair(q, r, NULL, NULL);
		}
	}

	vector<List*> * pairModeListTest = new vector<List*>;
	pairModeListTest->clear();

	objConfMap[1][0] = objConfMap[0][1] = objConfMap[0][0] = objConfMap[1][1] = objConfMap[2][2] = objConfMap[3][3] = objConfMap[4][4] = 4;
	colorScoreMap[0][1] = colorScoreMap[0][0] = colorScoreMap[1][1] = colorScoreMap[2][2] = colorScoreMap[3][3] = colorScoreMap[4][4] = 1;
	colorScoreMap[1][0] = 0.3;

	Rule rule;

	double** CSM = (double**)colorScoreMap;
	int** OCM = (int**)objConfMap;

	findAllPairMode(0, rule, NULL, CSM, OCM, NULL, NULL, pairModeListTest);

	vector<List*> copyList = *pairModeListTest;

	cout << "\n\n\n\n";

	int cnt = 0;
	int maxlength = 0;// want bigger
	double bestScore = 100;//want smaller
	vector<List*> * pairModeList_MAXL = new vector<List*>;
	List bestList;
	pairModeList_MAXL->clear();
	for (; !pairModeListTest->empty();)
	{
		cnt++;
		cout << "length = " << pairModeListTest->front()->length << ", " << "totalscore = " << pairModeListTest->front()->totalScore << " : ";
		if (pairModeListTest->front()->length >= maxlength)
		{
			if (pairModeListTest->front()->length > maxlength)
			{
				pairModeList_MAXL->clear();
				pairModeList_MAXL->push_back(pairModeListTest->front());
				maxlength = pairModeListTest->front()->length;
				bestScore = pairModeListTest->front()->totalScore;
				bestList = *pairModeListTest->front();
			}
			else
			{
				pairModeList_MAXL->push_back(pairModeListTest->front());
				if (pairModeListTest->front()->totalScore < bestScore)
				{
					bestScore = pairModeListTest->front()->totalScore;
					bestList = *pairModeListTest->front();
				}
			}
		}
		for (; !pairModeListTest->front()->pairList.empty(); )
		{
			cout << "(" << pairModeListTest->front()->pairList.front().i << ", " << pairModeListTest->front()->pairList.front().j << ")  ";
			pairModeListTest->front()->pairList.erase(pairModeListTest->front()->pairList.begin());
		}
		cout << "..." << endl;
		//delete pairModeListTest->front();
		pairModeListTest->erase(pairModeListTest->begin());
	}

	cout << "--------------------------------------------------------\n" "bestScore = " << bestScore << endl;
	cout << "bestList length = " << bestList.length << endl;
	for (; !bestList.pairList.empty(); )
	{
		cout << "(" << bestList.pairList.front().i << ", " << bestList.pairList.front().j << ")  ";
		bestList.pairList.erase(bestList.pairList.begin());
	}


	for (; !copyList.empty();)
	{
		delete copyList.front();
		//cout << "delete front! ";
		copyList.erase(copyList.begin());
	}
	cout << endl;


	cout << "\ncnt = " << cnt << endl;


	bool *AliveCAM1 = new bool[MAX_TARGET_NUM], *AliveCAM2 = new bool[MAX_TARGET_NUM];
	
	//initialize the observation integration table
	for (int q=0; q<MAX_TARGET_NUM; q++)
	{
		AliveCAM1[q] = AliveCAM2[q] = false;
		for (int r=0; r<MAX_TARGET_NUM; r++)
		{
			objConfMap[q][r] = 0;
		}
	}

	//show initial table
	fprintf(stderr, "initial \n");
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");
	
	// stage 1: starting conditions
	fprintf(stderr, "\nstage 1 \n");
	AliveCAM1[0] = AliveCAM1[2] = AliveCAM1[5] = true;
	AliveCAM2[1] = AliveCAM2[2] = AliveCAM2[3] = true;
	if (observationIntegrate(objConfMap, 2, 3, 4, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");

	// stage 2: D disappeared
	fprintf(stderr, "\nstage 2 \n");
	AliveCAM2[3] = false;
	if (observationDisappear(objConfMap, CAM2, 3, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");

	// stage 3: 1 shows as A
	fprintf(stderr, "\nstage 3 \n");
	AliveCAM2[0] = true;
	if (observationIntegrate(objConfMap, 0, 0, 3, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");

	// stage 4: B is recognized as 6
	fprintf(stderr, "\nstage 4 \n");
	if (observationIntegrate(objConfMap, 5, 1, 2, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");
	
	// stage 5: B disappeared
	fprintf(stderr, "\nstage 5 \n");
	AliveCAM2[1] = false;
	if (observationDisappear(objConfMap, CAM2, 1, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");

	// stage 6: D shows up indep
	fprintf(stderr, "\nstage 6 \n");
	AliveCAM2[3] = true;
	if (observationShowIndep(objConfMap, CAM2, 3, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");

	// stage 7: 5 shows up indep
	fprintf(stderr, "\nstage 7 \n");
	AliveCAM1[4] = true;
	if (observationShowIndep(objConfMap, CAM1, 4, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");

	// stage 8: 5 is recognized as D
	fprintf(stderr, "\nstage 8 \n");
	if (observationIntegrate(objConfMap, 4, 3, 1, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");

	// stage 9: 3B2 5D4 6C3 is recognized as D
	AliveCAM1[0] = AliveCAM1[2] = AliveCAM1[4] = AliveCAM1[5] = true;
	AliveCAM2[0] = AliveCAM2[1] = AliveCAM2[2] = AliveCAM2[3] = true;
	fprintf(stderr, "\nstage 9 \n");
	if (observationShowIndep(objConfMap, CAM2, 1, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	if (observationIntegrate(objConfMap, 4, 3, 4, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	if (observationIntegrate(objConfMap, 5, 2, 3, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	if (observationIntegrate(objConfMap, 2, 1, 2, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");

	// stage 10: 2 shows up, want to be recognized as B, but fails.
	AliveCAM1[1] = true;
	fprintf(stderr, "\nstage 10 \n");
	if (observationShowIndep(objConfMap, CAM1, 1, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	if (observationIntegrate(objConfMap, 1, 1, 2, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");

	// stage 11: 6 is re-recognized as B, Lv = 4.
	fprintf(stderr, "\nstage 11 \n");
	if (observationIntegrate(objConfMap, 5, 1, 4, AliveCAM1, AliveCAM2) == RETURN_CONTRADICT)
	{
		fprintf(stderr, "problem here! \n");
		system("pause");
		return 0;
	}
	printConfMapOnConsole(objConfMap, AliveCAM1, AliveCAM2);
	system("pause");

	return 0;
}






