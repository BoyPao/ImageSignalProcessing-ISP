// License: GPL-3.0-or-later
/*
 * ParamManager aims to manage multi params.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ParamManager.h"
#include "InterfaceWrapper.h"

#include "Param_1920x1080_D65_1000Lux.h"

static ISPCfgParams gISPConfigueParams[PARAM_INDEX_NUM] {
	{
		/* 	BLC_PARAM  */	(void *)&BLCPARAM_1920x1080_D65_1000Lux,
		/* 	LSC_PARAM  */	(void *)&LSCPARM_1920x1080_D65_1000Lux,
		/* 	WB_PARM  */		(void *)&WBPARAM_1920x1080_D65_1000Lux,
		/* 	CC_PARAM  */	(void *)&CCPARAM_1920x1080_D65_1000Lux,
		/* 	GAMMA_PARAM  */	(void *)&GAMMAPARAM_1920x1080_D65_1000Lux,
		/* 	WNR_PARAM  */	(void *)&WNRPARAM_1920x1080_D65_1000Lux,
		/* 	EE_PARAM  */	(void *)&EEPARAM_1920x1080_D65_1000Lux,
	},
};

ISPParamManager* ISPParamManager::GetInstance()
{
	static ISPParamManager gInstance;
	return &gInstance;
}

ISPParamManager::ISPParamManager()
{
	mMediaInfo.img = { 0 };
	mISPConfigParams = { nullptr };
	mState = PM_EMPTY;

	SelectParams(PARAM_1920x1080_D65_1000Lux);
}

ISPParamManager::~ISPParamManager()
{
}

int32_t ISPParamManager::SetMediaInfo(MediaInfo* info)
{
	int32_t rt = ISP_SUCCESS;

	if (!info) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null! %d", rt);
	}

	if (SUCCESS(rt)) {
		rt = SetImgInfo(&info->img);
	}

	if (SUCCESS(rt)) {
		rt = SetVideoInfo(&info->video);
	}

	return rt;
}

int32_t ISPParamManager::SetImgInfo(ImgInfo* info)
{
	int32_t rt = ISP_SUCCESS;

	if (info) {
		memcpy(&mMediaInfo.img, info, sizeof(ImgInfo));
	}
	else {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null! %d", rt);
	}

	return rt;
}

int32_t ISPParamManager::SetVideoInfo(VideoInfo* info)
{
	int32_t rt = ISP_SUCCESS;

	if (info) {
		memcpy(&mMediaInfo.video, info, sizeof(VideoInfo));
	}
	else {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null! %d", rt);
	}

	return rt;
}

int32_t ISPParamManager::GetImgDimension(int32_t* width, int32_t* height)
{
	int32_t rt = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(rt)) {
		if (width && height) {
			*width = mMediaInfo.img.width;
			*height = mMediaInfo.img.height;
		}
		else {
			rt = ISP_INVALID_PARAM;
			ILOGE("Input is null! %d", rt);
		}
	}

	return rt;
}

int32_t ISPParamManager::GetVideoFPS(int32_t* fps)
{
	int32_t rt = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(rt)) {
		if (fps) {
			*fps = mMediaInfo.video.fps;
		}
		else {
			rt = ISP_INVALID_PARAM;
			ILOGE("Input is null! %d", rt);
		}
	}

	return rt;
}

int32_t ISPParamManager::GetVideoFrameNum(int32_t* num)
{
	int32_t rt = ISP_SUCCESS;

	if (mState != PM_SELECTED) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid param manager state:%d", mState);
	}

	if (SUCCESS(rt)) {
		if (num) {
			*num = mMediaInfo.video.frameNum;
		}
		else {
			rt = ISP_INVALID_PARAM;
			ILOGE("Input is null! %d", rt);
		}
	}

	return rt;
}

int32_t ISPParamManager::SelectParams(int32_t paramIndex)
{
	int32_t rt = ISP_SUCCESS;

	if (paramIndex >= PARAM_INDEX_NUM) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled param index:%d. %d", paramIndex, rt);
	}

	if (SUCCESS(rt)) {
		memcpy(&mISPConfigParams,
			&gISPConfigueParams[PARAM_1920x1080_D65_1000Lux + paramIndex],
			sizeof(ISPCfgParams));
	}

	if (SUCCESS(rt)) {
		mState = PM_SELECTED;
	}

	return rt;
}

int32_t ISPParamManager::GetImgInfo(void* pParams)
{
	int32_t rt = ISP_SUCCESS;
	BZParam* param = nullptr;

	param = static_cast<BZParam*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
	}

	if (SUCCESS(rt)) {
		param->info.width = mMediaInfo.img.width;
		param->info.height = mMediaInfo.img.height;
		param->info.rawFormat = mMediaInfo.img.rawFormat;
		param->info.bitspp = mMediaInfo.img.bitspp;
		param->info.stride = mMediaInfo.img.stride;
		param->info.bayerOrder = mMediaInfo.img.bayerOrder;
	}

	return rt;
}

int32_t ISPParamManager::GetParamByCMD(void* pParams, int32_t cmd)
{
	int32_t rt = ISP_SUCCESS;

	if (!pParams) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
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
				ILOGE("Invalid cmd:%d", cmd);
				break;
		}
	}

	return rt;
}

int32_t ISPParamManager::GetBLCParam(void* pParams)
{
	int32_t rt = ISP_SUCCESS;
	BZParam* param = nullptr;

	param = static_cast<BZParam*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
	}

	if (SUCCESS(rt)) {
		int difBitNum = static_cast<BlcParam*>(mISPConfigParams.pBlcParam)->bitNum - 8;
		if (0 < difBitNum && difBitNum < 4) {
			param->blc.offset = (static_cast<BlcParam*>(mISPConfigParams.pBlcParam)->BlcDefaultValue << difBitNum);
		}
		else {
			param->blc.offset = static_cast<BlcParam*>(mISPConfigParams.pBlcParam)->BlcDefaultValue;
		}
	}

	return rt;
}

int32_t ISPParamManager::GetLSCParam(void* pParams)
{
	int32_t rt = ISP_SUCCESS;
	BZParam* param = nullptr;

	param = static_cast<BZParam*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
	}

	if (SUCCESS(rt)) {
		for (int32_t i = 0; i < LSC_LUT_HEIGHT; i++) {
			memcpy(param->lsc.gainCh1 + i * LSC_LUT_WIDTH, static_cast<LscParam*>(mISPConfigParams.pLscParam)->gainCh1[i], LSC_LUT_WIDTH * sizeof(float));
			memcpy(param->lsc.gainCh2 + i * LSC_LUT_WIDTH, static_cast<LscParam*>(mISPConfigParams.pLscParam)->gainCh2[i], LSC_LUT_WIDTH * sizeof(float));
			memcpy(param->lsc.gainCh3 + i * LSC_LUT_WIDTH, static_cast<LscParam*>(mISPConfigParams.pLscParam)->gainCh3[i], LSC_LUT_WIDTH * sizeof(float));
			memcpy(param->lsc.gainCh4 + i * LSC_LUT_WIDTH, static_cast<LscParam*>(mISPConfigParams.pLscParam)->gainCh4[i], LSC_LUT_WIDTH * sizeof(float));
		}
	}
	return rt;
}

int32_t ISPParamManager::GetWBParam(void* pParams)
{
	int32_t rt = ISP_SUCCESS;
	BZParam* param = nullptr;

	param = static_cast<BZParam*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
	}

	if (SUCCESS(rt)) {
		int32_t mode = static_cast<WbParam*>(mISPConfigParams.pWbParam)->Wb1stGamma2rd;
		param->wb.rGain = mode ? static_cast<WbParam*>(mISPConfigParams.pWbParam)->gainType1.rGain : static_cast<WbParam*>(mISPConfigParams.pWbParam)->gainType2.rGain;
		param->wb.gGain = mode ? static_cast<WbParam*>(mISPConfigParams.pWbParam)->gainType1.gGain : static_cast<WbParam*>(mISPConfigParams.pWbParam)->gainType2.gGain;
		param->wb.bGain = mode ? static_cast<WbParam*>(mISPConfigParams.pWbParam)->gainType1.bGain : static_cast<WbParam*>(mISPConfigParams.pWbParam)->gainType2.bGain;
	}

	return rt;
}

int32_t ISPParamManager::GetCCParam(void* pParams)
{
	int32_t rt = ISP_SUCCESS;
	BZParam* param = nullptr;

	param = static_cast<BZParam*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
	}

	if (SUCCESS(rt)) {

		for (int32_t row = 0; row < CCM_HEIGHT; row++) {
			memcpy(param->cc.ccm + row * CCM_WIDTH, static_cast<CcParam*>(mISPConfigParams.pCcParam)->ccm[row], CCM_WIDTH * sizeof(float));
		}
	}

	return rt;
}

int32_t ISPParamManager::GetGAMMAParam(void* pParams)
{
	int32_t rt = ISP_SUCCESS;
	BZParam* param = nullptr;

	param = static_cast<BZParam*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
	}

	if (SUCCESS(rt)) {
		memcpy(param->gamma.lut, &static_cast<GammaParam*>(mISPConfigParams.pGammaParam)->lut, 1024 * sizeof(uint16_t));
	}

	return rt;
}

int32_t ISPParamManager::GetWNRParam(void* pParams)
{
	int32_t rt = ISP_SUCCESS;
	BZParam* param = nullptr;

	param = static_cast<BZParam*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
	}

	if (SUCCESS(rt)) {
		for (int32_t l = 0; l < 3; l++) {
			param->wnr.ch1Threshold[l] = static_cast<WnrParam*>(mISPConfigParams.pWnrParam)->ch1Threshold[l];
			param->wnr.ch2Threshold[l] = static_cast<WnrParam*>(mISPConfigParams.pWnrParam)->ch2Threshold[l];
			param->wnr.ch3Threshold[l] = static_cast<WnrParam*>(mISPConfigParams.pWnrParam)->ch3Threshold[l];
		}
	}

	return rt;
}

int32_t ISPParamManager::GetEEParam(void* pParams)
{
	int32_t rt = ISP_SUCCESS;
	BZParam* param = nullptr;

	param = static_cast<BZParam*>(pParams);
	if (!param) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
	}

	if (SUCCESS(rt)) {
		param->ee.alpha = static_cast<EeParam*>(mISPConfigParams.pEeParam)->alpha;
		param->ee.coreSize = static_cast<EeParam*>(mISPConfigParams.pEeParam)->coreSize;
		param->ee.sigma = static_cast<EeParam*>(mISPConfigParams.pEeParam)->sigma;
	}

	return rt;
}

