#include "DateInfo.h"

#ifndef _TIME_INFO_H_
#define _TIME_INFO_H_

class TimeInfo : public DateInfo
{
private:
	int mHour;
	int mMin;
	int mSec;


public:
	TimeInfo();
	TimeInfo(int hour, int min, int sec);
	~TimeInfo();

public:
	void setHour(int hour);
	void setMin(int min);
	void setSec(int sec);
	int getHour();
	int getMin();
	int getSec();

};

#endif