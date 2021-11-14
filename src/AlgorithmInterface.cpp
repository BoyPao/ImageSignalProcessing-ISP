//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: AlgorithmInterface.cpp
// @brief: ISP algorithm interface source file.
//////////////////////////////////////////////////////////////////////////////////////

#include "AlgorithmInterface.h"
#include "LibInterface.h"

static LIB_PARAMS gISPLibParams;

ISPResult ISPLibParamsInit(ISPParamManager* pPM, ...)
{
	ISPResult result = ISP_SUCCESS;

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
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

//Bayer Process
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
		Lib_BlackLevelCorrection(data, &gISPLibParams);
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
		Lib_LensShadingCorrection(data, &gISPLibParams);
	}

	return result;
}

//RGB Process
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
		Lib_WhiteBalance(data, &gISPLibParams);
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
		Lib_ColorCorrection(data, &gISPLibParams);
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
		Lib_GammaCorrection(data, &gISPLibParams);
	}

	return result;
}

//YUVProcess
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
		Lib_WaveletNR(data, &gISPLibParams);
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
		Lib_EdgeEnhancement(data, &gISPLibParams);
	}

	return result;
}

ISPResult Demosaic(void* src, void* dst, ISPParamManager* pPM, ...)
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
		Lib_Demosaic(src, dst, &gISPLibParams);
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
		Lib_CST_RGB2YUV(src, dst, &gISPLibParams);
	}

	return result;
}
