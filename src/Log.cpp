#include "Log.h"
#include <stdio.h>

//using namespace std;
int LogAddInfo(const char* str, ...)
{
#if LOG_ON
	va_list(va);
	__crt_va_start(va, str);
	LogAddTime(str, va);
	__crt_va_end(va);
#endif
	return 0;
}

void LogAddTime(const char* str, va_list va)
{
	char milliseconds[3] = { '0','0','0' };
	char seconds[2] = { '0', '0' };
	char minutes[2] = { '0', '0' };
	char hours[2] = { '0', '0' };

	getTimeChar(hours, minutes, seconds, milliseconds);

	char strBuffer[AddTime_BufferSize];
	snprintf(strBuffer, AddTime_BufferSize, "[%c%c:%c%c:%c%c:%c%c%c]:%s",
		hours[0], hours[1],
		minutes[0], minutes[1],
		seconds[0], seconds[1],
		milliseconds[0], milliseconds[1], milliseconds[2],
		str);

	LogPrint(strBuffer, va);
}

void LogPrint(const char* str, va_list va)
{
	char strBuffer[256];
	vsnprintf(strBuffer, 254, str, va);
	strBuffer[254] = '\0';
	strBuffer[255] = '\n';
	printf("%s\n", strBuffer);
}