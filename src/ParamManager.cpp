//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ParamManager.cpp
// @brief: ParamManager source file. ParamManager aims to manage multi params.
//////////////////////////////////////////////////////////////////////////////////////

#include "ParamManager.h"
#include "InterfaceWrapper.h"

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
	mMediaInfo.img = { 0 };
	mISP_ConfigParams = { nullptr };
	mState = PM_EMPTY;

	SelectParams(PARAM_1920x1080_D65_1000Lux);
}

ISPParamManager::~ISPParamManager()
{
}

ISPResult ISPParamManager::SetMediaInfo(MEDIA_INFO* info)
{
	ISPResult rt = ISP_SUCCESS;

	if (!info) {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null! %d", rt);
	}

	if (SUCCESS(rt)) {
		rt = SetImgInfo(&info->img);
	}

	if (SUCCESS(rt)) {
		rt = SetVideoInfo(&info->video);
	}

	return rt;
}

ISPResult ISPParamManager::SetImgInfo(IMG_INFO* info)
{
	ISPResult rt = ISP_SUCCESS;

	if (info) {
		memcpy(&mMediaInfo.img, info, sizeof(IMG_INFO));
	}
	else {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null! %d", rt);
	}

	return rt;
}

ISPResult ISPParamManager::SetVideoInfo(VIDEO_INFO* info)
{
	ISPResult rt = ISP_SUCCESS;

	if (info) {
		memcpy(&mMediaInfo.video, info, sizeof(VIDEO_INFO));
	}
	else {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null! %d", rt);
	}

	return rt;
}

ISPResult ISPParamManager::GetImgDimension(int32_t* width, int32_t* height)
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		rt = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(rt)) {
		if (width && height) {
			*width = mMediaInfo.img.width;
			*height = mMediaInfo.img.height;
		}
		else {
			rt = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", rt);
		}
	}

	return rt;
}

ISPResult ISPParamManager::GetVideoFPS(int32_t* fps)
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		rt = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(rt)) {
		if (fps) {
			*fps = mMediaInfo.video.fps;
		}
		else {
			rt = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", rt);
		}
	}

	return rt;
}

ISPResult ISPParamManager::GetVideoFrameNum(int32_t* num)
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		rt = ISP_STATE_ERROR;
		ISPLoge("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(rt)) {
		if (num) {
			*num = mMediaInfo.video.frameNum;
		}
		else {
			rt = ISP_INVALID_PARAM;
			ISPLoge("Input is null! %d", rt);
		}
	}

	return rt;
}

ISPResult ISPParamManager::SelectParams(int32_t paramIndex)
{
	ISPResult rt = ISP_SUCCESS;

	if (paramIndex >= PARAM_INDEX_NUM) {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Invaled param index:%d. %d", paramIndex, rt);
	}

	if (SUCCESS(rt)) {
		memcpy(&mISP_ConfigParams,
			&ISPConfigueParams[PARAM_1920x1080_D65_1000Lux + paramIndex],
			sizeof(ISP_Config_Params));
	}

	if (SUCCESS(rt)) {
		mState = PM_SELECTED;
	}

	return rt;
}

ISPResult ISPParamManager::GetImgInfo(void* pParams)
{
	ISPResult rt = ISP_SUCCESS;
	LIB_PARAMS* param = nullptr;

	param = static_cast<LIB_PARAMS*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(rt)) {
		param->info.width = mMediaInfo.img.width;
		param->info.height = mMediaInfo.img.height;
		param->info.rawFormat = (LIB_RAW_FORMAT)mMediaInfo.img.rawFormat;
		param->info.bitspp = mMediaInfo.img.bitspp;
		param->info.stride = mMediaInfo.img.stride;
		param->info.bayerOrder = (LIB_BAYER_ORDER)mMediaInfo.img.bayerOrder;
	}

	return rt;
}

ISPResult ISPParamManager::GetParamByCMD(void* pParams, int32_t cmd)
{
	ISPResult rt = ISP_SUCCESS;

	if (!pParams) {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(rt)) {
		switch(cmd) {
			case ALG_CMD_BLC:
				rt = GetBLCParam(pParams);
				break;
			case ALG_CMD_LSC:
				rt = GetLSCParam(pParams);
				break;
			case ALG_CMD_DEMOSAIC:
				break;
			case ALG_CMD_WB:
				rt = GetWBParam(pParams);
				break;
			case ALG_CMD_CC:
				rt = GetCCParam(pParams);
				break;
			case ALG_CMD_GAMMA:
				rt = GetGAMMAParam(pParams);
				break;
			case ALG_CMD_WNR:
				rt = GetWNRParam(pParams);
				break;
			case ALG_CMD_EE:
				rt = GetEEParam(pParams);
				break;
			case ALG_CMD_CTS_RAW2RGB:
				break;
			case ALG_CMD_CTS_RGB2YUV:
				break;
			case ALG_CMD_CTS_YUV2RGB:
				break;
			case ALG_CMD_NUM:
			default:
				rt = ISP_INVALID_PARAM;
				ISPLoge("Invalid cmd:%d", cmd);
				break;
		}
	}

	return rt;
}

ISPResult ISPParamManager::GetBLCParam(void* pParams)
{
	ISPResult rt = ISP_SUCCESS;
	LIB_PARAMS* param = nullptr;

	param = static_cast<LIB_PARAMS*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(rt)) {
		int difBitNum = mISP_ConfigParams.pBLC_param->bitNum - 8;
		if (0 < difBitNum && difBitNum < 4) {
			param->BLC_param.offset = (mISP_ConfigParams.pBLC_param->BLCDefaultValue << difBitNum);
		}
		else {
			param->BLC_param.offset = mISP_ConfigParams.pBLC_param->BLCDefaultValue;
		}
	}

	return rt;
}

ISPResult ISPParamManager::GetLSCParam(void* pParams)
{
	ISPResult rt = ISP_SUCCESS;
	LIB_PARAMS* param = nullptr;

	param = static_cast<LIB_PARAMS*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(rt)) {
		for (int32_t i = 0; i < LSC_LUT_HEIGHT; i++) {
			memcpy(param->LSC_param.GainCh1 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh1[i], LSC_LUT_WIDTH * sizeof(float));
			memcpy(param->LSC_param.GainCh2 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh2[i], LSC_LUT_WIDTH * sizeof(float));
			memcpy(param->LSC_param.GainCh3 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh3[i], LSC_LUT_WIDTH * sizeof(float));
			memcpy(param->LSC_param.GainCh4 + i * LSC_LUT_WIDTH, mISP_ConfigParams.pLSC_param->GainCh4[i], LSC_LUT_WIDTH * sizeof(float));
		}
	}
	return rt;
}

ISPResult ISPParamManager::GetWBParam(void* pParams)
{
	ISPResult rt = ISP_SUCCESS;
	LIB_PARAMS* param = nullptr;

	param = static_cast<LIB_PARAMS*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(rt)) {
		param->WB_param.rGain = mISP_ConfigParams.pWB_param->WB1stGAMMA2rd ? mISP_ConfigParams.pWB_param->gainType1.rGain : mISP_ConfigParams.pWB_param->gainType2.rGain;
		param->WB_param.gGain = mISP_ConfigParams.pWB_param->WB1stGAMMA2rd ? mISP_ConfigParams.pWB_param->gainType1.gGain : mISP_ConfigParams.pWB_param->gainType2.gGain;
		param->WB_param.bGain = mISP_ConfigParams.pWB_param->WB1stGAMMA2rd ? mISP_ConfigParams.pWB_param->gainType1.bGain : mISP_ConfigParams.pWB_param->gainType2.bGain;
	}

	return rt;
}

ISPResult ISPParamManager::GetCCParam(void* pParams)
{
	ISPResult rt = ISP_SUCCESS;
	LIB_PARAMS* param = nullptr;

	param = static_cast<LIB_PARAMS*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(rt)) {

		for (int32_t row = 0; row < CCM_HEIGHT; row++) {
			memcpy(param->CC_param.CCM + row * CCM_WIDTH, mISP_ConfigParams.pCC_param->CCM[row], CCM_WIDTH * sizeof(float));
		}
	}

	return rt;
}

ISPResult ISPParamManager::GetGAMMAParam(void* pParams)
{
	ISPResult rt = ISP_SUCCESS;
	LIB_PARAMS* param = nullptr;

	param = static_cast<LIB_PARAMS*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(rt)) {
		memcpy(param->Gamma_param.lut, &mISP_ConfigParams.pGamma_param->lut, 1024 * sizeof(uint16_t));
	}

	return rt;
}

ISPResult ISPParamManager::GetWNRParam(void* pParams)
{
	ISPResult rt = ISP_SUCCESS;
	LIB_PARAMS* param = nullptr;

	param = static_cast<LIB_PARAMS*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(rt)) {
		param->WNR_param.L1_threshold = mISP_ConfigParams.pWNR_param->L1_threshold;
		param->WNR_param.L2_threshold = mISP_ConfigParams.pWNR_param->L2_threshold;
		param->WNR_param.L3_threshold = mISP_ConfigParams.pWNR_param->L3_threshold;
	}

	return rt;
}

ISPResult ISPParamManager::GetEEParam(void* pParams)
{
	ISPResult rt = ISP_SUCCESS;
	LIB_PARAMS* param = nullptr;

	param = static_cast<LIB_PARAMS*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ISPLoge("Input is null!");
	}

	if (SUCCESS(rt)) {
		param->EE_param.alpha = mISP_ConfigParams.pEE_param->alpha;
		param->EE_param.coreSize = mISP_ConfigParams.pEE_param->coreSize;
		param->EE_param.delta = mISP_ConfigParams.pEE_param->delta;
	}

	return rt;
}

