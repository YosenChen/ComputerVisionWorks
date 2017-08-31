#include "ProcessState.h"

void ProcessState::changeState(ProcessState* state )
{ 
	OpticalFlow::getSingletonPtr()->changeState(state); 
}

void ProcessState::pushState(ProcessState* state) 
{
	OpticalFlow::getSingletonPtr()->pushState(state);
}

void ProcessState::popState() 
{ 
	OpticalFlow::getSingletonPtr()->popState(); 
}
