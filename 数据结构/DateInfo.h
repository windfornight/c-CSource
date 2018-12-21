#ifndef _DATE_INFO_H_
#define _DATE_INFO_H_

class DateInfo
{
private:
	int mYear;
	int mMonth;
	int mDay;


public:
	DateInfo();
	DateInfo(int year, int month, int day);
	~DateInfo();

public:
	void setYear(int year);
	int getYear();
	void setMonth(int month);
	int getMonth();
	void setDay(int day);
	int getDay();
};

#endif