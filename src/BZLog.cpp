//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Log.cpp
// @brief: ISP log source file
//////////////////////////////////////////////////////////////////////////////////////

#include "BZLog.h"
#include <stdio.h>

//using namespace std;
int BZLogAddInfo(const char* str, ...)
{
#if LOG_ON
	va_list(va);
	__crt_va_start(va, str);
	BZLogAddTime(str, va);
	__crt_va_end(va);
#endif
	return 0;
}

void BZLogAddTime(const char* str, va_list va)
{
	char years[4] = { '0', '0', '0', '0' };
	char monthes[2] = { '0', '0' };
	char days[2] = { '0', '0' };
	char milliseconds[3] = { '0','0','0' };
	char seconds[2] = { '0', '0' };
	char minutes[2] = { '0', '0' };
	char hours[2] = { '0', '0' };

	GetTimeWithDateChar(years, monthes, days, hours, minutes, seconds, milliseconds);
	char strBuffer[LOG_BUFFER_LEFT_SIZE];
	snprintf(strBuffer, LOG_BUFFER_LEFT_SIZE, "%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c:%c%c%c %s",
		years[0], years[1], years[2], years[3],
		monthes[0], monthes[1],
		days[0], days[1],
		hours[0], hours[1],
		minutes[0], minutes[1],
		seconds[0], seconds[1],
		milliseconds[0], milliseconds[1], milliseconds[2],
		str);

	BZLogPrint(strBuffer, va);
}

void BZLogPrint(const char* str, va_list va)
{
	char strBuffer[LOG_BUFFER_SIZE];
	vsnprintf(strBuffer, LOG_BUFFER_SIZE - LOG_BUFFER_PERSERVE_SIZE, str, va);
	strBuffer[LOG_BUFFER_SIZE - 2] = '\0';
	strBuffer[LOG_BUFFER_SIZE - 1] = '\n';
	printf("%s\n", strBuffer);
}


char BZTimeInt2Char(int32_t i)
{
	char result = 'x';

	if (i >= 0 && i <= 9) {
		result = '0' + i;
	}

	return result;
}

void GetTimeWithDateInt(int32_t* years, int32_t* months, int32_t* days, int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds)
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

void GetTimeWithDateChar(char* years, char* months, char* days, char* hours, char* minutes, char* seconds, char* milliseconds)
{
	int32_t dy = 0, dm = 0, dd = 0, tms = 0, ts = 0, tm = 0, th = 0;

	GetTimeWithDateInt(&dy, &dm, &dd, &th, &tm, &ts, &tms);

	*years = BZTimeInt2Char(dy / 1000);
	*(years + 1) = BZTimeInt2Char(dy / 100 % 10);
	*(years + 2) = BZTimeInt2Char(dy / 10 % 10);
	*(years + 3) = BZTimeInt2Char(dy % 10);

	*months = BZTimeInt2Char(dm / 10);
	*(months + 1) = BZTimeInt2Char(dm % 10);

	*months = BZTimeInt2Char(dm / 10);
	*(months + 1) = BZTimeInt2Char(dm % 10);

	*days = BZTimeInt2Char(dd / 10);
	*(days + 1) = BZTimeInt2Char(dd % 10);

	*minutes = BZTimeInt2Char(tm / 10);
	*(minutes + 1) = BZTimeInt2Char(tm % 10);

	*seconds = BZTimeInt2Char(ts / 10);
	*(seconds + 1) = BZTimeInt2Char(ts % 10);

	*milliseconds = BZTimeInt2Char(tms / 100);
	*(milliseconds + 1) = BZTimeInt2Char(tms / 10 % 10);
	*(milliseconds + 2) = BZTimeInt2Char(tms % 10);
}