// License: GPL-3.0-or-later
/*
 * BoZhi interface implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "../interface/LibInterface.h"
#include "../include/Algorithm.h"

#ifdef LINUX_SYSTEM
#define BZ_PRE_DECLARE
#elif defined WIN32_SYSTEM
#define BZ_PRE_DECLARE _declspec(dllexport)
#endif

static ISP_CALLBACKS gISPCbs;

extern "C" {
BZ_PRE_DECLARE void InterfaceInit(void* pOps, ...)
{
	LIB_OPS* pLibOps = static_cast<LIB_OPS*>(pOps);

	if (pLibOps) {
		pLibOps->LIB_BLC = &Lib_BlackLevelCorrection;
		pLibOps->LIB_LSC = &Lib_LensShadingCorrection;
		pLibOps->LIB_Demosaic = &Lib_Demosaic;
		pLibOps->LIB_WB = &Lib_WhiteBalance;
		pLibOps->LIB_CC = &Lib_ColorCorrection;
		pLibOps->LIB_Gamma = &Lib_GammaCorrection;
		pLibOps->LIB_WNR = &Lib_WaveletNR;
		pLibOps->LIB_EE = &Lib_EdgeEnhancement;
		pLibOps->LIB_CST_RAW2RGB = &Lib_CST_RAW2RGB;
		pLibOps->LIB_CST_RGB2YUV = &Lib_CST_RGB2YUV;
		pLibOps->LIB_CST_YUV2RGB = &Lib_CST_YUV2RGB;
	}
}

BZ_PRE_DECLARE void RegistCallbacks(void* pCbs, ...)
{
	ISP_CALLBACKS* pISPCbs = static_cast<ISP_CALLBACKS*>(pCbs);

	if (pISPCbs) {
		gISPCbs.UtilsFuncs.Log = pISPCbs->UtilsFuncs.Log;
	}
}
}
