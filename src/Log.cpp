// License: GPL-3.0-or-later
/*
 * ISP log supports.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "Log.h"
#include <stdio.h>

#define LOG_NEED_EXT_INFO true

void LogBase(const char* str, ...)
{
	bool needExtInfo = LOG_NEED_EXT_INFO;

	va_list va;
	va_start(va, str);
	LogAddInfo(needExtInfo, str, va);
	va_end(va);
}

void LogAddInfo(bool needExtInfo, const char* str, va_list va)
{
	if (needExtInfo) {
		char strBuffer[LOG_BUFFER_LEFT_SIZE];
		char years[4] = { '0', '0', '0', '0' };
		char monthes[2] = { '0', '0' };
		char days[2] = { '0', '0' };
		char milliseconds[3] = { '0','0','0' };
		char seconds[2] = { '0', '0' };
		char minutes[2] = { '0', '0' };
		char hours[2] = { '0', '0' };

		getTimeWithDateChar(years, monthes, days, hours, minutes, seconds, milliseconds);
		snprintf(strBuffer, LOG_BUFFER_LEFT_SIZE, "%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c:%c%c%c %s",
				years[0], years[1], years[2], years[3],
				monthes[0], monthes[1],
				days[0], days[1],
				hours[0], hours[1],
				minutes[0], minutes[1],
				seconds[0], seconds[1],
				milliseconds[0], milliseconds[1], milliseconds[2],
				str);
		LogPrint(strBuffer, va);
	} else {
		LogPrint(str, va);
	}
}

void LogPrint(const char* str, va_list va)
{
	char strBuffer[LOG_BUFFER_SIZE];
	vsnprintf(strBuffer, LOG_BUFFER_SIZE - LOG_BUFFER_PERSERVE_SIZE, str, va);
	strBuffer[LOG_BUFFER_SIZE - 2] = '\0';
	strBuffer[LOG_BUFFER_SIZE - 1] = '\n';
	printf("%s\n", strBuffer);
}
