//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Log.h
// @brief: ISP log head file.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "BZUtils.h"

#define LOG_ON 1
#define LOG_LEVEL (0x1 + 0x2 + 0x4)

#define LOG_BUFFER_SIZE				256
#define LOG_BUFFER_PERSERVE_SIZE	2		//2 preserve for \0 and \n
#define LOG_BUFFER_TIME_SIZE		24		//24 char "xxxx-xx-xx xx:xx:xx:xxx "
#define LOG_BUFFER_LEFT_SIZE		LOG_BUFFER_SIZE -  LOG_BUFFER_PERSERVE_SIZE - LOG_BUFFER_TIME_SIZE - sizeof(long long int)


#define SYSTEM_YEAR_OFFSET			1900
#define SYSTEM_MONTH_OFFSET			1
#define LOCAL_TIME_ZOOM_OFFSET		8	//Beijing time zoom

int BZLogAddInfo(const char* str, ...);
void BZLogAddTime(const char* str, va_list va);
void BZLogPrint(const char* str, va_list va);

enum SYSTEM_TYPES {
	WINDOWS_SYSTEM = 1,
	LINUX_SYSTEM
};

enum LOG_MASK {
	LOG_ERROR_MASK = 0x1,
	LOG_WARN_MASK = 0x2,
	LOG_INFO_MASK = 0x4,
	LOG_DEBUG_MASK = 0x8
};

#define ISP_LOG_INFO_FORMAT "%d:%s: "

#define LogErroSwitch(on, str, ...)		((on) ? (BZLogAddInfo("BZ  E " ISP_LOG_INFO_FORMAT str, __LINE__, __func__, ##__VA_ARGS__)) : (0))
#define LogWarnSwitch(on, str, ...)		((on) ? (BZLogAddInfo("BZ  W " ISP_LOG_INFO_FORMAT str, __LINE__, __func__, ##__VA_ARGS__)) : (0))
#define LogInfoSwitch(on, str, ...)		((on) ? (BZLogAddInfo("BZ  I " ISP_LOG_INFO_FORMAT str, __LINE__, __func__, ##__VA_ARGS__)) : (0))
#define LogDebugSwitch(on, str, ...)	((on) ? (BZLogAddInfo("BZ  D " ISP_LOG_INFO_FORMAT str, __LINE__, __func__, ##__VA_ARGS__)) : (0))

#define BZLoge(str, ...)	LogErroSwitch((LOG_LEVEL & LOG_ERROR_MASK), str, ##__VA_ARGS__)
#define BZLogw(str, ...)	LogWarnSwitch((LOG_LEVEL & LOG_WARN_MASK), str, ##__VA_ARGS__)
#define BZLogi(str, ...)	LogInfoSwitch((LOG_LEVEL & LOG_INFO_MASK), str, ##__VA_ARGS__)
#define BZLogd(str, ...)	LogDebugSwitch((LOG_LEVEL & LOG_DEBUG_MASK), str, ##__VA_ARGS__)


char BZTimeInt2Char(int32_t i);
void GetTimeWithDateInt(int32_t* years, int32_t* months, int32_t* days, int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds);
void GetTimeWithDateChar(char* years, char* months, char* days, char* hours, char* minutes, char* seconds, char* milliseconds);
