//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Log.h
// @brief: ISP log head file
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Utils.h"

#define LOG_ON 1
#define LOG_LEVEL 0x1 + 0x2 + 0x4 + 0x8

#define AddTime_BufferSize 256 -  sizeof(long long int) - 17 //2 preserve and 15 char [xx:xx:xx:xxx]:

int LogAddInfo(const char* str, ...);
void LogAddTime(const char* str, va_list va);
void LogPrint(const char* str, va_list va);

typedef enum SYSTEM_TYPES {
	WINDOWS_SYSTEM = 1,
	LINUX_SYSTEM
};

typedef enum LOG_MASK {
	LOG_ERROR_MASK = 0x1,
	LOG_WARN_MASK = 0x2,
	LOG_INFO_MASK = 0x4,
	LOG_DEBUG_MASK = 0x8
};

#define ISP_LOG_INFO_FORMAT "%d:%s: "

#define LogErroSwitch(on, str, ...)		((on) ? (LogAddInfo("[ISP][Erro] " ISP_LOG_INFO_FORMAT str, __LINE__, __func__, ##__VA_ARGS__)) : (0))
#define LogWarnSwitch(on, str, ...)		((on) ? (LogAddInfo("[ISP][Warn] " ISP_LOG_INFO_FORMAT str, __LINE__, __func__, ##__VA_ARGS__)) : (0))
#define LogInfoSwitch(on, str, ...)		((on) ? (LogAddInfo("[ISP][Info] " ISP_LOG_INFO_FORMAT str, __LINE__, __func__, ##__VA_ARGS__)) : (0))
#define LogDebugSwitch(on, str, ...)	((on) ? (LogAddInfo("[ISP][Debug] " ISP_LOG_INFO_FORMAT str, __LINE__, __func__, ##__VA_ARGS__)) : (0))

#define ISPLoge(str, ...)	LogErroSwitch((LOG_LEVEL & LOG_ERROR_MASK), str, ##__VA_ARGS__)
#define ISPLogw(str, ...)	LogWarnSwitch((LOG_LEVEL & LOG_WARN_MASK), str, ##__VA_ARGS__)
#define ISPLogi(str, ...)	LogInfoSwitch((LOG_LEVEL & LOG_INFO_MASK), str, ##__VA_ARGS__)
#define ISPLogd(str, ...)	LogDebugSwitch((LOG_LEVEL & LOG_DEBUG_MASK), str, ##__VA_ARGS__)
