//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ParamManager.cpp
// @brief: ParamManager source file. Implement a parameter manager for ISP params.
//////////////////////////////////////////////////////////////////////////////////////

#include "ParamManager.h"

#include "Param_1920x1080_D65_1000Lux.h"

ISP_PARAMS ISP_Params_Configue{
	/* 	BLC_PARAM  */	&BLCPARAM_1920x1080_D65_1000Lux,
	/* 	LSC_PARAM  */	&LSCPARM_1920x1080_D65_1000Lux,
	/* 	GCC_PARAM  */	&GCCPARAM_1920x1080_D65_1000Lux,
	/* 	WB_PARM  */		&WBPARAM_1920x1080_D65_1000Lux,
	/* 	CC_PARAM  */	&CCPARAM_1920x1080_D65_1000Lux,
	/* 	GAMMA_PARAM  */	&GAMMAPARAM_1920x1080_D65_1000Lux,
	/* 	WNR_PARAM  */	&WNRPARAM_1920x1080_D65_1000Lux,
	/* 	EE_PARAM  */	&EEPARAM_1920x1080_D65_1000Lux,
};


ISPParamManager::ISPParamManager()
{
	mImg_Info = { 0 };

	mISP_Params = { nullptr };
	mState = PM_EMPTY;

	SelectParams(PARAM_1920x1080_D65_1000Lux);
}

ISPParamManager::~ISPParamManager()
{
}

ISPResult ISPParamManager::SelectParams(int32_t paramIndex)
{
	ISPResult result = ISP_SUCCESS;

	switch (paramIndex) {
	case PARAM_1920x1080_D65_1000Lux:
		memcpy(&mISP_Params, &ISP_Params_Configue, sizeof(ISP_PARAMS));
		mState = PM_SELECTED;
		break;
	default:
		result = ISP_INVALID_PARAM;
		ISPLoge("Failed to Select %d Params", paramIndex);
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

ISPResult ISPParamManager::GetBLCParam(uint16_t* offset)
{
	ISPResult result = ISP_SUCCESS;
	
	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (offset) {
			int difBitNum = mISP_Params.pBLC_param->bitNum - 8;
			if (0 < difBitNum && difBitNum < 4) {
				*offset = (mISP_Params.pBLC_param->BLCDefaultValue << difBitNum);
			}
			else {
				*offset = mISP_Params.pBLC_param->BLCDefaultValue;
			}
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
}

ISPResult ISPParamManager::GetLSCParam(float *pRGain, float *pGrGain, float *pGbGain, float *pBGain)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (pRGain && pGrGain && pGbGain && pBGain) {
			for (int32_t i = 0; i < LSC_LUT_HEIGHT; i++) {
				memcpy(pRGain + i * LSC_LUT_WIDTH, mISP_Params.pLSC_param->rGain[i], LSC_LUT_WIDTH * sizeof(float));
				memcpy(pGrGain + i * LSC_LUT_WIDTH, mISP_Params.pLSC_param->grGain[i], LSC_LUT_WIDTH * sizeof(float));
				memcpy(pGbGain + i * LSC_LUT_WIDTH, mISP_Params.pLSC_param->gbGain[i], LSC_LUT_WIDTH * sizeof(float));
				memcpy(pBGain + i * LSC_LUT_WIDTH, mISP_Params.pLSC_param->bGain[i], LSC_LUT_WIDTH * sizeof(float));
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
			*weight = mISP_Params.pGCC_param->weight;
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
			*rGain = mISP_Params.pWB_param->WB1stGAMMA2rd ? mISP_Params.pWB_param->gainType1.rGain : mISP_Params.pWB_param->gainType2.rGain;
			*gGain = mISP_Params.pWB_param->WB1stGAMMA2rd ? mISP_Params.pWB_param->gainType1.gGain : mISP_Params.pWB_param->gainType2.gGain;
			*bGain = mISP_Params.pWB_param->WB1stGAMMA2rd ? mISP_Params.pWB_param->gainType1.bGain : mISP_Params.pWB_param->gainType2.bGain;
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
				memcpy(pCcm + row * CCM_WIDTH, mISP_Params.pCC_param->CCM[row], CCM_WIDTH * sizeof(float));
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
			memcpy(plut, &mISP_Params.pGamma_param->lut, 1024 * sizeof(uint16_t));
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
			*l1Threshold = mISP_Params.pWNR_param->L1_threshold;
			*l2Threshold = mISP_Params.pWNR_param->L2_threshold;
			*l3Threshold = mISP_Params.pWNR_param->L3_threshold;
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
			*alpha = mISP_Params.pEE_param->alpha;
			*coreSize = mISP_Params.pEE_param->coreSize;
			*delta = mISP_Params.pEE_param->delta;
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", result);
		}
	}

	return result;
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