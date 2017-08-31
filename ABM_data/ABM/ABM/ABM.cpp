#include "ABM.h"

#define GAUSSIAN(val, m, devi) exp(-pow((val-m),2)/(2*devi*devi))/(pow(2*PI, 0.5)*devi)

fstream fileOut2;
IplImage * scoreImg = cvCreateImage(cvSize(WIDTH,HEIGHT),8,1);

int compare (const void * a, const void * b)
{
  /*return ( *(int*)a - *(int*)b );*/
	if ((((GKernel*)a)->dev) == 0 && (((GKernel*)b)->dev) == 0) return 0;
	if ((((GKernel*)a)->dev) == 0) return 1;
	if ((((GKernel*)b)->dev) == 0) return -1;

	return (((((GKernel*)b)->weight)/(((GKernel*)b)->dev) - (((GKernel*)a)->weight)/(((GKernel*)a)->dev) > 0) ? 1 : -1);
}


ABM::ABM()
{
	//ABM_Matrix = new GMM[HEIGHT][WIDTH];
	ABM_Matrix = new GMM*[HEIGHT];
	for (int i=0; i<HEIGHT; i++)
	{
		ABM_Matrix[i] = new GMM[WIDTH];
	}
	fileOut2.open("error report.txt",ios::out);
	cvZero(scoreImg);

}
ABM::~ABM()
{
	if (ABM_Matrix!=NULL)
	{
		for (int i=0; i<HEIGHT; i++)
		{
			if (ABM_Matrix[i]!=NULL)
			{
				delete [] ABM_Matrix[i];
				ABM_Matrix[i] = NULL;
			}

		}
		delete [] ABM_Matrix;
		ABM_Matrix = NULL;
	}
}

ABM::ABM(const ABM & abm)
{
	//alpha = abm.alpha;
	//ABM_Matrix = new GMM[HEIGHT][WIDTH];
	ABM_Matrix = new GMM*[HEIGHT];
	for (int i=0; i<HEIGHT; i++)
	{
		ABM_Matrix[i] = new GMM[WIDTH];
	}


	for (int i=0; i<HEIGHT; i++)
	{
		for (int j=0; j<WIDTH; j++)
		{
			ABM_Matrix[i][j] = abm.ABM_Matrix[i][j];
		}
	}
}
ABM & ABM::operator=(const ABM & rhs)
{
	//dtor
	if (ABM_Matrix!=NULL)
	{
		for (int i=0; i<HEIGHT; i++)
		{
			if (ABM_Matrix[i]!=NULL)
			{
				delete [] ABM_Matrix[i];
				ABM_Matrix[i] = NULL;
			}

		}
		delete [] ABM_Matrix;
		ABM_Matrix = NULL;
	}

	//cctor
	//alpha = rhs.alpha;
	//ABM_Matrix = new GMM[HEIGHT][WIDTH];
	ABM_Matrix = new GMM*[HEIGHT];
	for (int i=0; i<HEIGHT; i++)
	{
		ABM_Matrix[i] = new GMM[WIDTH];
	}
	
	for (int i=0; i<HEIGHT; i++)
	{
		for (int j=0; j<WIDTH; j++)
		{
			ABM_Matrix[i][j] = rhs.ABM_Matrix[i][j];
		}
	}

	//
	return *this;


}
void ABM::Evaluation(IplImage * src)
{
	//cout<<"haha";
	//cvZero(scoreImg);
	double score = 0;
	for (int i=0; i<HEIGHT; i++)//y
	{
		for (int j=0; j<WIDTH; j++)//x
		{
			score = Score(src,/*x*/ j,/*y*/ i);
			/*if (j%50 == 1) 
				j=j;*/
			//if (score > TH_GMM)	
				cvSetReal2D(scoreImg, /*row*/HEIGHT-1-i, /*col*/j, score);
		}
		/*if (i%50 == 1)
			i=i;*/
	}
	fileOut2<<"................start................................"<<endl;
	fileOut2<<"score = "<<Score(src, 160, 120)<<endl;
	fileOut2<<"Kernel Num = "<<ABM_Matrix[119][160].KnlNum<<endl;
	for (int u=0; u<MAX_KERNEL_NUM; u++)
	{
		fileOut2<<"dev["<<u<<"] = "<<ABM_Matrix[119][160].kernel[u].dev<<endl;
		fileOut2<<"weight["<<u<<"] = "<<ABM_Matrix[119][160].kernel[u].weight<<endl;
		fileOut2<<"mean["<<u<<"] = "<<ABM_Matrix[119][160].kernel[u].mean<<endl;
	}
	fileOut2<<".................end........................................."<<endl;

}

void ABM::show()
{
	//cvEqualizeHist(scoreImg, scoreImg);
	cvNamedWindow("ABM", 1);
	cvShowImage("ABM", scoreImg);
}

double ABM::Score(IplImage * src, int x, int y)
{
	double score = 0;
	double weight, mean, dev;

	for (int h=0; h</*MAX_KERNEL_NUM*/CANDIDATE_NUM; h++)
	{
		if (ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].dev!=0)
		{
			//score += ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].weight[h]*GAUSSIAN(CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x), 
			//	ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].mean[h], ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].dev[h]);
			weight = ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].weight;
			mean = ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].mean;
			dev = ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].dev;
			score += weight * ( GAUSSIAN(cvGetReal2D(src,/*row*/ HEIGHT-1-y ,/*col*/ x), mean, dev) );//CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x), mean, dev) );
		}
		else
		{
			break;
		}
	}
	//save the time of EqualizeHist
	return MAGNIFY_FACTOR*score;
}

void ABM::Update(IplImage * src)
{
	double dv, dist, weight, mean;
	for (int y=0; y<HEIGHT; y++)
	{
		for (int x=0; x<WIDTH; x++)
		{
			int unmatch_Cnt = 0;
			for (int h=0; h<MAX_KERNEL_NUM; h++)
			{
				dv = ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].dev;
				if (dv!=0)
				{
					//this kernel is already set up
					dist = ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].mean-cvGetReal2D(src,/*row*/ HEIGHT-1-y,/*col*/ x);//CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x);
					if ((dist < 2.5*dv) && (dist > -2.5*dv))
					{
						//match
						//update weight, mean and dev
						weight = ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].weight;
						mean = ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].mean;
						ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].weight = (1-ALPHA)*weight + ALPHA;
						ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].mean = (1-ALPHA)*mean + ALPHA*cvGetReal2D(src,/*row*/ HEIGHT-1-y,/*col*/ x);//CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x);//
						
						//avoid dev conv to zero
						if (dv >= MIN_DEV)
							ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].dev = pow((1-ALPHA)*dv*dv+ALPHA*dist*dist, 0.5);
						else
							ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].dev = MIN_DEV;

					}
					else
					{
						//unmatched
						//only update weight
						unmatch_Cnt++;
						weight = ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].weight;
						ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[h].weight = (1-ALPHA)*weight;
					}

				}
				else
				{
					//reach the end of available kernels 
					//update kernel number
					ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].KnlNum = h;
					break;
				}
				ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].KnlNum = MAX_KERNEL_NUM;

			}
			if (ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].KnlNum == unmatch_Cnt)//all existing kernels don't match.
			{
				if (unmatch_Cnt == MAX_KERNEL_NUM)//full
				{
					//replace the least trusted kernel with a new one
					qsort(ABM_Matrix[HEIGHT-1-y][x].kernel, MAX_KERNEL_NUM, sizeof(GKernel), compare);
					//replace kernel
					ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[MAX_KERNEL_NUM-1].dev = INIT_HIGH_DEV;
					ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[MAX_KERNEL_NUM-1].mean = cvGetReal2D(src,/*row*/ HEIGHT-1-y,/*col*/ x);//CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x);
					ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[MAX_KERNEL_NUM-1].weight = INIT_LOW_WEIGHT;
				}
				else
				{
					//create an extra kernel
					ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].KnlNum = unmatch_Cnt + 1; 
					ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[unmatch_Cnt].dev = INIT_HIGH_DEV;
					ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[unmatch_Cnt].mean = cvGetReal2D(src, /*row*/HEIGHT-1-y,/*col*/ x);//CV_IMAGE_ELEM(src, int, /*row*/HEIGHT-1-y , /*col*/x);
					ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel[unmatch_Cnt].weight = INIT_LOW_WEIGHT;

				}
			}
			else //ranking all existing kernels
			{
				qsort(ABM_Matrix[/*row*/HEIGHT-1-y][/*col*/x].kernel, MAX_KERNEL_NUM/*ABM_Matrix[HEIGHT-1-y][x].KnlNum*/, sizeof(GKernel), compare);
			}

		}
	}	
}
