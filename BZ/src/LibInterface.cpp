//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: AlgorithmInterface.cpp
// @brief: ISP algorithm interface source file.
//////////////////////////////////////////////////////////////////////////////////////

#include "../include/LibInterface.h"
#include "../include/BZLog.h"
#include "../include/Algorithm.h"

static LIB_FUNCS gLib_funcs = {nullptr};
static ISP_CALLBACKS gISP_cbs = {nullptr};

void RegisterISPLibFuncs() 
{
	gLib_funcs.LIB_BLC			= &LIB_BlackLevelCorrection;
	gLib_funcs.LIB_LSC			= &LIB_LensShadingCorrection;
	gLib_funcs.LIB_WB			= &LIB_WhiteBalance;
	gLib_funcs.LIB_CC			= &LIB_ColorCorrection;
	gLib_funcs.LIB_Gamma		= &LIB_GammaCorrection;
	gLib_funcs.LIB_WNR			= &LIB_WaveletNR;
	gLib_funcs.LIB_EE			= &LIB_EdgeEnhancement;
	gLib_funcs.LIB_Demosaic		= &LIB_Demosaic;
	gLib_funcs.LIB_CST_RGB2YUV	= &LIB_CST_RGB2YUV;
}

//Bayer Process
void Lib_BlackLevelCorrection(void* data, LIB_PARAMS* pParams, ...)
{

	if (gLib_funcs.LIB_BLC) {
		gLib_funcs.LIB_BLC(data, pParams, gISP_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_LensShadingCorrection(void* data, LIB_PARAMS* pParams, ...)
{
	if (gLib_funcs.LIB_LSC) {
		gLib_funcs.LIB_LSC(data, pParams, gISP_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

//RGB Process
void Lib_WhiteBalance(void* data, LIB_PARAMS* pParams, ...)
{
	if (gLib_funcs.LIB_WB) {
		gLib_funcs.LIB_WB(data, pParams, gISP_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_ColorCorrection(void* data, LIB_PARAMS* pParams, ...)
{
	if (gLib_funcs.LIB_CC) {
		gLib_funcs.LIB_CC(data, pParams, gISP_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_GammaCorrection(void* data, LIB_PARAMS* pParams, ...)
{
	if (gLib_funcs.LIB_Gamma) {
		gLib_funcs.LIB_Gamma(data, pParams, gISP_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

//YUVProcess
void Lib_WaveletNR(void* data, LIB_PARAMS* pParams, ...)
{
	if (gLib_funcs.LIB_WNR) {
		gLib_funcs.LIB_WNR(data, pParams, gISP_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_EdgeEnhancement(void* data, LIB_PARAMS* pParams, ...)
{
	if (gLib_funcs.LIB_EE) {
		gLib_funcs.LIB_EE(data, pParams, gISP_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_Demosaic(void* src, void* dst, LIB_PARAMS* pParams, ...)
{
	bool enable = true;
	va_list va;
	va_start(va, pParams);
	enable = static_cast<bool>(va_arg(va, int32_t));
	va_end(va);
	if (gLib_funcs.LIB_Demosaic) {
		gLib_funcs.LIB_Demosaic(src, dst, pParams, gISP_cbs, enable);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_CST_RGB2YUV(void* src, void* dst, LIB_PARAMS* pParams, ...)
{
	if (gLib_funcs.LIB_CST_RGB2YUV) {
		gLib_funcs.LIB_CST_RGB2YUV(src, dst, pParams, gISP_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}
