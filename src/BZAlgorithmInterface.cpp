//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: AlgorithmInterface.cpp
// @brief: ISP algorithm interface source file.
//////////////////////////////////////////////////////////////////////////////////////

#include "BZAlgorithmInterface.h"
#include "BZLog.h"
#include "Algorithm.h"

static ISP_LIB_FUNCS gISP_Lib_funcs = {nullptr};
static ISP_PROCESS_CALLBACKS gISP_Process_cbs = {nullptr};

void RegisterISPLibFuncs() 
{
	gISP_Lib_funcs.ISP_BLC			= &ISP_BlackLevelCorrection;
	gISP_Lib_funcs.ISP_LSC			= &ISP_LensShadingCorrection;
	gISP_Lib_funcs.ISP_WB			= &ISP_WhiteBalance;
	gISP_Lib_funcs.ISP_CC			= &ISP_ColorCorrection;
	gISP_Lib_funcs.ISP_Gamma		= &ISP_GammaCorrection;
	gISP_Lib_funcs.ISP_WNR			= &ISP_WaveletNR;
	gISP_Lib_funcs.ISP_EE			= &ISP_EdgeEnhancement;
	gISP_Lib_funcs.ISP_Demosaic		= &ISP_Demosaic;
	gISP_Lib_funcs.ISP_CST_RGB2YUV	= &ISP_CST_RGB2YUV;
}

//Bayer Process
void Lib_BlackLevelCorrection(void* data, ISP_LIB_PARAMS* pParams, ...)
{

	if (gISP_Lib_funcs.ISP_BLC) {
		gISP_Lib_funcs.ISP_BLC(data, pParams, gISP_Process_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_LensShadingCorrection(void* data, ISP_LIB_PARAMS* pParams, ...)
{
	if (gISP_Lib_funcs.ISP_LSC) {
		gISP_Lib_funcs.ISP_LSC(data, pParams, gISP_Process_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

//RGB Process
void Lib_WhiteBalance(void* data, ISP_LIB_PARAMS* pParams, ...)
{
	if (gISP_Lib_funcs.ISP_WB) {
		gISP_Lib_funcs.ISP_WB(data, pParams, gISP_Process_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_ColorCorrection(void* data, ISP_LIB_PARAMS* pParams, ...)
{
	if (gISP_Lib_funcs.ISP_CC) {
		gISP_Lib_funcs.ISP_CC(data, pParams, gISP_Process_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_GammaCorrection(void* data, ISP_LIB_PARAMS* pParams, ...)
{
	if (gISP_Lib_funcs.ISP_Gamma) {
		gISP_Lib_funcs.ISP_Gamma(data, pParams, gISP_Process_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

//YUVProcess
void Lib_WaveletNR(void* data, ISP_LIB_PARAMS* pParams, ...)
{
	if (gISP_Lib_funcs.ISP_WNR) {
		gISP_Lib_funcs.ISP_WNR(data, pParams, gISP_Process_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_EdgeEnhancement(void* data, ISP_LIB_PARAMS* pParams, ...)
{
	if (gISP_Lib_funcs.ISP_EE) {
		gISP_Lib_funcs.ISP_EE(data, pParams, gISP_Process_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_Demosaic(void* src, void* dst, ISP_LIB_PARAMS* pParams, ...)
{
	bool enable = true;
	va_list va;
	__crt_va_start(va, pParams);
	enable = static_cast<bool>(__crt_va_arg(va, bool));
	__crt_va_end(va);
	if (gISP_Lib_funcs.ISP_Demosaic) {
		gISP_Lib_funcs.ISP_Demosaic(src, dst, pParams, gISP_Process_cbs, enable);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}

void Lib_CST_RGB2YUV(void* src, void* dst, ISP_LIB_PARAMS* pParams, ...)
{
	if (gISP_Lib_funcs.ISP_CST_RGB2YUV) {
		gISP_Lib_funcs.ISP_CST_RGB2YUV(src, dst, pParams, gISP_Process_cbs);
	}
	else {
		BZLoge("ISP lib function has not been registed!");
	}
}