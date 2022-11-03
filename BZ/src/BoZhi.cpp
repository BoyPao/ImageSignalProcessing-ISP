// License: GPL-3.0-or-later
/*
 * BoZhi object source file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "Algorithm.h"
#include "BZLog.h"

static BoZhi* gBZ = nullptr;

int32_t WrapEvent(void* msg)
{
	BZResult rt = BZ_SUCCESS;

	if (!gBZ) {
		rt = BZ_FAILED;
		BLOGE("Cannot find BZ!");
	}

	if (SUCCESS(rt)) {
		if (msg) {
			memcpy(&gBZ->mMsg, msg, sizeof(LIB_MSG));
			rt = gBZ->ExecuteCMD();
		} else {
			rt = BZ_FAILED;
			BLOGE("message is null!");
		}
	}

	return (int32_t)rt;
}

int32_t WrapGetOPS(void* pOPS)
{
	BZResult rt = BZ_SUCCESS;

	LIB_OPS* pLibOPS = static_cast<LIB_OPS*>(pOPS);
	if (pLibOPS) {
		/* Add OPS if need */
		pLibOPS->LIB_Event = &WrapEvent;
	} else {
		rt = BZ_INVALID_PARAM;
		BLOGE("Input is nullptr!");
	}

	return (int32_t)rt;
}

int32_t WrapRegistCallbacks(void* pCBs)
{
	BZResult rt = BZ_SUCCESS;

	if (!gBZ) {
		rt = BZ_FAILED;
		BLOGE("Cannot find BZ!");
	}

	if (SUCCESS(rt)) {
		rt = gBZ->RegisterCallbacks(pCBs);
	}

	return (int32_t)rt;
}

int32_t WrapLibInit(void* pOPS)
{
	BZResult rt = BZ_SUCCESS;

	if (gBZ) {
		rt = BZ_FAILED;
		BLOGE("BZ has been inited!");
	} else {
		gBZ = new BoZhi;
		if (!gBZ) {
			rt = BZ_MEMORY_ERROR;
			BLOGE("Create BZ failed!");
		} else {
			rt = gBZ->Init();
		}
	}

	if (SUCCESS(rt)) {
		rt = (BZResult)WrapGetOPS(pOPS);
	}

	return (int32_t)rt;
}

int32_t WrapLibDeInit()
{
	BZResult rt = BZ_SUCCESS;

	if (!gBZ) {
		rt = BZ_FAILED;
		BLOGE("BZ not inited!");
	} else {
		rt = gBZ->DeInit();
		delete gBZ;
		gBZ = nullptr;
	}

	return (int32_t)rt;
}

void* WrapGetBoZhi()
{
	return (void*)gBZ;
}

BoZhi::BoZhi()
{
	mISPCBs = { 0 };
	mState = BZ_STATE_NEW;
}

BoZhi::~BoZhi()
{
	memset(&mISPCBs, 0, sizeof(ISP_CALLBACKS));
}

BZResult BoZhi::Init()
{
	BZResult rt = BZ_SUCCESS;

	if (mState != BZ_STATE_NEW) {
		rt = BZ_STATE_ERROR;
		BLOGE("Invalid state:%d", mState);
	}

	if (SUCCESS(rt)) {
		memset(&mMsg, 0 , sizeof(LIB_MSG));
	}

	if (SUCCESS(rt)) {
		BLOGDC("state %d -> %d", mState, BZ_STATE_INITED);
		mState = BZ_STATE_INITED;
	}

	return rt;
}

BZResult BoZhi::DeInit()
{
	BZResult rt = BZ_SUCCESS;

	if (SUCCESS(rt)) {
		memset(&mMsg, 0 , sizeof(LIB_MSG));
		BLOGDC("state %d -> %d", mState, BZ_STATE_NEW);
		mState = BZ_STATE_NEW;
	}

	return rt;
}

BZResult BoZhi::ExecuteCMD()
{
	BZResult rt = BZ_SUCCESS;
	bool enable = mMsg.enable;

	BLOGDC("cmd:%d on:%d", mMsg.cmd, enable);
	ISP_CALLBACKS tempCbs;
	switch(mMsg.cmd) {
		case LIB_CMD_BLC:
			WrapBlackLevelCorrection(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case LIB_CMD_LSC:
			WrapLensShadingCorrection(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case LIB_CMD_DEMOSAIC:
			WrapDemosaic(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case LIB_CMD_WB:
			WrapWhiteBalance(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case LIB_CMD_CC:
			WrapColorCorrection(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case LIB_CMD_GAMMA:
			WrapGammaCorrection(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case LIB_CMD_WNR:
			WrapWaveletNR(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case LIB_CMD_EE:
			WrapEdgeEnhancement(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case LIB_CMD_RAW2RGB:
			WrapCST_RAW2RGB(mMsg.pSrc, mMsg.pDst, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case LIB_CMD_RGB2YUV:
			WrapCST_RGB2YUV(mMsg.pSrc, mMsg.pDst, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case LIB_CMD_YUV2RGB:
			WrapCST_YUV2RGB(mMsg.pSrc, mMsg.pDst, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case LIB_CMD_NUM:
		default:
			BLOGW("Nothing todo for cmd:%d", mMsg.cmd);
			break;
	}

	return rt;
}

BZResult BoZhi::RegisterCallbacks(void *pCBs)
{
	BZResult rt = BZ_SUCCESS;
	ISP_CALLBACKS* pISPCBs = static_cast<ISP_CALLBACKS*>(pCBs);

	if (pISPCBs) {
		memcpy(&mISPCBs, pISPCBs, sizeof(ISP_CALLBACKS));
	} else {
		rt = BZ_INVALID_PARAM;
		BLOGE("Input is nullptr!");
	}

	return rt;
}

ISP_CALLBACKS const* BoZhi::GetCallbacks()
{
	return &mISPCBs;
}

