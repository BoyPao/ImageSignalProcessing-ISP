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
#define DBG_LEVEL (0x1)

#define LOG_BUFFER_SIZE				256
#define LOG_BUFFER_PERSERVE_SIZE	2		/* 2 preserve for \0 and \n */
#define LOG_BUFFER_TIME_SIZE		24		/* 24 char "xxxx-xx-xx xx:xx:xx:xxx " */
#define LOG_BUFFER_LEFT_SIZE		LOG_BUFFER_SIZE - LOG_BUFFER_PERSERVE_SIZE - LOG_BUFFER_TIME_SIZE - sizeof(long long int)

void LogBase(const char* str, ...);
void LogAddInfo(bool needExtInfo, const char* str, va_list va);
void LogPrint(const char* str, va_list va);

enum ISP_LOG_MASK {
	LOG_BASE_MASK	= 0x1,
	LOG_ERROR_MASK	= LOG_BASE_MASK,
	LOG_WARN_MASK	= LOG_BASE_MASK << 1,
	LOG_INFO_MASK	= LOG_BASE_MASK << 2,
	LOG_DEBUG_MASK	= LOG_BASE_MASK << 3,
};

enum ISP_DBG_MASK {
	DBG_BASE_MASK = 0x1,
	DBG_CORE_MASK = DBG_BASE_MASK,
	DBG_FILE_MASK = DBG_BASE_MASK << 1,
	DBG_LIST_MASK = DBG_BASE_MASK << 2,
	DBG_INTF_MASK = DBG_BASE_MASK << 3,
};

#define LOG_MODULE "ISP "
#define LOG_FORMAT " %d:%s: "

#define ISPLogError(on, str, ...)		((on) ?														\
		({LogBase(LOG_MODULE "E" LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__); (0);})	\
		: (0))

#define ISPLogWarn(on, str, ...)		((on) ?														\
		({LogBase(LOG_MODULE "W" LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__); (0);})	\
		: (0))

#define ISPLogInfo(on, str, ...)		((on) ?														\
		({LogBase(LOG_MODULE "I" LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__); (0);})  \
		: (0))

#define ISPLogDebug(on, str, ...)		((on) ?														\
		({LogBase(LOG_MODULE "D" LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__); (0);})  \
		: (0))

#define ILOGE(str, ...)	((LOG_ON) ? ISPLogError((LOG_LEVEL & LOG_ERROR_MASK), str, ##__VA_ARGS__) : (0))
#define ILOGW(str, ...)	((LOG_ON) ? ISPLogWarn((LOG_LEVEL & LOG_WARN_MASK), str, ##__VA_ARGS__) : (0))
#define ILOGI(str, ...)	((LOG_ON) ? ISPLogInfo((LOG_LEVEL & LOG_INFO_MASK), str, ##__VA_ARGS__) : (0))
#define ILOGD(str, ...)	((LOG_ON) ? ISPLogDebug((LOG_LEVEL & LOG_DEBUG_MASK), str, ##__VA_ARGS__) : (0))

#define ILOGDC(str, ...)	((DBG_LEVEL & DBG_CORE_MASK) ? ({ILOGD(str, ##__VA_ARGS__); (0);}) : (0))
#define ILOGDF(str, ...)	((DBG_LEVEL & DBG_FILE_MASK) ? ({ILOGD(str, ##__VA_ARGS__); (0);}) : (0))
#define ILOGDL(str, ...)	((DBG_LEVEL & DBG_LIST_MASK) ? ({ILOGD(str, ##__VA_ARGS__); (0);}) : (0))
#define ILOGDI(str, ...)	((DBG_LEVEL & DBG_INTF_MASK) ? ({ILOGD(str, ##__VA_ARGS__); (0);}) : (0))
