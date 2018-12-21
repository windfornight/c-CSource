#include "TimeInfo.h"

TimeInfo::TimeInfo():DateInfo(2000, 1, 1)
{
	mHour = 0;
	mMin = 0;
	mSec = 0;
}

TimeInfo::TimeInfo(int hour, int min, int sec)
{
	//DateInfo::DateInfo(2003, 12, 12); /*´íÎóµÄ³õÊ¼»¯*/
	mHour = hour;
	mMin  = min;
	mSec = sec;
}

TimeInfo::~TimeInfo()
{

}

void TimeInfo::setHour(int hour)
{
	mHour = hour;
}

void TimeInfo::setMin(int min)
{
	mMin = min;
}

void TimeInfo::setSec(int sec)
{
	mSec = sec;
}

int TimeInfo::getHour()
{
	return mHour;
}

int TimeInfo::getMin()
{
	return mMin;
}

int TimeInfo::getSec()
{
	return mSec;
}