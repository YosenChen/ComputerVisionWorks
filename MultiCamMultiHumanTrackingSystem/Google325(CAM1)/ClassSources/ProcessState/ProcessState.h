#ifndef __H_ProcessState__
#define __H_ProcessState__

#include "OpticalFlow.h"
#include <fstream>
#include <sstream>

using namespace std;

class OpticalFlow;

class ProcessState
{
public:
	virtual void enter( OpticalFlow* SingletonInstancePtr ) = 0;
	virtual void exit() = 0;

	virtual void pause() = 0;
	virtual void resume() = 0;

	virtual bool execute( IplImage* imageSource ) = 0;

	void changeState(ProcessState* state );
	void pushState(ProcessState* state);
	void popState();

protected:
	OpticalFlow* mSingletonInstancePtr;

	ProcessState()
	{
	}

	virtual ~ProcessState()
	{
	}
};

#endif