#ifndef CAM_PLL_TRACKING_H
#define CAM_PLL_TRACKING_H

#include <iostream>
#include <ctime>
#include <windows.h>
#include <process.h> //_beginthreadex
#include <fstream>
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "highgui.h"


unsigned parallelTracking( void *pMyID )
{
	char    *MyID = (char*)pMyID;
	int id=(int)*MyID;
	switch (id)
	{
	case 1:
		while(1) cvWaitKey(100);
		break;

	case 2:
		while(1) cvWaitKey(100);
		break;

	case 3:
		while(1) cvWaitKey(100);
		break;

	case 4:
		while(1) cvWaitKey(100);
		break;


	default:
		;
	}
	return 0;
}

#endif
