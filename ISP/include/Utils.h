// License: GPL-3.0-or-later
/*
 * Common fuction head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#if defined __linux__
#define LINUX_SYSTEM
#elif defined _WIN32
#define WIN32_SYSTEM
#endif

/* Regular lib */
#include <time.h>
#include <chrono>
#include <stdint.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "Log.h"

#ifdef LINUX_SYSTEM
#include <unistd.h>
#elif defined WIN32_SYSTEM
#endif

#define VERSION 0
#define SUB_VERSION 1

typedef unsigned char uchar;

#define SERIAL_NUM_TAIL(n)  ((n) == 1 ? "st" : \
		((n) == 2 ? "nd" : \
		 ((n) == 3 ? "rd" : "th")))

#define SYSTEM_YEAR_OFFSET			1900
#define SYSTEM_MONTH_OFFSET			1
#define LOCAL_TIME_ZOOM_OFFSET		8	/* Beijing time zoom */

#define BITS_PER_WORD				8
#define FILE_PATH_MAX_SIZE			255

#define SUCCESS(rt)		((rt) >= 0) ? true : false

enum ISPResult {
	ISP_TIMEOUT			= -5,
	ISP_INVALID_PARAM	= -4,
	ISP_MEMORY_ERROR	= -3,
	ISP_STATE_ERROR		= -2,
	ISP_FAILED			= -1,
	ISP_SUCCESS			= 0,
	ISP_SKIP			= 1
};

using namespace std;

char Int2Char(int32_t i);
int32_t Char2Int(char c);
void getTimeChar(char* hours, char* minutes, char* seconds, char* milliseconds);
void getTimeInt(int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds);
void getDateInt(int32_t* years, int32_t* months, int32_t* days);
void getTimeWithDateInt(int32_t* years, int32_t* months, int32_t* days, int32_t* hours, int32_t* minutes, int32_t* seconds, int32_t* milliseconds);
void getTimeWithDateChar(char* years, char* months, char* days, char* hours, char* minutes, char* seconds, char* milliseconds);
int32_t CharNum2IntNum(char* pC);
