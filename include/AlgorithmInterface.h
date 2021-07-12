//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: AlgorithmInterface.h
// @brief: ISP algorithm interface head file.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Utils.h"
#include "ParamManager.h"

struct ISP_PROCESS_CALLBACKS {
	ISPResult(*ISP_CBs)(int32_t argNum, ...);
};

struct ISP_LIB_FUNCS
{
	ISPResult(*ISP_BLC)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_LSC)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_WB)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_CC)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_Gamma)		(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_WNR)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_EE)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_Demosaic)	(void* src, void* dst, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_CST_RGB2YUV) (void* src, void* dst, ISP_PROCESS_CALLBACKS CBs, ...);
};

ISPResult RegisterISPLibFuncs();
//Bayer Process
ISPResult BlackLevelCorrection(ISPParamManager* pPM, void* data, ...);
ISPResult LensShadingCorrection(ISPParamManager* pPM, void* data, ...);

//RGB Process
ISPResult WhiteBalance(ISPParamManager* pPM, void* data, ...);
ISPResult ColorCorrection(ISPParamManager* pPM, void* data, ...);
ISPResult GammaCorrection(ISPParamManager* pPM, void* data, ...);

//YUVProcess
ISPResult WaveletNR(ISPParamManager* pPM, void* data, ...);
ISPResult Sharpness(ISPParamManager* pPM, void* data, ...);

//CST
ISPResult Demosaic(ISPParamManager* pPM, void* src, void* dst);
ISPResult CST_RGB2YUV(ISPParamManager* pPM, void* src, void* dst);