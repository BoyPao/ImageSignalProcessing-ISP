// License: GPL-3.0-or-later
/*
 * BoZhi log supports.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include "BoZhi.h"

#define LOG_ON 1
#define LOG_LEVEL (0x1 + 0x2 + 0x4)
#define DBG_LEVEL (0x1)

#define LOG_BUFFER_SIZE				256
#define LOG_BUFFER_PERSERVE_SIZE	2		/* 2 preserve for \0 and \n */
#define LOG_BUFFER_LEFT_SIZE		LOG_BUFFER_SIZE - LOG_BUFFER_PERSERVE_SIZE - sizeof(long long int)

void LogBase(const char* str, ...);
void LogAddInfo(const char* str, ...);
void LogPrint(const char* str, va_list va);

enum BZ_LOG_MASK {
	LOG_BASE_MASK	= 0x1,
	LOG_ERROR_MASK	= LOG_BASE_MASK,
	LOG_WARN_MASK	= LOG_ERROR_MASK << 1,
	LOG_INFO_MASK	= LOG_ERROR_MASK << 2,
	LOG_DEBUG_MASK	= LOG_ERROR_MASK << 3,
};

enum BZ_DBG_MASK {
	DBG_BASE_MASK = 0x1,
	DBG_CORE_MASK = DBG_BASE_MASK,
	DBG_INTF_MASK = DBG_BASE_MASK << 1,
	DBG_ALGO_MASK = DBG_BASE_MASK << 2,
};

#define LOG_MODULE " BZ "
#define LOG_FORMAT " %d:%s: "

#define BZLogBase(type, on, str, ...)																\
	do {																							\
		if (LOG_ON) {																				\
			if (on) {																				\
				if (WrapGetBoZhi()) {																\
					if(static_cast<BoZhi*>(WrapGetBoZhi())->GetCallbacks()->UtilsFuncs.Log) {		\
						static_cast<BoZhi*>(WrapGetBoZhi())->GetCallbacks()->UtilsFuncs.Log(        \
								LOG_MODULE type LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__); \
					} else {																		\
						LogBase(LOG_MODULE type LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__);	\
					}																				\
				} else {																			\
					LogBase(LOG_MODULE type LOG_FORMAT str, __LINE__, __func__, ##__VA_ARGS__);		\
				}																					\
			}																						\
		}																							\
	} while(0)

#define BLOGE(str, ...) BZLogBase("E", (LOG_LEVEL & LOG_ERROR_MASK), str, ##__VA_ARGS__)
#define BLOGW(str, ...) BZLogBase("W", (LOG_LEVEL & LOG_WARN_MASK), str, ##__VA_ARGS__)
#define BLOGI(str, ...) BZLogBase("I", (LOG_LEVEL & LOG_INFO_MASK), str, ##__VA_ARGS__)
#define BLOGD(str, ...)	BZLogBase("D", (LOG_LEVEL & LOG_DEBUG_MASK), str, ##__VA_ARGS__)

#define BLOGDC(str, ...)					\
	do{										\
		if (DBG_LEVEL & DBG_CORE_MASK) {	\
			BLOGD(str, ##__VA_ARGS__);		\
		}									\
	} while(0)

#define BLOGDI(str, ...)					\
	do{										\
		if (DBG_LEVEL & DBG_INTF_MASK) {	\
			BLOGD(str, ##__VA_ARGS__);		\
		}									\
	} while(0)

#define BLOGDA(str, ...)					\
	do{										\
		if (DBG_LEVEL & DBG_ALGO_MASK) {	\
			BLOGD(str, ##__VA_ARGS__);		\
		}									\
	} while(0)

