#ifndef TRACKING_STATE_H 
#define TRACKING_STATE_H

enum TRACKING_RESULT
{
	RE_INITIALIZATION,
	LOSING_OBJECT,
	FINE
};

enum SENDING_MODE
{
	DONT_CARE,
	REGRESSION,
	DISCONNECT,
	TEST_ALIVE
};

#endif