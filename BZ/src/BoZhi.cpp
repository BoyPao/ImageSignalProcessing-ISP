// License: GPL-3.0-or-later
/*
 * BoZhi object source file.
 *
 * Copyright (c) 2019 Penint32_tHao <635945005@qq.com>
 */

#include "Algorithm.h"
#include "BZLog.h"

int32_t WrapEvent(uint32_t *msg)
{
	int32_t rt = BZ_SUCCESS;

	rt = BoZhi::GetInstance()->Event(msg);

	return rt;
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

	return rt;
}

int32_t WrapRegistCallbacks(void* pCBs)
{
	int32_t rt = BZ_SUCCESS;

	rt = BoZhi::GetInstance()->RegisterCallbacks(pCBs);

	return (int32_t)rt;
}

int32_t WrapLibInit(void* pOPS)
{
	int32_t rt = BZ_SUCCESS;

	rt = BoZhi::GetInstance()->Init();
	if (!SUCCESS(rt)) {
		BLOGE("Failed to init BZ");
		return rt;
	}

	rt = WrapGetOPS(pOPS);

	return rt;
}

int32_t WrapLibDeInit()
{
	int32_t rt = BZ_SUCCESS;

	rt = BoZhi::GetInstance()->DeInit();

	return rt;
}

void* WrapAlloc(size_t size)
{
	size_t num = 1;
	return WrapAlloc(size, num);
}

void* WrapAlloc(size_t size, size_t num)
{
	if (BoZhi::GetInstance()->GetCallbacks()->UtilsFuncs.Alloc) {
		return BoZhi::GetInstance()->GetCallbacks()->UtilsFuncs.Alloc(size * num);
	} else {
		return (void*) new u_char[size * num];
	}
}

void* WrapFree(void* pBuf)
{
	if (!pBuf) {
		BLOGE("Cannot free buf:%p", pBuf);
		return pBuf;
	}

	if (BoZhi::GetInstance()->GetCallbacks()->UtilsFuncs.Free) {
		return BoZhi::GetInstance()->GetCallbacks()->UtilsFuncs.Free(pBuf);
	} else {
		delete[] static_cast<u_char*>(pBuf);
		pBuf = NULL;
	}

	return pBuf;
}

BoZhi* BoZhi::GetInstance()
{
	static BoZhi gInstance;
	return &gInstance;
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
	int32_t curState = mState;

	if (mState != BZ_STATE_NEW) {
		rt = BZ_STATE_ERROR;
		BLOGE("Invalid state:%d", mState);
		return rt;
	}

	mState = BZ_STATE_INITED;
	BLOGDC("state %d -> %d", curState, mState);

	return rt;
}

int32_t BoZhi::DeInit()
{
	int32_t rt = BZ_SUCCESS;
	int32_t curState = mState;

	mState = BZ_STATE_NEW;
	rt = DestroyAllProcessor();
	BLOGDC("state %d -> %d", curState, mState);

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

int32_t BoZhi::Event(uint32_t *msg)
{
	int32_t rt = BZ_SUCCESS;

	if (!msg) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Msg is null");
		return rt;
	}

	int32_t cmd = msg[MSG_D0];
	switch (cmd) {
		case BZ_CMD_CREATE_PROC:
			rt = CreateProcessor(msg[MSG_D1]);
			break;
		case BZ_CMD_PROCESS:
			rt = Process(msg);
			break;
		case BZ_CMD_NUM:
		default:
			rt = BZ_FAILED;
			BLOGE("Invalid cmd:%d", cmd);
			PrintMessage(msg);
			break;
	}

	return rt;
}

void BoZhi::PrintMessage(uint32_t *msg)
{
	if (!msg) {
		BLOGE("msg is null");
		return;
	}
	for (size_t i = 0; i < MSG_NUM_MAX; i++) {
		BLOGI("msg[%d]:0x%x", i, msg[i]);
	}
}

int32_t BoZhi::CreateProcessor(int32_t id)
{
	int32_t rt = BZ_SUCCESS;

	{
		unique_lock <mutex> lock(procMapLock);
		if (mProcMap.find(id) != mProcMap.end()) {
			rt = BZ_FAILED;
			BLOGE("Processor(%d) already exist", id);
		} else {
			Processor *pProc = new Processor(id);
			if (pProc) {
				mProcMap.insert(make_pair(id, pProc));
			} else {
				rt = BZ_MEMORY_ERROR;
				BLOGE("Failed to new processor(%d)", id);
			}
		}
	}

	return rt;
}

int32_t BoZhi::DestroyProcessorById(int32_t id)
{
	int32_t rt = BZ_SUCCESS;

	Processor *pProc = FindProcessorById(id);
	if (!pProc) {
		rt = BZ_FAILED;
		BLOGE("Cannot find Processor(%d)", id);
		return rt;
	}

	delete pProc;

	{
		unique_lock <mutex> lock(procMapLock);
		mProcMap.erase(id);
	}
	BLOGDC("Processor(%d) is destroied", id);

	return rt;
}

int32_t BoZhi::DestroyAllProcessor()
{
	int32_t rt = BZ_SUCCESS;
	int32_t id = 0;

	while(!mProcMap.empty()) {
		{
			unique_lock <mutex> lock(procMapLock);
			id = mProcMap.begin()->first;
		}
		rt |= DestroyProcessorById(id);
	}

	return rt;
}

Processor *BoZhi::FindProcessorById(int32_t id)
{
	Processor* pProc = NULL;

	{
		unique_lock <mutex> lock(procMapLock);
		map<int32_t, Processor*>::iterator iter = mProcMap.find(id);
		if (iter == mProcMap.end()) {
			BLOGE("Cannot find Processor(%d)", id);
		} else {
			pProc = iter->second;
		}
	}

	return pProc;
}

int32_t BoZhi::Process(uint32_t *msg)
{
	int32_t rt = BZ_SUCCESS;

	if (!msg) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Msg is null");
		return rt;
	}

	int32_t id = msg[MSG_D1];
	Processor *pProc = FindProcessorById(id);
	if (!pProc) {
		rt = BZ_FAILED;
		BLOGE("Cannot find Processor(%d)", id);
		return rt;
	}

	void* pVCtrl = NULL;
#if  __WORDSIZE ==  64
	pVCtrl = (void*)(((uint64_t)msg[MSG_D3] & 0xffffffff) | (((uint64_t)msg[MSG_D4] << 32) & 0xffffffff00000000));
#elif __WORDSIZE == 32
	pVCtrl = (void*)msg[MSG_D3];
#endif
	BZCtrl* pCtrl = NULL;
	pCtrl = static_cast<BZCtrl*>(pVCtrl);
	if (!pCtrl) {
		rt = BZ_FAILED;
		BLOGE("Faild to get ctrl(%p) from msg", pCtrl);
		return rt;
	}

	BZParam* pParam = static_cast<BZParam*>(pCtrl->pInfo);

	Job job = {0};
	job.info.type = msg[MSG_D2];
	job.info.en = pCtrl->en;
	job.info.srcInfo.w = pParam->info.width;
	job.info.srcInfo.h = pParam->info.height;
	job.info.srcInfo.fmt = pParam->info.rawFormat;
	job.info.srcInfo.order = pParam->info.bayerOrder;
	job.info.srcInfo.bpp = pParam->info.bitspp;
	job.info.srcInfo.buf.size = 0;
	job.info.srcInfo.buf.pAddr = pCtrl->pSrc;
	job.info.dstInfo.w = pParam->info.width;
	job.info.dstInfo.h = pParam->info.height;
	job.info.dstInfo.fmt = pParam->info.rawFormat;
	job.info.dstInfo.order = pParam->info.bayerOrder;
	job.info.dstInfo.bpp =pParam->info.bitspp;
	job.info.dstInfo.buf.size = 0;
	job.info.dstInfo.buf.pAddr = pCtrl->pDst;
	job.info.param.size = 0;
	job.info.param.pAddr = pCtrl->pParam;

	rt = pProc->ScheduleJob(&job);

	return rt;
}
