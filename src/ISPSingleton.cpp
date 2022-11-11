// License: GPL-3.0-or-later
/*
 * ISP singleton function implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPSingleton.h"

ISPSingleton *ISPSingleton::gISP = NULL;
std::mutex ISPSingleton::gLock;
int32_t ISPSingleton::rCnt = 0x1;

ISPSingleton* ISPSingleton::GetInstance()
{
	if (gISP == NULL)
	{
		std::unique_lock<std::mutex> lock(gLock);
		if (gISP == NULL) {
			gISP = new ISPSingleton();
		}
	}

	return gISP;
}

int32_t ISPSingleton::ReleaseInstance()
{
	int32_t rt = ISP_SUCCESS;

	rCnt--;
	if (gISP)
	{
		std::unique_lock<std::mutex> lock(gLock);
		if (gISP) {
			delete gISP;
			gISP = NULL;
		}
	} else {
		rt = ISP_FAILED;
		ILOGE("%d%s invalid release operation!", -1 * rCnt, SERIAL_NUM_TAIL(-1 * rCnt));
	}

	return rt;
}
