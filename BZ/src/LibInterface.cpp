// License: GPL-3.0-or-later
/*
 * BoZhi interface implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "BZInterface.h"
#include "BoZhi.h"

#ifdef LINUX_SYSTEM
#define BZ_PRE_DECLARE
#elif defined WIN32_SYSTEM
#define BZ_PRE_DECLARE _declspec(dllexport)
#endif

extern "C" {
BZ_PRE_DECLARE void LibInit(void* pOPS, ...)
{
	WrapLibInit(pOPS);
}

BZ_PRE_DECLARE void LibDeInit(void* v, ...)
{
	(void) v;
	WrapLibDeInit();
}

BZ_PRE_DECLARE void RegistCallbacks(void* pCBs, ...)
{
	WrapRegistCallbacks(pCBs);
}
}
