#ifndef __H_ENUMTYPE__
#define __H_ENUMTYPE__

enum SampleStatus
{
	SAMPLE_NONE,
	SAMPLE_START,
	SAMPLE_BREAK, // provide additional info. for HMM segmentation
	SAMPLE_INTERMEDIATE,
	SAMPLE_END,
};

enum ControlMode
{
	EIGHT_DIRECTION = 0,
	CIRCLE,
	TRIANGLE,
	RECTANGLE
};

enum SampleSource
{
	VISION,
	MOUSE_TRACK,
};

enum PM_STATUS
{
	OTHER_ACTION,
	RECOGNIZING,
	TRAINING
};

// FOR recognize(), training(), addNewDirective()
enum StatusCode
{
	None_Status,
	Finished, // recognition finished
	HMMModelNotCreated,
	NoModelChecked,
	SampleNotReady,
	TooFewSamples,
	DuplicateName
};

enum Direction // MIN_DIRECTION must start with 0
{
	NONE_DIRECTION = -1,
	MIN_DIRECTION, // 0

	UP = MIN_DIRECTION,
	UP_RIGHT,
	RIGHT,
	DOWN_RIGHT,
	DOWN,
	DOWN_LEFT,
	LEFT,
	UP_LEFT,

	MAX_DIRECTION = UP_LEFT
};

enum Shape // MIN_SHAPE must start with 0
{
	NONE_SHAPE = -1,
	MIN_SHAPE, // 0

	SMALL_CIRCLE = MIN_SHAPE,
	MEDIUM_CIRCLE,
	LARGE_CIRCLE,

	SMALL_TRIANGLE,
	MEDIUM_TRIANGLE,
	LARGE_TRIANGLE,

	SMALL_RECTANGLE,
	MEDIUM_RECTANGLE,
	LARGE_RECTANGLE,

	MAX_SHAPE = LARGE_RECTANGLE,

	// important
	CIRCLE_START = SMALL_CIRCLE,
	TRIANGLE_START = SMALL_TRIANGLE,
	RECTANGLE_START = SMALL_RECTANGLE,
};


enum QuantizerType
{
	None_QuantizerType,
	Default,
	HMMDefault,
	WeightAverage,
	HMMWeightAverage,
	WeightAveRetainLong,
	WeightAveAcuteAngle
};

//////////////////////// for particle track

enum DrawMode
{
	DRAW_POINT,
	DRAW_LINE,
	DRAW_FILL
};

#endif