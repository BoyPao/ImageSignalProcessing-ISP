// License: GPL-3.0-or-later
/*
 * BoZhi object source file.
 *
 * Copyright (c) 2019 Penint32_tHao <635945005@qq.com>
 */

#include "Algorithm.h"
#include "BZLog.h"

static BoZhi* gBZ = nullptr;

int32_t WrapEvent(void* msg)
{
	int32_t rt = BZ_SUCCESS;

	if (!gBZ) {
		rt = BZ_FAILED;
		BLOGE("Cannot find BZ!");
	}

	if (SUCCESS(rt)) {
		if (msg) {
			memcpy(&gBZ->mMsg, msg, sizeof(BZMsg));
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
	int32_t rt = BZ_SUCCESS;

	BZOps* pLibOPS = static_cast<BZOps*>(pOPS);
	if (pLibOPS) {
		/* Add OPS if need */
		pLibOPS->BZEvent = &WrapEvent;
	} else {
		rt = BZ_INVALID_PARAM;
		BLOGE("Input is nullptr!");
	}

	return (int32_t)rt;
}

int32_t WrapRegistCallbacks(void* pCBs)
{
	int32_t rt = BZ_SUCCESS;

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
	int32_t rt = BZ_SUCCESS;

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
		rt = WrapGetOPS(pOPS);
	}

	return (int32_t)rt;
}

int32_t WrapLibDeInit()
{
	int32_t rt = BZ_SUCCESS;

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

void* WrapAlloc(size_t size)
{
	size_t num = 1;
	return WrapAlloc(size, num);
}

void* WrapAlloc(size_t size, size_t num)
{
	BoZhi* pBZ = static_cast<BoZhi*>(WrapGetBoZhi());

	if (!pBZ) {
		BLOGE("Failed to get BZ!");
		return NULL;
	}

	if (pBZ->mISPCBs.UtilsFuncs.Alloc) {
		return pBZ->mISPCBs.UtilsFuncs.Alloc(size * num);
	} else {
		return (void*) new u_char[size * num];
	}
}

void* WrapFree(void* pBuf)
{
	BoZhi* pBZ = static_cast<BoZhi*>(WrapGetBoZhi());

	if (!pBZ) {
		BLOGE("Failed to get BZ!");
		return pBuf;
	}

	if (pBuf) {
		if (pBZ->mISPCBs.UtilsFuncs.Free) {
			return pBZ->mISPCBs.UtilsFuncs.Free(pBuf);
		} else {
			delete[] static_cast<u_char*>(pBuf);
			pBuf = NULL;
		}
	}

	return pBuf;
}

BoZhi::BoZhi()
{
	mISPCBs = { 0 };
	mState = BZ_STATE_NEW;
}

BoZhi::~BoZhi()
{
	memset(&mISPCBs, 0, sizeof(ISPCallbacks));
}

int32_t BoZhi::Init()
{
	int32_t rt = BZ_SUCCESS;

	if (mState != BZ_STATE_NEW) {
		rt = BZ_STATE_ERROR;
		BLOGE("Invalid state:%d", mState);
	}

	if (SUCCESS(rt)) {
		memset(&mMsg, 0 , sizeof(BZMsg));
	}

	if (SUCCESS(rt)) {
		BLOGDC("state %d -> %d", mState, BZ_STATE_INITED);
		mState = BZ_STATE_INITED;
	}

	return rt;
}

int32_t BoZhi::DeInit()
{
	int32_t rt = BZ_SUCCESS;

	if (SUCCESS(rt)) {
		memset(&mMsg, 0 , sizeof(BZMsg));
		BLOGDC("state %d -> %d", mState, BZ_STATE_NEW);
		mState = BZ_STATE_NEW;
	}

	return rt;
}

int32_t BoZhi::ExecuteCMD()
{
	int32_t rt = BZ_SUCCESS;
	bool enable = mMsg.enable;

	BLOGDC("cmd:%d on:%d", mMsg.cmd, enable);
	ISPCallbacks tempCbs;
	switch(mMsg.cmd) {
		case BZ_CMD_BLC:
			WrapBlackLevelCorrection(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case BZ_CMD_LSC:
			WrapLensShadingCorrection(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case BZ_CMD_DEMOSAIC:
			WrapDemosaic(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case BZ_CMD_WB:
			WrapWhiteBalance(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case BZ_CMD_CC:
			WrapColorCorrection(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case BZ_CMD_GAMMA:
			WrapGammaCorrection(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case BZ_CMD_WNR:
			WrapWaveletNR(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case BZ_CMD_EE:
			WrapEdgeEnhancement(mMsg.pSrc, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case BZ_CMD_RAW2RGB:
			WrapCST_RAW2RGB(mMsg.pSrc, mMsg.pDst, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case BZ_CMD_RGB2YUV:
			WrapCST_RGB2YUV(mMsg.pSrc, mMsg.pDst, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case BZ_CMD_YUV2RGB:
			WrapCST_YUV2RGB(mMsg.pSrc, mMsg.pDst, mMsg.pParam, tempCbs, mMsg.enable);
			break;
		case BZ_CMD_NUM:
		default:
			BLOGW("Nothinint32_ttodo for cmd:%d", mMsg.cmd);
			break;
	}

	return rt;
}

int32_t BoZhi::RegisterCallbacks(void *pCBs)
{
	int32_t rt = BZ_SUCCESS;
	ISPCallbacks* pISPCBs = static_cast<ISPCallbacks*>(pCBs);

	if (pISPCBs) {
		memcpy(&mISPCBs, pISPCBs, sizeof(ISPCallbacks));
	} else {
		rt = BZ_INVALID_PARAM;
		BLOGE("Input is nullptr!");
	}

	return rt;
}

ISPCallbacks const* BoZhi::GetCallbacks()
{
	return &mISPCBs;
}

