//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Utils.cpp
// @brief: Common fuction source file.
//////////////////////////////////////////////////////////////////////////////////////

#include "Utils.h"

using namespace std;

char TimeInt2Char(int32_t i)
{
	char result = 'x';

	if (i >= 0 && i <= 9) {
		result = '0' + i;
	}

	return result;
}

void getTimeChar(char* hours, char* minutes, char* seconds, char* milliseconds)
{
	int32_t ms = 0, s = 0, m = 0, h = 0;

	getTimeInt(&h, &m, &s, &ms);

	*hours = TimeInt2Char(h / 10);
	*(hours + 1) = TimeInt2Char(h % 10);

	*minutes = TimeInt2Char(m / 10);
	*(minutes + 1) = TimeInt2Char(m % 10);

	*seconds = TimeInt2Char(s / 10);
	*(seconds + 1) = TimeInt2Char(s % 10);

	*milliseconds = TimeInt2Char(ms / 100);
	*(milliseconds + 1) = TimeInt2Char(ms / 10 % 10);
	*(milliseconds + 2) = TimeInt2Char(ms % 10);
}

void getTimeInt(int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds)
{
	auto c_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	*milliseconds = c_time.count() % 1000;
	*seconds = c_time.count() / 1000 % 60;
	*minutes = c_time.count() / 1000 / 60 % 60;
	*hours = c_time.count() / 1000 / 60 / 60 % 24 + LOCAL_TIME_ZOOM_OFFSET;
}

void getTimeWithDateInt(int32_t* years, int32_t* months, int32_t* days, int32_t* hours, int32_t* minutes, int32_t* seconds)
{
	int32_t milliseconds = 0;
	getTimeWithDateInt(years, months, days, hours, minutes, seconds, &milliseconds);
}

void getTimeWithDateInt(int32_t* years, int32_t* months, int32_t* days, int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds)
{
	auto tp = std::chrono::system_clock::now();
	auto ms_time = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());

	std::time_t systemTime = std::chrono::system_clock::to_time_t(tp);
	tm pTime;
	gmtime_s(&pTime, &systemTime);

	*milliseconds = ms_time.count() % 1000;
	*seconds = pTime.tm_sec;
	*minutes = pTime.tm_min;
	*hours = pTime.tm_hour + LOCAL_TIME_ZOOM_OFFSET;
	*days = pTime.tm_mday;
	*months = pTime.tm_mon + SYSTEM_MONTH_OFFSET;
	*years = pTime.tm_year + SYSTEM_YEAR_OFFSET;
}

