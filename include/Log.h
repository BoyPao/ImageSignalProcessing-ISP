// License: GPL-3.0-or-later
/*
 * ISP log head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include "Utils.h"

#define LOG_ON 1
#define LOG_LEVEL (0x1 + 0x2 + 0x4)

#define LOG_BUFFER_SIZE				256
#define LOG_BUFFER_PERSERVE_SIZE	2		/* 2 preserve for \0 and \n */
#define LOG_BUFFER_TIME_SIZE		24		/* 24 char "xxxx-xx-xx xx:xx:xx:xxx " */
#define LOG_BUFFER_LEFT_SIZE		LOG_BUFFER_SIZE - LOG_BUFFER_PERSERVE_SIZE - LOG_BUFFER_TIME_SIZE - sizeof(long long int)

void LogAddInfo(const char* str, ...);
void LogPrint(const char* str, va_list va);

enum LOG_MASK {
	LOG_ERROR_MASK	= 0x1,
	LOG_WARN_MASK	= LOG_ERROR_MASK << 1,
	LOG_INFO_MASK	= LOG_ERROR_MASK << 2,
	LOG_DEBUG_MASK	= LOG_ERROR_MASK << 3,
};

#define LOG_MODULE "ISP "
#define LOG_FORMAT " %d:%s: "

#define LogError(on, str, ...)		((on) ?														\
		({LogAddInfo(LOG_MODULE "E" LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__); (0);})	\
		: (0))

#define LogWarn(on, str, ...)		((on) ?														\
		({LogAddInfo(LOG_MODULE "W" LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__); (0);})	\
		: (0))

#define LogInfo(on, str, ...)		((on) ?														\
		({LogAddInfo(LOG_MODULE "I" LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__); (0);})  \
		: (0))

#define LogDebug(on, str, ...)		((on) ?														\
		({LogAddInfo(LOG_MODULE "D" LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__); (0);})  \
		: (0))

#define ISPLoge(str, ...)	((LOG_ON) ? LogError((LOG_LEVEL & LOG_ERROR_MASK), str, ##__VA_ARGS__) : (0))
#define ISPLogw(str, ...)	((LOG_ON) ? LogWarn((LOG_LEVEL & LOG_WARN_MASK), str, ##__VA_ARGS__) : (0))
#define ISPLogi(str, ...)	((LOG_ON) ? LogInfo((LOG_LEVEL & LOG_INFO_MASK), str, ##__VA_ARGS__) : (0))
#define ISPLogd(str, ...)	((LOG_ON) ? LogDebug((LOG_LEVEL & LOG_DEBUG_MASK), str, ##__VA_ARGS__) : (0))
