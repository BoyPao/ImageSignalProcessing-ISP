//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ParamManager.cpp
// @brief: ParamManager source file. ParamManager aims to manage multi params.
//////////////////////////////////////////////////////////////////////////////////////

#include "ParamManager.h"

#include "Param_1920x1080_D65_1000Lux.h"

static ISP_Config_Params ISPConfigueParams[PARAM_INDEX_NUM] {
	{
		/* 	BLC_PARAM  */	&BLCPARAM_1920x1080_D65_1000Lux,
		/* 	LSC_PARAM  */	&LSCPARM_1920x1080_D65_1000Lux,
		/* 	GCC_PARAM  */	&GCCPARAM_1920x1080_D65_1000Lux,
		/* 	WB_PARM  */		&WBPARAM_1920x1080_D65_1000Lux,
		/* 	CC_PARAM  */	&CCPARAM_1920x1080_D65_1000Lux,
		/* 	GAMMA_PARAM  */	&GAMMAPARAM_1920x1080_D65_1000Lux,
		/* 	WNR_PARAM  */	&WNRPARAM_1920x1080_D65_1000Lux,
		/* 	EE_PARAM  */	&EEPARAM_1920x1080_D65_1000Lux,
	},
};

ISPParamManager::ISPParamManager()
{
	mImg_Info = { 0 };
	mISP_ConfigParams = { nullptr };
	mState = PM_EMPTY;

	SelectParams(PARAM_1920x1080_D65_1000Lux);
}

ISPParamManager::~ISPParamManager()
{
}

ISPResult ISPParamManager::SetIMGInfo(IMG_INFO* info)
{
	ISPResult result = ISP_SUCCESS;

	if (info) {
		memcpy(&mImg_Info, info, sizeof(IMG_INFO));
	}
	else {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null! %d", result);
	}

	return result;
}

ISPResult ISPParamManager::GetIMGDimension(int32_t* width, int32_t* height)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (width && height) {
			*width = mImg_Info.width;
			*height = mImg_Info.height;
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
}

ISPResult ISPParamManager::SelectParams(int32_t paramIndex)
{
	ISPResult result = ISP_SUCCESS;

	if (paramIndex >= PARAM_INDEX_NUM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invaled param index:%d. %d", paramIndex, result);
	}

	if (SUCCESS(result)) {
		memcpy(&mISP_ConfigParams,
			&ISPConfigueParams[PARAM_1920x1080_D65_1000Lux + paramIndex],
			sizeof(ISP_Config_Params));
	}

	if (SUCCESS(result)) {
		mState = PM_SELECTED;
	}

	return result;
}

ISPResult ISPParamManager::GetImgInfo(LIB_PARAMS* pParams)
{
	ISPResult result = ISP_SUCCESS;

	if (!pParams) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(result)) {
		pParams->info.width = mImg_Info.width;
		pParams->info.height = mImg_Info.height;
		pParams->info.rawType = (LIB_RAW_TYPE)mImg_Info.rawType;
		pParams->info.bitspp = mImg_Info.bitspp;
		pParams->info.stride = mImg_Info.stride;
		pParams->info.packaged = mImg_Info.packaged;
	}

	return result;
}

ISPResult ISPParamManager::GetBLCParam(LIB_PARAMS* pParams)
{
	ISPResult result = ISP_SUCCESS;

	if (!pParams) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(result)) {
		int difBitNum = mISP_ConfigParams.pBLC_param->bitNum - 8;
		if (0 < difBitNum && difBitNum < 4) {
			pParams->BLC_param.offset = (mISP_ConfigParams.pBLC_param->BLCDefaultValue << difBitNum);
		}
		else {
			pParams->BLC_param.offset = mISP_ConfigParams.pBLC_param->BLCDefaultValue;
		}
	}

	return result;
}

ISPResult ISPParamManager::GetLSCParam(LIB_PARAMS* pParams)
{
	ISPResult result = ISP_SUCCESS;

	if (!pParams) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(result)) {
		for (int32_t i = 0; i < LSC_LUT_HEIGHT; i++) {
			memcpy(pParams->LSC_param.GainCh1 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh1[i], LSC_LUT_WIDTH * sizeof(float));
			memcpy(pParams->LSC_param.GainCh2 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh2[i], LSC_LUT_WIDTH * sizeof(float));
			memcpy(pParams->LSC_param.GainCh3 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh3[i], LSC_LUT_WIDTH * sizeof(float));
			memcpy(pParams->LSC_param.GainCh4 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh4[i], LSC_LUT_WIDTH * sizeof(float));
		}
	}
	return result;
}

ISPResult ISPParamManager::GetWBParam(LIB_PARAMS* pParams)
{
	ISPResult result = ISP_SUCCESS;

	if (!pParams) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(result)) {
		pParams->WB_param.rGain = mISP_ConfigParams.pWB_param->WB1stGAMMA2rd ? mISP_ConfigParams.pWB_param->gainType1.rGain : mISP_ConfigParams.pWB_param->gainType2.rGain;
		pParams->WB_param.gGain = mISP_ConfigParams.pWB_param->WB1stGAMMA2rd ? mISP_ConfigParams.pWB_param->gainType1.gGain : mISP_ConfigParams.pWB_param->gainType2.gGain;
		pParams->WB_param.bGain = mISP_ConfigParams.pWB_param->WB1stGAMMA2rd ? mISP_ConfigParams.pWB_param->gainType1.bGain : mISP_ConfigParams.pWB_param->gainType2.bGain;
	}

	return result;
}

ISPResult ISPParamManager::GetCCParam(LIB_PARAMS* pParams)
{
	ISPResult result = ISP_SUCCESS;

	if (!pParams) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(result)) {

		for (int32_t row = 0; row < CCM_HEIGHT; row++) {
			memcpy(pParams->CC_param.CCM + row * CCM_WIDTH, mISP_ConfigParams.pCC_param->CCM[row], CCM_WIDTH * sizeof(float));
		}
	}

	return result;
}

ISPResult ISPParamManager::GetGAMMAParam(LIB_PARAMS* pParams)
{
	ISPResult result = ISP_SUCCESS;

	if (!pParams) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(result)) {
		memcpy(pParams->Gamma_param.lut, &mISP_ConfigParams.pGamma_param->lut, 1024 * sizeof(uint16_t));
	}

	return result;
}

ISPResult ISPParamManager::GetWNRParam(LIB_PARAMS* pParams)
{
	ISPResult result = ISP_SUCCESS;

	if (!pParams) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(result)) {
		pParams->WNR_param.L1_threshold = mISP_ConfigParams.pWNR_param->L1_threshold;
		pParams->WNR_param.L2_threshold = mISP_ConfigParams.pWNR_param->L2_threshold;
		pParams->WNR_param.L3_threshold = mISP_ConfigParams.pWNR_param->L3_threshold;
	}

	return result;
}

ISPResult ISPParamManager::GetEEParam(LIB_PARAMS* pParams)
{
	ISPResult result = ISP_SUCCESS;

	if (!pParams) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(result)) {
		pParams->EE_param.alpha = mISP_ConfigParams.pEE_param->alpha;
		pParams->EE_param.coreSize = mISP_ConfigParams.pEE_param->coreSize;
		pParams->EE_param.delta = mISP_ConfigParams.pEE_param->delta;
	}

	return result;
}


/*

ISPResult ISPParamManager::GetIMGInfo(void* imgInfo)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (imgInfo) {
			memcpy((IMG_INFO*)imgInfo, &mImg_Info, sizeof(IMG_INFO));
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
}

ISPResult ISPParamManager::GetRawType(RAW_TYPE* pType)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (pType) {
			*pType = mImg_Info.rawType;
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
}

ISPResult ISPParamManager::GetBLCParam(uint16_t* offset)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (offset) {
			int difBitNum = mISP_ConfigParams.pBLC_param->bitNum - 8;
			if (0 < difBitNum && difBitNum < 4) {
				*offset = (mISP_ConfigParams.pBLC_param->BLCDefaultValue << difBitNum);
			}
			else {
				*offset = mISP_ConfigParams.pBLC_param->BLCDefaultValue;
			}
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
}

ISPResult ISPParamManager::GetLSCParam(float *pGainCh1, float *pGainCh2, float *pGainCh3, float *pGainCh4)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (pGainCh1 && pGainCh2 && pGainCh3 && pGainCh4) {
			for (int32_t i = 0; i < LSC_LUT_HEIGHT; i++) {
				memcpy(pGainCh1 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh1[i], LSC_LUT_WIDTH * sizeof(float));
				memcpy(pGainCh2 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh2[i], LSC_LUT_WIDTH * sizeof(float));
				memcpy(pGainCh3 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh3[i], LSC_LUT_WIDTH * sizeof(float));
				memcpy(pGainCh4 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh4[i], LSC_LUT_WIDTH * sizeof(float));
			}
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
}

ISPResult ISPParamManager::GetGCCParam(double * weight)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (weight) {
			*weight = mISP_ConfigParams.pGCC_param->weight;
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
}

ISPResult ISPParamManager::GetWBParam(double* rGain, double* gGain, double* bGain)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (rGain && gGain && bGain) {
			*rGain = mISP_ConfigParams.pWB_param->WB1stGAMMA2rd ? mISP_ConfigParams.pWB_param->gainType1.rGain : mISP_ConfigParams.pWB_param->gainType2.rGain;
			*gGain = mISP_ConfigParams.pWB_param->WB1stGAMMA2rd ? mISP_ConfigParams.pWB_param->gainType1.gGain : mISP_ConfigParams.pWB_param->gainType2.gGain;
			*bGain = mISP_ConfigParams.pWB_param->WB1stGAMMA2rd ? mISP_ConfigParams.pWB_param->gainType1.bGain : mISP_ConfigParams.pWB_param->gainType2.bGain;
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
}

ISPResult ISPParamManager::GetCCParam(float* pCcm)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (pCcm) {
			for (int32_t row = 0; row < CCM_HEIGHT; row++) {
				memcpy(pCcm + row * CCM_WIDTH, mISP_ConfigParams.pCC_param->CCM[row], CCM_WIDTH * sizeof(float));
			}
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
}

ISPResult ISPParamManager::GetGAMMAPARAM(uint16_t* plut)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (plut) {
			memcpy(plut, &mISP_ConfigParams.pGamma_param->lut, 1024 * sizeof(uint16_t));
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
}

ISPResult ISPParamManager::GetWNRPARAM(int32_t* l1Threshold, int32_t* l2Threshold, int32_t* l3Threshold)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (l1Threshold && l2Threshold && l3Threshold) {
			*l1Threshold = mISP_ConfigParams.pWNR_param->L1_threshold;
			*l2Threshold = mISP_ConfigParams.pWNR_param->L2_threshold;
			*l3Threshold = mISP_ConfigParams.pWNR_param->L3_threshold;
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

 	return result;
}

ISPResult ISPParamManager::GetEERPARAM(double* alpha, int32_t* coreSize, int32_t* delta)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (alpha && coreSize && delta) {
			*alpha = mISP_ConfigParams.pEE_param->alpha;
			*coreSize = mISP_ConfigParams.pEE_param->coreSize;
			*delta = mISP_ConfigParams.pEE_param->delta;
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
}


ISPResult ISPParamManager::SetIMGDimension(int32_t* width, int32_t* height)
{
	ISPResult result = ISP_SUCCESS;

	if (width && height) {
		mImg_Info.width = *width;
		mImg_Info.height = *height;
	}
	else {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null! %d", result);
	}

	return result;
}

ISPResult ISPParamManager::SetIMGWidth(int32_t* width)
{
	ISPResult result = ISP_SUCCESS;

	if (width) {
		mImg_Info.width = *width;
	}
	else {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null! %d", result);
	}

	return result;
}

ISPResult ISPParamManager::SetIMGHeight(int32_t* height)
{
	ISPResult result = ISP_SUCCESS;

	if (height) {
		mImg_Info.height = *height;
	}
	else {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input is null! %d", result);
	}

	return result;
}

ISPResult ISPParamManager::SetBLCParam(uint16_t* offset)
{
	ISPResult result = ISP_SUCCESS;
	return result;
}

//Temporally do nothing
ISPResult ISPParamManager::SetLSCParam(float* pRGain, float* pGrGain, float* pGbGain, float* pBGain)
{
	ISPResult result = ISP_SUCCESS;
	return result;
}

ISPResult ISPParamManager::SetGCCParam(double* weight)
{
	ISPResult result = ISP_SUCCESS;
	return result;
}

ISPResult ISPParamManager::SetWBParam(double* rGain, double* gGain, double* bGain)
{
	ISPResult result = ISP_SUCCESS;
	return result;
}

ISPResult ISPParamManager::SetCCParam(float* pCcm)
{
	ISPResult result = ISP_SUCCESS;
	return result;
}

ISPResult ISPParamManager::SetGAMMAPARAM(uint16_t* plut)
{
	ISPResult result = ISP_SUCCESS;
	return result;
}

ISPResult ISPParamManager::SetWNRPARAM(int32_t* l1Threshold, int32_t* l2Threshold, int32_t* l3Threshold)
{
	ISPResult result = ISP_SUCCESS;
	return result;
}

ISPResult ISPParamManager::SetEERPARAM(double* alph, int32_t* coreSize, int32_t* delta)
{
	ISPResult result = ISP_SUCCESS;
	return result;
}
*/
