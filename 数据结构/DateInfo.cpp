#include "DateInfo.h"

DateInfo::DateInfo():mYear(2018), mMonth(12), mDay(19) 
{

}

DateInfo::DateInfo(int year, int month, int day):mYear(year)
{
	mMonth = month;
	mDay = day;
}

DateInfo::~DateInfo()
{

}

void DateInfo::setYear(int year)
{
	mYear = year;
}

int DateInfo::getYear()
{
	return mYear;
}

void DateInfo::setMonth(int month)
{
	mMonth = month;
}

int DateInfo::getMonth()
{
	return mMonth;
}

void DateInfo::setDay(int Day)
{
	mDay = Day;
}

int DateInfo::getDay()
{
	return mDay;
}
