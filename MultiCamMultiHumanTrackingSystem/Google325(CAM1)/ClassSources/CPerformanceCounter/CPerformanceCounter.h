#pragma once
//--------------------------------------------------------------
//Performance Counter 類別的原始碼 (宣告及定義)
//Performance Counter Class Source code(Declaration and Definition)
//--------------------------------------------------------------
#pragma once
#include "windows.h"
#include "winbase.h"
//宣告-------------------------------------------------------------------------------
class CPerformanceCounter
{
	public:
		inline bool ResetPeriodController(double);   //重新開始Controller 參數為周期(秒)
		inline bool CheckPeriodController(void);     //檢察是否已到時間(超過true 未到false)
	
	private:
		double dLoopPeriod;      //loop的周期 以秒為單位

	public:
		inline CPerformanceCounter(void);
		inline ~CPerformanceCounter(void);
		inline bool QueryStartTime(LARGE_INTEGER*); //單位是counter number
		inline bool QueryEndTime(LARGE_INTEGER*);	
		inline bool StartCount(void);
		inline bool EndCount(void);
		inline bool QueryFrequency(LARGE_INTEGER*); 
		inline double CalculateElapsedTime(void);   //計算並return StartCount和EndCount的時間差(秒)
	private:
		LARGE_INTEGER liStartTime;
		LARGE_INTEGER liEndTime;
		LARGE_INTEGER liFrequency;
};

//定義---------------------------------------------------------------------------------
//Constructor
inline CPerformanceCounter::CPerformanceCounter(void)
{
	QueryPerformanceFrequency(&liFrequency);
}
//destructor
inline CPerformanceCounter::~CPerformanceCounter(void)
{
}

inline bool CPerformanceCounter::QueryStartTime(LARGE_INTEGER *lipStartTime)  //查詢開始時間
{
	*lipStartTime=liStartTime;
	return true;
}
inline bool CPerformanceCounter::QueryEndTime(LARGE_INTEGER *lipEndTime)  //查詢結束時間
{
	*lipEndTime=liEndTime;
	return true;	
}

inline bool CPerformanceCounter::StartCount(void)   //開始計時(將現在的counter數存入member)
{
	QueryPerformanceCounter(&liStartTime);
	
	return true;
}

inline bool CPerformanceCounter::EndCount(void)     //停止計時(將現在的counter數存入member)
{
	QueryPerformanceCounter(&liEndTime);

	return true;
}

inline bool CPerformanceCounter::QueryFrequency(LARGE_INTEGER *lipFrequency) //查詢頻率
{
	*lipFrequency=liFrequency;

	return true;
}

inline double CPerformanceCounter::CalculateElapsedTime(void)   //計算開始和結束的時間差(單位秒)
{
	double dTime;
	
	dTime=((double)liEndTime.QuadPart-(double)liStartTime.QuadPart)/((double)liFrequency.QuadPart);

    return dTime;
}

inline bool CPerformanceCounter::ResetPeriodController(double dSecond)//重新開始Controller 參數為周期(秒)
{
	dLoopPeriod=dSecond;   //將輸入的周期存入member variable
	StartCount();   //將現在的Count數存在liStartTime
	return true;
}

inline bool CPerformanceCounter::CheckPeriodController(void) //檢察是否已到時間(超過true 未到false)
{
	double dElpasedTime;

	EndCount();     //將現在的count數存在liEndTime

	dElpasedTime=CalculateElapsedTime();
	if(dElpasedTime>dLoopPeriod)
		return true;
	else if(dElpasedTime>0)        //防止counteroverflow
		return false;
	else                           //防止counteroverflow
	{
		ResetPeriodController(dLoopPeriod);
        return false;
    }
	
}