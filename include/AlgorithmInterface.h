#pragma once
#include "ParamManager.h"

struct ISP_PROCESS_CALLBACKS {
	ISPResult(*ISP_CBs)(int32_t argNum, ...);
};

struct ISP_LIB_FUNCS
{
	ISPResult(*ISP_BLC)		(void* data, va_list input_va, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_LSC)		(void* data, va_list input_va, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_WB)		(void* data, va_list input_va, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_CC)		(void* data, va_list input_va, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_Gamma)	(void* data, va_list input_va, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_WNR)		(void* data, va_list input_va, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_EE)		(void* data, va_list input_va, ISP_PROCESS_CALLBACKS CBs, ...);
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
