// License: GPL-3.0-or-later
/*
 * BoZhi log supports.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include "BoZhi.h"

#define LOG_ON 1
#define LOG_LEVEL (0x1 + 0x2 + 0x4 + 0x8)
#define DBG_LEVEL (0x1)

#define LOG_BUFFER_SIZE				256
#define LOG_BUFFER_PERSERVE_SIZE	2		/* 2 preserve for \0 and \n */
#define LOG_BUFFER_LEFT_SIZE		LOG_BUFFER_SIZE - LOG_BUFFER_PERSERVE_SIZE - sizeof(long long int)

int32_t LogBase(const char* str, ...);
void LogPrint(const char* str, va_list va);

enum BZLogMask {
	LOG_BASE_MASK	= 0x1,
	LOG_ERROR_MASK	= LOG_BASE_MASK,
	LOG_WARN_MASK	= LOG_ERROR_MASK << 1,
	LOG_INFO_MASK	= LOG_ERROR_MASK << 2,
	LOG_DEBUG_MASK	= LOG_ERROR_MASK << 3,
};

enum BZDbgMask {
	DBG_BASE_MASK = 0x1,
	DBG_CORE_MASK = DBG_BASE_MASK << 1,
	DBG_INTF_MASK = DBG_BASE_MASK << 2,
	DBG_ALGO_MASK = DBG_BASE_MASK << 3,
};

#ifdef LOG_FOR_DBG
#define LOG_FORMAT " %d:%s: "
#define LOG_FMT_PARAM , __LINE__, __func__
#else
#define LOG_FORMAT " | "
#define LOG_FMT_PARAM
#endif

#define LOG_MODULE " BZ "

#define BZLogError(on, str, ...)	((on) ? LogBase(LOG_MODULE "E" LOG_FORMAT str LOG_FMT_PARAM, ##__VA_ARGS__) : (0))
#define BZLogWarn(on, str, ...)		((on) ? LogBase(LOG_MODULE "W" LOG_FORMAT str LOG_FMT_PARAM, ##__VA_ARGS__) : (0))
#define BZLogInfo(on, str, ...)		((on) ? LogBase(LOG_MODULE "I" LOG_FORMAT str LOG_FMT_PARAM, ##__VA_ARGS__) : (0))
#define BZLogDebug(on, str, ...)	((on) ? LogBase(LOG_MODULE "D" LOG_FORMAT str LOG_FMT_PARAM, ##__VA_ARGS__) : (0))

#define BLOGE(str, ...)	((LOG_ON) ? BZLogError((LOG_LEVEL & LOG_ERROR_MASK), str, ##__VA_ARGS__) : (0))
#define BLOGW(str, ...)	((LOG_ON) ? BZLogWarn((LOG_LEVEL & LOG_WARN_MASK), str, ##__VA_ARGS__) : (0))
#define BLOGI(str, ...)	((LOG_ON) ? BZLogInfo((LOG_LEVEL & LOG_INFO_MASK), str, ##__VA_ARGS__) : (0))
#define BLOGD(str, ...)	((LOG_ON) ? BZLogDebug((LOG_LEVEL & LOG_DEBUG_MASK), str, ##__VA_ARGS__) : (0))

#define BLOGDC(str, ...) ((DBG_LEVEL & DBG_CORE_MASK) ? BLOGD(str, ##__VA_ARGS__) : (0))
#define BLOGDI(str, ...) ((DBG_LEVEL & DBG_INTF_MASK) ? BLOGD(str, ##__VA_ARGS__) : (0))
#define BLOGDA(str, ...) ((DBG_LEVEL & DBG_ALGO_MASK) ? BLOGD(str, ##__VA_ARGS__) : (0))
