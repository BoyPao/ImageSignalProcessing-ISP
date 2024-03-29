// License: GPL-3.0-or-later
/*
 * Common fuctions supports ISP.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "Utils.h"

using namespace std;

char Int2Char(int32_t i)
{
	char result = 'x';

	if (i >= 0 && i <= 9) {
		result = '0' + i;
	}

	return result;
}

int32_t Char2Int(char c)
{
	int32_t result = -1;

	if (c >= '0' && c <= '9') {
		result = c - '0';
	}

	return result;
}

void getTimeChar(char* hours, char* minutes, char* seconds, char* milliseconds)
{
	int32_t ms = 0, s = 0, m = 0, h = 0;

	getTimeInt(&h, &m, &s, &ms);

	*hours = Int2Char(h / 10);
	*(hours + 1) = Int2Char(h % 10);

	*minutes = Int2Char(m / 10);
	*(minutes + 1) = Int2Char(m % 10);

	*seconds = Int2Char(s / 10);
	*(seconds + 1) = Int2Char(s % 10);

	*milliseconds = Int2Char(ms / 100);
	*(milliseconds + 1) = Int2Char(ms / 10 % 10);
	*(milliseconds + 2) = Int2Char(ms % 10);
}

void getTimeInt(int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds)
{
	auto c_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	*milliseconds = c_time.count() % 1000;
	*seconds = c_time.count() / 1000 % 60;
	*minutes = c_time.count() / 1000 / 60 % 60;
	*hours = c_time.count() / 1000 / 60 / 60 % 24 + LOCAL_TIME_ZOOM_OFFSET;
}


void getDateInt(int32_t* years, int32_t* months, int32_t* days)
{
	int32_t hours = 0;
	int32_t minutes = 0;
	int32_t seconds = 0;
	int32_t milliseconds = 0;
	getTimeWithDateInt(years, months, days, &hours, &minutes, &seconds, &milliseconds);
}

void getTimeWithDateInt(int32_t* years, int32_t* months, int32_t* days, int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds)
{
	auto tp = std::chrono::system_clock::now();
	auto ms_time = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());

	std::time_t systemTime = std::chrono::system_clock::to_time_t(tp);
	tm* pTime;
	pTime = gmtime(&systemTime);

	*milliseconds = ms_time.count() % 1000;
	*seconds = pTime->tm_sec;
	*minutes = pTime->tm_min;
	*hours = pTime->tm_hour + LOCAL_TIME_ZOOM_OFFSET;
	*days = pTime->tm_mday;
	*months = pTime->tm_mon + SYSTEM_MONTH_OFFSET;
	*years = pTime->tm_year + SYSTEM_YEAR_OFFSET;
}

void getTimeWithDateChar(char* years, char* months, char* days, char* hours, char* minutes, char* seconds, char* milliseconds)
{
	int32_t dy = 0, dm = 0, dd = 0, tms = 0, ts = 0, tm = 0, th = 0;

	getTimeWithDateInt(&dy, &dm, &dd, &th, &tm, &ts, &tms);

	*years = Int2Char(dy / 1000);
	*(years + 1) = Int2Char(dy / 100 % 10);
	*(years + 2) = Int2Char(dy / 10 % 10);
	*(years + 3) = Int2Char(dy % 10);

	*months = Int2Char(dm / 10);
	*(months + 1) = Int2Char(dm % 10);

	*months = Int2Char(dm / 10);
	*(months + 1) = Int2Char(dm % 10);

	*days = Int2Char(dd / 10);
	*(days + 1) = Int2Char(dd % 10);

	*minutes = Int2Char(tm / 10);
	*(minutes + 1) = Int2Char(tm % 10);

	*seconds = Int2Char(ts / 10);
	*(seconds + 1) = Int2Char(ts % 10);

	*milliseconds = Int2Char(tms / 100);
	*(milliseconds + 1) = Int2Char(tms / 10 % 10);
	*(milliseconds + 2) = Int2Char(tms % 10);
}


int32_t CharNum2IntNum(char* pC)
{
	int32_t rt = 0;
	int32_t len = 0;

	if (!pC)
		return -1;

	while(pC[len] != '\0') {
		len++;
	}

	for(int32_t i = len - 1; i >= 0; i--) {
		if (Char2Int(pC[i]) == -1) {
			return -1;
		} else {
			rt += Char2Int(pC[i]) * pow(10, (len - 1 - i));
		}
	}

	return rt;
}
