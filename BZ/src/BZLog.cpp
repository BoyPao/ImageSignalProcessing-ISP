// License: GPL-3.0-or-later
/*
 * BoZhi log supports.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "BZLog.h"
#include <stdio.h>

int32_t LogBase(const char* str, ...)
{
	va_list va;
	va_start(va, str);
	LogPrint(str, va);
	va_end(va);
	return 0;
}

void LogPrint(const char* str, va_list va)
{
	char strBuffer[LOG_BUFFER_SIZE];
	vsnprintf(strBuffer, LOG_BUFFER_SIZE - LOG_BUFFER_PERSERVE_SIZE, str, va);
	strBuffer[LOG_BUFFER_SIZE - 2] = '\0';
	strBuffer[LOG_BUFFER_SIZE - 1] = '\n';
	printf("%s\n", strBuffer);
}
