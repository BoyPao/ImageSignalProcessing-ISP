//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: AlgorithmInterface.cpp
// @brief: ISP algorithm interface source file.
//////////////////////////////////////////////////////////////////////////////////////

#include "../include/LibInterface.h"
#include "../include/Algorithm.h"

static LIB_FUNCS gLib_funcs = {nullptr};
static ISP_CALLBACKS gISP_cbs = {nullptr};

void RegisterISPLibFuncs(LIB_FUNCS* pLibFuncs)
{
	if (pLibFuncs) {
		pLibFuncs->LIB_BLC = &Lib_BlackLevelCorrection;
		pLibFuncs->LIB_LSC = &Lib_LensShadingCorrection;
		pLibFuncs->LIB_WB = &Lib_WhiteBalance;
		pLibFuncs->LIB_CC = &Lib_ColorCorrection;
		pLibFuncs->LIB_Gamma = &Lib_GammaCorrection;
		pLibFuncs->LIB_WNR = &Lib_WaveletNR;
		pLibFuncs->LIB_EE = &Lib_EdgeEnhancement;
		pLibFuncs->LIB_TAIL = &Lib_TailProcess;
		pLibFuncs->LIB_Demosaic = &Lib_Demosaic;
		pLibFuncs->LIB_CST_RGB2YUV = &Lib_CST_RGB2YUV;
	}
}
