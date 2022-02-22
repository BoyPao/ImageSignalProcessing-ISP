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

#define ISPLogBase(type, on, str, ...)														\
	do {																					\
		if (LOG_ON) {																		\
			if (on) {																		\
				LogBase(LOG_MODULE type LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__);	\
			}																				\
		}																					\
	} while(0)

#define ILOGE(str, ...) ISPLogBase("E", (LOG_LEVEL & LOG_ERROR_MASK), str, ##__VA_ARGS__)
#define ILOGW(str, ...) ISPLogBase("W", (LOG_LEVEL & LOG_WARN_MASK), str, ##__VA_ARGS__)
#define ILOGI(str, ...) ISPLogBase("I", (LOG_LEVEL & LOG_INFO_MASK), str, ##__VA_ARGS__)
#define ILOGD(str, ...)	ISPLogBase("D", (LOG_LEVEL & LOG_DEBUG_MASK), str, ##__VA_ARGS__)

#define ILOGDC(str, ...)					\
	do{										\
		if (DBG_LEVEL & DBG_CORE_MASK) {	\
			ILOGD(str, ##__VA_ARGS__);		\
		}									\
	} while(0)

#define ILOGDF(str, ...)					\
	do{										\
		if (DBG_LEVEL & DBG_FILE_MASK) {	\
			ILOGD(str, ##__VA_ARGS__);		\
		}									\
	} while(0)

#define ILOGDL(str, ...)					\
	do{										\
		if (DBG_LEVEL & DBG_LIST_MASK) {	\
			ILOGD(str, ##__VA_ARGS__);		\
		}									\
	} while(0)

#define ILOGDI(str, ...)					\
	do{										\
		if (DBG_LEVEL & DBG_INTF_MASK) {	\
			ILOGD(str, ##__VA_ARGS__);		\
		}									\
	} while(0)

