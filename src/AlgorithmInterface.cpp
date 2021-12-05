//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: AlgorithmInterface.cpp
// @brief: ISP algorithm interface source file.
//////////////////////////////////////////////////////////////////////////////////////

#include "AlgorithmInterface.h"
#include "../BZ/interface/LibInterface.h"

static LIB_PARAMS gISPLibParams;
static LIB_FUNCS gLibFuncs;
static ISP_CALLBACKS gISPCallbacks;

ISPResult ISPLibInit(ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = InitISPCallbacks(&gISPCallbacks);
	}

	if (SUCCESS(result)) {
		RegisterISPLibFuncs(&gLibFuncs);
	}

	if (SUCCESS(result)) {
		memset(&gISPLibParams, 0, sizeof(gISPLibParams));
		result = pPM->GetImgInfo(&gISPLibParams);
		if (!SUCCESS(result)) {
			ISPLoge("Get image info failed. result:%d", result);
		}
	}

	return result;
}

ISPResult InitISPCallbacks(ISP_CALLBACKS* pCbs)
{
	ISPResult result = ISP_SUCCESS;

	if (!pCbs) {
		result = ISP_INVALID_PARAM;
		ISPLoge("cbs is null! result%d", result);
	}

	//TODO: add callbacks if need

	return result;
}


/* Bayer Process */
ISPResult BlackLevelCorrection(void* data, ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetBLCParam(&gISPLibParams);
		if (!SUCCESS(result)) {
			ISPLoge("Get BLC offset failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gLibFuncs.LIB_BLC) {
			gLibFuncs.LIB_BLC(data, &gISPLibParams, gISPCallbacks);
		}
		else {
			result = ISP_FAILED;
			ISPLoge("Lib function has not been registed!");
		}
	}

	return result;
}

ISPResult LensShadingCorrection(void* data, ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {

		result = pPM->GetLSCParam(&gISPLibParams);
		if (!SUCCESS(result)) {
			ISPLoge("Get LSC lut failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gLibFuncs.LIB_LSC) {
			gLibFuncs.LIB_LSC(data, &gISPLibParams, gISPCallbacks);
		}
		else {
			result = ISP_FAILED;
			ISPLoge("Lib function has not been registed!");
		}
	}

	return result;
}

/* RGB Process */
ISPResult Demosaic(void* data, ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		if (gLibFuncs.LIB_Demosaic) {
			gLibFuncs.LIB_Demosaic(data, &gISPLibParams, gISPCallbacks);
		}
		else {
			result = ISP_FAILED;
			ISPLoge("Lib function has not been registed!");
		}
	}

	return result;
}

ISPResult WhiteBalance(void* data, ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetWBParam(&gISPLibParams);
		if (!SUCCESS(result)) {
			ISPLoge("Get WB gain failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gLibFuncs.LIB_WB) {
			gLibFuncs.LIB_WB(data, &gISPLibParams, gISPCallbacks);
		}
		else {
			result = ISP_FAILED;
			ISPLoge("Lib function has not been registed!");
		}
	}

	return result;
}

ISPResult ColorCorrection(void* data, ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetCCParam(&gISPLibParams);
		if (!SUCCESS(result)) {
			ISPLoge("Get Color Correction Matrix failed. result:%d", result);
		}
	}


	if (SUCCESS(result)) {
		if (gLibFuncs.LIB_CC) {
			gLibFuncs.LIB_CC(data, &gISPLibParams, gISPCallbacks);
		}
		else {
			result = ISP_FAILED;
			ISPLoge("Lib function has not been registed!");
		}
	}

	return result;
}

ISPResult GammaCorrection(void* data, ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}


	if (SUCCESS(result)) {
		result = pPM->GetGAMMAParam(&gISPLibParams);
		if (!SUCCESS(result)) {
			ISPLoge("Get Gamma lut failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gLibFuncs.LIB_Gamma) {
			gLibFuncs.LIB_Gamma(data, &gISPLibParams, gISPCallbacks);
		}
		else {
			result = ISP_FAILED;
			ISPLoge("Lib function has not been registed!");
		}
	}

	return result;
}

/* YUVProcess */
ISPResult WaveletNR(void* data, ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetWNRParam(&gISPLibParams);
		if (!SUCCESS(result)) {
			ISPLoge("Get WNR param failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gLibFuncs.LIB_WNR) {
			gLibFuncs.LIB_WNR(data, &gISPLibParams, gISPCallbacks);
		}
		else {
			result = ISP_FAILED;
			ISPLoge("Lib function has not been registed!");
		}
	}

	return result;
}

ISPResult EdgeEnhancement(void* data, ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetEEParam(&gISPLibParams);
		if (!SUCCESS(result)) {
			ISPLoge("Get EE param failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gLibFuncs.LIB_EE) {
			gLibFuncs.LIB_EE(data, &gISPLibParams, gISPCallbacks);
		}
		else {
			result = ISP_FAILED;
			ISPLoge("Lib function has not been registed!");
		}
	}

	return result;
}

ISPResult CST_RAW2RGB(void* src, void* dst, ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		bool enable = true;
		va_list va;
		va_start(va, pPM);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		if (gLibFuncs.LIB_CST_RAW2RGB) {
			gLibFuncs.LIB_CST_RAW2RGB(src, dst, &gISPLibParams, gISPCallbacks, enable);
		}
		else {
			result = ISP_FAILED;
			ISPLoge("Lib function has not been registed!");
		}
	}

	return result;
}

ISPResult CST_RGB2YUV(void* src, void* dst, ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		bool enable = true;
		va_list va;
		va_start(va, pPM);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		if (gLibFuncs.LIB_CST_RGB2YUV) {
			gLibFuncs.LIB_CST_RGB2YUV(src, dst, &gISPLibParams, gISPCallbacks, enable);
		}
		else {
			result = ISP_FAILED;
			ISPLoge("Lib function has not been registed!");
		}
	}

	return result;
}

ISPResult CST_YUV2RGB(void* src, void* dst, ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		bool enable = true;
		va_list va;
		va_start(va, pPM);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		if (gLibFuncs.LIB_CST_YUV2RGB) {
			gLibFuncs.LIB_CST_YUV2RGB(src, dst, &gISPLibParams, gISPCallbacks, enable);
		}
		else {
			result = ISP_FAILED;
			ISPLoge("Lib function has not been registed!");
		}
	}

	return result;
}

