// License: GPL-3.0-or-later
/*
 * ParamManager aims to manage multi params.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ParamManager.h"
#include "InterfaceWrapper.h"
#include "BufferManager.h"

#include "Setting_1920x1080_D65_1000Lux.h"

const ISPSetting gISPSettings[SETTING_INDEX_NUM] {
	{
		/* 	BLC_SETTING   */	(void *)&BLC_SETTING_1920x1080_D65_1000Lux,
		/* 	LSC_SETTING   */	(void *)&LSC_SETTING_1920x1080_D65_1000Lux,
		/* 	WB_PARM	      */	(void *)&WB_SETTING_1920x1080_D65_1000Lux,
		/* 	CC_SETTING    */	(void *)&CC_SETTING_1920x1080_D65_1000Lux,
		/* 	GAMMA_SETTING */	(void *)&GAMMA_SETTING_1920x1080_D65_1000Lux,
		/* 	WNR_SETTING   */	(void *)&WNR_SETTING_1920x1080_D65_1000Lux,
		/* 	EE_SETTING    */	(void *)&EE_SETTING_1920x1080_D65_1000Lux,
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
	mState = PM_STATE_UNINIT;
}

ISPParamManager::~ISPParamManager()
{
	MemoryPoolItf<uchar> *memMgr = MemoryPool<uchar>::GetInstance();

	{
		unique_lock <mutex> lock(mParamListLock);
		for (auto iter = mActiveParamList.begin(); iter != mActiveParamList.end(); iter++) {
			iter->buf.addr = memMgr->RevertBuffer(static_cast<uchar*>(iter->buf.addr));
		}
		mActiveParamList.clear();
	}
}

ISPParamInfo *ISPParamManager::GetParamInfoById(int id)
{
	ISPParamInfo *pInfo = NULL;

	{
		unique_lock <mutex> lock(mParamListLock);
		for (auto iter = mActiveParamList.begin(); iter != mActiveParamList.end(); iter++) {
			if (iter->id == id) {
				pInfo = &(*iter);
			}
		}
	}

	return pInfo;
}

int32_t ISPParamManager::CreateParam(int32_t hostId, int32_t settingId)
{
	int32_t rt = ISP_SUCCESS;
	ISPParamInfo paramInfo = { 0 };

	paramInfo.id = hostId;
	paramInfo.settingIndex = settingId;
	paramInfo.buf.size = InterfaceWrapper::GetInstance()->GetAlgParamSize();
	paramInfo.buf.addr = MemoryPool<uchar>::GetInstance()->RequireBuffer(paramInfo.buf.size);

	rt = FillinParam(&paramInfo);
	if (!SUCCESS(rt)) {
		MemoryPool<uchar>::GetInstance()->RevertBuffer(static_cast<uchar*>(paramInfo.buf.addr));
		return rt;
	}

	{
		unique_lock <mutex> lock(mParamListLock);
		mActiveParamList.push_back(paramInfo);
	}

	return rt;
}

int32_t ISPParamManager::DeleteParam(int32_t hostId)
{
	int32_t rt = ISP_FAILED;
	list<ISPParamInfo>::iterator iter = mActiveParamList.begin();

	{
		unique_lock <mutex> lock(mParamListLock);
		while (iter != mActiveParamList.end()) {
			if (iter->id == hostId) {
				iter->buf.addr = MemoryPool<uchar>::GetInstance()->RevertBuffer(static_cast<uchar*>(iter->buf.addr));
				mActiveParamList.erase(iter);
				rt = ISP_SUCCESS;
				break;
			}
			iter++;
		}
	}

	if (!SUCCESS(rt)) {
		ILOGE("Invalid id:%d", hostId);
	}

	return rt;
}

int32_t ISPParamManager::FillinParam(ISPParamInfo *pParamInfo)
{
	int32_t rt = ISP_SUCCESS;

	if (!pParamInfo) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null");
		return rt;
	}

	if (pParamInfo->settingIndex >= SETTING_INDEX_NUM) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled setting index:%d. id:%d", pParamInfo->settingIndex, pParamInfo->id);
		return rt;
	}

	if (!pParamInfo->buf.addr) {
		rt = ISP_MEMORY_ERROR;
		ILOGE("Param addr is null. id:%d index:%d", pParamInfo->id, pParamInfo->settingIndex);
		return rt;
	}

	if (pParamInfo->buf.size != InterfaceWrapper::GetInstance()->GetAlgParamSize()) {
		rt = ISP_MEMORY_ERROR;
		ILOGE("Invalid param size:%u %d", pParamInfo->buf.size, pParamInfo->id, pParamInfo->settingIndex);
		return rt;
	}

	/* TODO[H]: according to list node on/off state to get param */
	for (int32_t paramType = 0; paramType < BZ_PARAM_TYPE_NUM; paramType++) {
		rt |= FillinParamByType(pParamInfo, paramType);
		if (!SUCCESS(rt)) {
			return rt;
		}
	}

	return rt;
}

void* ISPParamManager::GetParam(int32_t id, int32_t type)
{
	return GetParamByType(GetParamInfoById(id), type);
}

void ISPParamManager::DumpParamInfo()
{
	{
		unique_lock <mutex> lock(mParamListLock);
		ILOGI("Total %d active param.", mActiveParamList.size());
		for (auto iter = mActiveParamList.begin(); iter != mActiveParamList.end(); iter++) {
			ILOGI("param[%d] id:%d index:%d size:%u addr:0x%x)",
					iter = mActiveParamList.begin(),
					iter->id,
					iter->settingIndex,
					iter->buf.size,
					iter->buf.addr);
		}
	}
}

void* ISPParamManager::GetParamByType(ISPParamInfo *pInfo, int32_t type)
{
	void *pTargetParam = NULL;

	if (!pInfo) {
		ILOGE("Input is null!");
		DumpParamInfo();
		return NULL;
	}

	BZParam *pParams = static_cast<BZParam*>(pInfo->buf.addr);
	if (!pParams) {
		ILOGE("Param buffer is null!");
		return NULL;
	}

	switch(type) {
		//TODO[M]: use offset table instead.
		case BZ_PARAM_TYPE_IMAGE_INFO:
			pTargetParam = (void*)&pParams->info;
			break;
		case BZ_PARAM_TYPE_BLC:
			pTargetParam = (void*)&pParams->blc;
			break;
		case BZ_PARAM_TYPE_LSC:
			pTargetParam = (void*)&pParams->lsc;
			break;
		case BZ_PARAM_TYPE_WB:
			pTargetParam = (void*)&pParams->wb;
			break;
		case BZ_PARAM_TYPE_CC:
			pTargetParam = (void*)&pParams->cc;
			break;
		case BZ_PARAM_TYPE_Gamma:
			pTargetParam = (void*)&pParams->gamma;
			break;
		case BZ_PARAM_TYPE_WNR:
			pTargetParam = (void*)&pParams->wnr;
			break;
		case BZ_PARAM_TYPE_EE:
			pTargetParam = (void*)&pParams->ee;
			break;
		case BZ_PARAM_TYPE_DMC:
		case BZ_PARAM_TYPE_RAW2RGB:
		case BZ_PARAM_TYPE_RGB2YUV:
		case BZ_PARAM_TYPE_YUV2RGB:
		case BZ_PARAM_TYPE_NUM:
			/* No param */
			pTargetParam = NULL;
			break;
		default:
			pTargetParam = NULL;
			ILOGE("Invalid param type:%d", type);
			break;
	}

	return pTargetParam;
}

int32_t ISPParamManager::SetMediaInfo(MediaInfo* info)
{
	int32_t rt = ISP_SUCCESS;

	if (!info) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null! %d", rt);
		return rt;
	}

	rt = SetImgInfo(&info->img);
	if (!SUCCESS(rt)) {
		rt = ISP_FAILED;
		ILOGE("Failed to set img info %d", rt);
		return rt;
	}

	rt = SetVideoInfo(&info->video);
	if (!SUCCESS(rt)) {
		rt = ISP_FAILED;
		ILOGE("Failed to set video info %d", rt);
		return rt;
	}

	mState = PM_STATE_MEDIA_INFO_SET;

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

	if (mState < PM_STATE_MEDIA_INFO_SET) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid param manager state:%d", mState);
		return rt;
	}

	if (width && height) {
		*width = mMediaInfo.img.width;
		*height = mMediaInfo.img.height;
	}
	else {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null! %d", rt);
	}

	return rt;
}

int32_t ISPParamManager::GetVideoFPS(int32_t* fps)
{
	int32_t rt = ISP_SUCCESS;

	if (mState < PM_STATE_MEDIA_INFO_SET) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid param manager state:%d", mState);
		return rt;
	}

	if (fps) {
		*fps = mMediaInfo.video.fps;
	}
	else {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null! %d", rt);
	}

	return rt;
}

int32_t ISPParamManager::GetVideoFrameNum(int32_t* num)
{
	int32_t rt = ISP_SUCCESS;

	if (mState < PM_STATE_MEDIA_INFO_SET) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid param manager state:%d", mState);
		return rt;
	}

	if (num) {
		*num = mMediaInfo.video.frameNum;
	}
	else {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null! %d", rt);
	}

	return rt;
}

int32_t ISPParamManager::FillinImgInfo(void *pInfo)
{
	int32_t rt = ISP_SUCCESS;

	BZImgInfo *pImgInfo = static_cast<BZImgInfo*>(pInfo);
	if (!pImgInfo) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
		return rt;
	}

	if (mState != PM_STATE_MEDIA_INFO_SET) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid pm state:%d", mState);
		return rt;
	}

	if (SUCCESS(rt)) {
		pImgInfo->width = mMediaInfo.img.width;
		pImgInfo->height = mMediaInfo.img.height;
		pImgInfo->rawFormat = mMediaInfo.img.rawFormat;
		pImgInfo->bitspp = mMediaInfo.img.bitspp;
		pImgInfo->stride = mMediaInfo.img.stride;
		pImgInfo->bayerOrder = mMediaInfo.img.bayerOrder;
	}

	return rt;
}

int32_t ISPParamManager::FillinParamByType(ISPParamInfo *pInfo, int32_t type)
{
	int32_t rt = ISP_SUCCESS;

	if (!pInfo) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
		return rt;
	}

	BZParam *pParams = static_cast<BZParam*>(pInfo->buf.addr);
	if (!pParams) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Param buffer is null!");
		return rt;
	}

	void *pTargetParam = GetParamByType(pInfo, type);
	switch(type) {
		/* TODO[M]: use a func array. */
		case BZ_PARAM_TYPE_IMAGE_INFO:
			rt = FillinImgInfo(pTargetParam);
			break;
		case BZ_PARAM_TYPE_BLC:
			rt = FillinBLCParam(pTargetParam, (ISPSetting*)&gISPSettings[pInfo->settingIndex]);
			break;
		case BZ_PARAM_TYPE_LSC:
			rt = FillinLSCParam(pTargetParam, (ISPSetting*)&gISPSettings[pInfo->settingIndex]);
			break;
		case BZ_PARAM_TYPE_WB:
			rt = FillinWBParam(pTargetParam, (ISPSetting*)&gISPSettings[pInfo->settingIndex]);
			break;
		case BZ_PARAM_TYPE_CC:
			rt = FillinCCParam(pTargetParam, (ISPSetting*)&gISPSettings[pInfo->settingIndex]);
			break;
		case BZ_PARAM_TYPE_Gamma:
			rt = FillinGAMMAParam(pTargetParam, (ISPSetting*)&gISPSettings[pInfo->settingIndex]);
			break;
		case BZ_PARAM_TYPE_WNR:
			rt = FillinWNRParam(pTargetParam, (ISPSetting*)&gISPSettings[pInfo->settingIndex]);
			break;
		case BZ_PARAM_TYPE_EE:
			rt = FillinEEParam(pTargetParam, (ISPSetting*)&gISPSettings[pInfo->settingIndex]);
			break;
		case BZ_PARAM_TYPE_DMC:
		case BZ_PARAM_TYPE_RAW2RGB:
		case BZ_PARAM_TYPE_RGB2YUV:
		case BZ_PARAM_TYPE_YUV2RGB:
			/* No param needs to fill in */
			break;
		case BZ_PARAM_TYPE_NUM:
		default:
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid param type:%d", type);
			break;
	}

	return rt;
}

int32_t ISPParamManager::FillinBLCParam(void *pParam, ISPSetting *pSetting)
{
	int32_t rt = ISP_SUCCESS;

	BZBlcParam* pBlcParam = static_cast<BZBlcParam*>(pParam);
	if (!pBlcParam || !pSetting) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
		return rt;
	}

	int difBitNum = static_cast<BlcSetting*>(pSetting->pBlc)->bitNum - 8;
	if (0 < difBitNum && difBitNum < 4) {
		pBlcParam->offset = (static_cast<BlcSetting*>(pSetting->pBlc)->BlcDefaultValue << difBitNum);
	}
	else {
		pBlcParam->offset = static_cast<BlcSetting*>(pSetting->pBlc)->BlcDefaultValue;
	}

	return rt;
}

int32_t ISPParamManager::FillinLSCParam(void *pParam, ISPSetting *pSetting)
{
	int32_t rt = ISP_SUCCESS;

	BZLscParam *pLscParam = static_cast<BZLscParam*>(pParam);
	if (!pLscParam || !pSetting) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
		return rt;
	}

	for (int32_t i = 0; i < LSC_LUT_HEIGHT; i++) {
		memcpy(pLscParam->gainCh1 + i * LSC_LUT_WIDTH, static_cast<LscSetting*>(pSetting->pLsc)->gainCh1[i], LSC_LUT_WIDTH * sizeof(float));
		memcpy(pLscParam->gainCh2 + i * LSC_LUT_WIDTH, static_cast<LscSetting*>(pSetting->pLsc)->gainCh2[i], LSC_LUT_WIDTH * sizeof(float));
		memcpy(pLscParam->gainCh3 + i * LSC_LUT_WIDTH, static_cast<LscSetting*>(pSetting->pLsc)->gainCh3[i], LSC_LUT_WIDTH * sizeof(float));
		memcpy(pLscParam->gainCh4 + i * LSC_LUT_WIDTH, static_cast<LscSetting*>(pSetting->pLsc)->gainCh4[i], LSC_LUT_WIDTH * sizeof(float));
	}

	return rt;
}

int32_t ISPParamManager::FillinWBParam(void *pParam, ISPSetting *pSetting)
{
	int32_t rt = ISP_SUCCESS;

	BZWbParam *pWbParam = static_cast<BZWbParam*>(pParam);
	if (!pWbParam || !pSetting) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
		return rt;
	}

	int32_t mode = static_cast<WbSetting*>(pSetting->pWb)->Wb1stGamma2rd;
	pWbParam->rGain = mode ? static_cast<WbSetting*>(pSetting->pWb)->gainType1.rGain : static_cast<WbSetting*>(pSetting->pWb)->gainType2.rGain;
	pWbParam->gGain = mode ? static_cast<WbSetting*>(pSetting->pWb)->gainType1.gGain : static_cast<WbSetting*>(pSetting->pWb)->gainType2.gGain;
	pWbParam->bGain = mode ? static_cast<WbSetting*>(pSetting->pWb)->gainType1.bGain : static_cast<WbSetting*>(pSetting->pWb)->gainType2.bGain;

	return rt;
}

int32_t ISPParamManager::FillinCCParam(void *pParam, ISPSetting *pSetting)
{
	int32_t rt = ISP_SUCCESS;

	BZCcParam *pCcParam = static_cast<BZCcParam*>(pParam);
	if (!pCcParam || !pSetting) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
		return rt;
	}

	for (int32_t row = 0; row < CCM_HEIGHT; row++) {
		memcpy(pCcParam->ccm + row * CCM_WIDTH, static_cast<CcSetting*>(pSetting->pCc)->ccm[row], CCM_WIDTH * sizeof(float));
	}

	return rt;
}

int32_t ISPParamManager::FillinGAMMAParam(void *pParam, ISPSetting *pSetting)
{
	int32_t rt = ISP_SUCCESS;

	BZGammaParam *pGammaParam = static_cast<BZGammaParam*>(pParam);
	if (!pGammaParam || !pSetting) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
		return rt;
	}

	memcpy(pGammaParam->lut, &static_cast<GammaSetting*>(pSetting->pGamma)->lut, 1024 * sizeof(uint16_t));

	return rt;
}

int32_t ISPParamManager::FillinWNRParam(void *pParam, ISPSetting *pSetting)
{
	int32_t rt = ISP_SUCCESS;

	BZWnrParam *pWnrParam = static_cast<BZWnrParam*>(pParam);
	if (!pWnrParam || !pSetting) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
		return rt;
	}

	for (int32_t l = 0; l < 3; l++) {
		pWnrParam->ch1Threshold[l] = static_cast<WnrSetting*>(pSetting->pWnr)->ch1Threshold[l];
		pWnrParam->ch2Threshold[l] = static_cast<WnrSetting*>(pSetting->pWnr)->ch2Threshold[l];
		pWnrParam->ch3Threshold[l] = static_cast<WnrSetting*>(pSetting->pWnr)->ch3Threshold[l];
	}

	return rt;
}

int32_t ISPParamManager::FillinEEParam(void *pParam, ISPSetting *pSetting)
{
	int32_t rt = ISP_SUCCESS;

	BZEeParam *pEeParam = static_cast<BZEeParam*>(pParam);
	if (!pEeParam || !pSetting) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null!");
		return rt;
	}

	pEeParam->alpha = static_cast<EeSetting*>(pSetting->pEe)->alpha;
	pEeParam->coreSize = static_cast<EeSetting*>(pSetting->pEe)->coreSize;
	pEeParam->sigma = static_cast<EeSetting*>(pSetting->pEe)->sigma;

	return rt;
}

