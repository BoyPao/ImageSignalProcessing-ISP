#include "AlgorithmInterface.h"
#include "Algorithm.h"

static ISP_LIB_FUNCS gISP_Lib_funcs = {nullptr};
static ISP_PROCESS_CALLBACKS gISP_Process_cbs = {nullptr};

ISPResult RegisterISPLibFuncs() 
{
	ISPResult result = ISP_SUCCESS;

	gISP_Lib_funcs.ISP_BLC = &ISP_BlackLevelCorrection;
	gISP_Lib_funcs.ISP_LSC = &ISP_LensShadingCorrection;
	gISP_Lib_funcs.ISP_WB = &ISP_WhiteBalance;
	gISP_Lib_funcs.ISP_CC = &ISP_ColorCorrection;
	gISP_Lib_funcs.ISP_Gamma = &ISP_GammaCorrection;
	gISP_Lib_funcs.ISP_WNR = &ISP_WaveletNR;
	gISP_Lib_funcs.ISP_EE = &ISP_EdgeEnhancement;
	gISP_Lib_funcs.ISP_Demosaic = &ISP_Demosaic;
	gISP_Lib_funcs.ISP_CST_RGB2YUV = &ISP_CST_RGB2YUV;

	return result;
}

//Bayer Process
ISPResult BlackLevelCorrection(ISPParamManager* pPM, void* data, ...)
{
	ISPResult result = ISP_SUCCESS;

	int32_t width, height;
	uint16_t offset;
	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		
		result = pPM->GetIMGDimension(&width, &height);
		if (SUCCESS(result)) {
			result = pPM->GetBLCParam(&offset);
			if (!SUCCESS(result)) {
				ISPLoge("get BLC offset failed. result:%d", result);
			}
		}
		else {
			ISPLoge("get IMG Dimension failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gISP_Lib_funcs.ISP_BLC) {
			va_list(va);
			__crt_va_start(va, data);
			result = gISP_Lib_funcs.ISP_BLC(data, gISP_Process_cbs, width, height, offset);
			__crt_va_end(va);
		}
		else {
			result = ISP_STATE_ERROR;
			ISPLoge("ISP lib function has not been registed! result:%d", result);
		}
	}

	return result;
}

ISPResult LensShadingCorrection(ISPParamManager* pPM, void* data, ...)
{
	ISPResult result = ISP_SUCCESS;
	
	int32_t width, height;
	int32_t i, j;

	float R_lsc[LSC_LUT_HEIGHT * LSC_LUT_WIDTH], Gr_lsc[LSC_LUT_HEIGHT * LSC_LUT_WIDTH], Gb_lsc[LSC_LUT_HEIGHT * LSC_LUT_WIDTH], B_lsc[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];

	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetIMGDimension(&width, &height);
		if (SUCCESS(result)) {
			result = pPM->GetLSCParam(R_lsc, Gr_lsc, Gb_lsc, B_lsc);
			if (!SUCCESS(result)) {
				ISPLoge("get LSC lut failed. result:%d", result);
			}
		}
		else {
			ISPLoge("get IMG Dimension failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gISP_Lib_funcs.ISP_LSC) {
			va_list(va);
			__crt_va_start(va, data);
			result = gISP_Lib_funcs.ISP_LSC(data, gISP_Process_cbs, width, height, R_lsc, Gr_lsc, Gb_lsc, B_lsc);
			__crt_va_end(va);
		}
		else {
			result = ISP_STATE_ERROR;
			ISPLoge("ISP lib function has not been registed! result:%d", result);
		}
	}

	return result;
}

//RGB Process
ISPResult WhiteBalance(ISPParamManager* pPM, void* data, ...)
{
	ISPResult result = ISP_SUCCESS;

	int32_t width, height;
	double bGain, gGain, rGain;
	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetIMGDimension(&width, &height);
		if (SUCCESS(result)) {
			result = pPM->GetWBParam(&rGain, &gGain, &bGain);
			if (!SUCCESS(result)) {
				ISPLoge("get WB gain failed. result:%d", result);
			}
		}
		else {
			ISPLoge("get IMG Dimension failed. result:%d", result);
		}

	}

	if (SUCCESS(result)) {
		if (gISP_Lib_funcs.ISP_WB) {
			va_list(va);
			__crt_va_start(va, data);
			result = gISP_Lib_funcs.ISP_WB(data, gISP_Process_cbs, width, height, rGain, gGain, bGain);
			__crt_va_end(va);
		}
		else {
			result = ISP_STATE_ERROR;
			ISPLoge("ISP lib function has not been registed! result:%d", result);
		}
	}

	return result;
}

ISPResult ColorCorrection(ISPParamManager* pPM, void* data, ...)
{
	ISPResult result = ISP_SUCCESS;

	int32_t width, height;
	float ccm[CCM_HEIGHT * CCM_WIDTH] = { 0.0 };
	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetIMGDimension(&width, &height);
		if (SUCCESS(result)) {
			result = pPM->GetCCParam(ccm);
			if (!SUCCESS(result)) {
				ISPLoge("get Color Correction Matrix failed. result:%d", result);
			}
		}
		else {
			ISPLoge("get IMG Dimension failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gISP_Lib_funcs.ISP_CC) {
			va_list(va);
			__crt_va_start(va, data);
			result = gISP_Lib_funcs.ISP_CC(data, gISP_Process_cbs, width, height, ccm);
			__crt_va_end(va);
		}
		else {
			result = ISP_STATE_ERROR;
			ISPLoge("ISP lib function has not been registed! result:%d", result);
		}
	}

	return result;
}

ISPResult GammaCorrection(ISPParamManager* pPM, void* data, ...)
{
	ISPResult result = ISP_SUCCESS;

	int32_t width, height;
	uint16_t lut[GAMMA_LUT_SIZE];
	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetIMGDimension(&width, &height);
		if (SUCCESS(result)) {
			result = pPM->GetGAMMAPARAM(lut);
			if (!SUCCESS(result)) {
				ISPLoge("get Gamma lut failed. result:%d", result);
			}
		}
		else {
			ISPLoge("get IMG Dimension failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gISP_Lib_funcs.ISP_Gamma) {
			va_list(va);
			__crt_va_start(va, data);
			result = gISP_Lib_funcs.ISP_Gamma(data, gISP_Process_cbs, width, height, lut);
			__crt_va_end(va);
		}
		else {
			result = ISP_STATE_ERROR;
			ISPLoge("ISP lib function has not been registed! result:%d", result);
		}
	}

	return result;
}

//YUVProcess
ISPResult WaveletNR(ISPParamManager* pPM, void* data, ...)
{
	ISPResult result = ISP_SUCCESS;

	int32_t width, height;
	int32_t strength1;
	int32_t strength2;
	int32_t strength3;
	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetIMGDimension(&width, &height);
		if (SUCCESS(result)) {
			result = pPM->GetWNRPARAM(&strength1, &strength2, &strength3);
			if (!SUCCESS(result)) {
				ISPLoge("get SWNR param failed. result:%d", result);
			}
		}
		else {
			ISPLoge("get IMG Dimension failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gISP_Lib_funcs.ISP_WNR) {
			va_list(va);
			__crt_va_start(va, data);
			result = gISP_Lib_funcs.ISP_WNR(data, gISP_Process_cbs, width, height, strength1, strength2, strength3);
			__crt_va_end(va);
		}
		else {
			result = ISP_STATE_ERROR;
			ISPLoge("ISP lib function has not been registed! result:%d", result);
		}
	}

	return result;
}

ISPResult Sharpness(ISPParamManager* pPM, void* data, ...)
{
	ISPResult result = ISP_SUCCESS;

	int32_t width, height;
	double alpha;
	int32_t coreSzie;
	int32_t delta;
	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetIMGDimension(&width, &height);
		if (SUCCESS(result)) {
			result = pPM->GetEERPARAM(&alpha, &coreSzie, &delta);
			if (!SUCCESS(result)) {
				ISPLoge("get SWNR param failed. result:%d", result);
			}
		}
		else {
			ISPLoge("get IMG Dimension failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gISP_Lib_funcs.ISP_EE) {
			va_list(va);
			__crt_va_start(va, data);
			result = gISP_Lib_funcs.ISP_EE(data, gISP_Process_cbs, width, height, alpha, coreSzie, delta);
			__crt_va_end(va);
		}
		else {
			result = ISP_STATE_ERROR;
			ISPLoge("ISP lib function has not been registed! result:%d", result);
		}
	}

	return result;
}

ISPResult Demosaic(ISPParamManager* pPM, void* src, void* dst)
{
	ISPResult result = ISP_SUCCESS;

	int32_t width, height;
	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetIMGDimension(&width, &height);
		if (!SUCCESS(result)) {
			ISPLoge("get IMG Dimension failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gISP_Lib_funcs.ISP_Demosaic) {
			result = gISP_Lib_funcs.ISP_Demosaic(src, dst, gISP_Process_cbs, width, height);
		}
		else {
			result = ISP_STATE_ERROR;
			ISPLoge("ISP lib function has not been registed! result:%d", result);
		}
	}

	return result;
}

ISPResult CST_RGB2YUV(ISPParamManager* pPM, void* src, void* dst)
{
	ISPResult result = ISP_SUCCESS;

	int32_t width, height;
	if (!pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("ParamManager is null! result%d", result);
	}

	if (SUCCESS(result)) {
		result = pPM->GetIMGDimension(&width, &height);
		if (!SUCCESS(result)) {
			ISPLoge("get IMG Dimension failed. result:%d", result);
		}
	}

	if (SUCCESS(result)) {
		if (gISP_Lib_funcs.ISP_CST_RGB2YUV) {
			result = gISP_Lib_funcs.ISP_CST_RGB2YUV(src, dst, gISP_Process_cbs, width, height);
		}
		else {
			result = ISP_STATE_ERROR;
			ISPLoge("ISP lib function has not been registed! result:%d", result);
		}
	}

	return result;
}