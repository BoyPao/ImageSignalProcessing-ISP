// License: GPL-3.0-or-later
/*
 * BoZhi common fuctions head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
/* Regular lib */
#include <chrono>
#include <stdint.h>
#include <math.h>
#include <cstdarg>
#include <string.h>
#include <unistd.h>

#if defined __linux__
#define LINUX_SYSTEM
#elif defined _WIN32
#define WIN32_SYSTEM
#endif

#define BITS_PER_WORD				8
#define FILE_PATH_MAX_SIZE			255

#define SUCCESS(rt)		((rt) >= 0) ? true : false

using namespace std;

enum BZResult {
	BZ_INVALID_PARAM = -4,
	BZ_MEMORY_ERROR = -3,
	BZ_STATE_ERROR = -2,
	BZ_FAILED = -1,
	BZ_SUCCESS = 0,
	BZ_SKIP = 1
};
