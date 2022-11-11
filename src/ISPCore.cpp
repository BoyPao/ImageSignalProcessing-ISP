// License: GPL-3.0-or-later
/*
 * ISP core function implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPCore.h"

ISPCore::ISPCore():
	mState(CORE_IDLE)
{
}

ISPCore::~ISPCore()
{
}

ISPResult ISPCore::Init()
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != CORE_IDLE) {
		rt = ISP_FAILED;
		ILOGE("Invalid core state:%d", mState);
	}

	if(SUCCESS(rt)) {
		if (!mParamMgr) {
			mParamMgr = std::make_shared<ISPParamManager>();
		}
	}

	if (SUCCESS(rt)) {
		if (!mFileMgr) {
			mFileMgr = std::make_shared<FileManager>();
		}
		rt = mFileMgr->Init();
	}

	if (SUCCESS(rt)) {
		if (!mItfWrapper) {
			mItfWrapper = std::make_shared<InterfaceWrapper>();
		}
		rt = mItfWrapper->Init();
	}

	if (SUCCESS(rt)) {
		if (!mListMgr) {
			mListMgr = std::make_shared<ISPListManager>();
		}
		rt = mListMgr->Init(mParamMgr.get(), mItfWrapper.get());
	}

	if (SUCCESS(rt)) {
		if (!mBufferMgr) {
			mBufferMgr = std::make_shared<MemoryPool<uchar>>();
		}
	}

	if (SUCCESS(rt)) {
		mState = CORE_INITED;
	}

	return rt;
}

void* ISPCore::GetParamManager()
{
	return (mState == CORE_INITED) ? (void*) mParamMgr.get() : nullptr;
}

void* ISPCore::GetFileManager()
{
	return (mState == CORE_INITED) ? (void*) mFileMgr.get() : nullptr;
}

void* ISPCore::GetListManager()
{
	return (mState == CORE_INITED) ? (void*) mListMgr.get() : nullptr;
}

void* ISPCore::GetBufferManager()
{
	return (mState == CORE_INITED) ? (void*) mBufferMgr.get() : nullptr;
}

void* ISPAlloc(size_t size, ...)
{
	ISPCore* pCore = static_cast<ISPCore*>(GetCore());
	MemoryPool<uchar>* pBufMgr = NULL;

	if (!pCore) {
		ILOGE("Faild to get core");
		return NULL;
	} else {
		pBufMgr = static_cast<MemoryPool<uchar>*>(pCore->GetBufferManager());
	}

	if (!pBufMgr) {
		ILOGE("Failed to get memory pool");
		return NULL;
	} else {
		size_t num = 1;
		va_list va_param;
		va_start(va_param, size);
		num = va_arg(va_param, size_t);
		va_end(va_param);
		return pBufMgr->RequireBuffer(size, num);
	}
}

void* ISPFree(void* pBuf, ...)
{
	ISPCore* pCore = static_cast<ISPCore*>(GetCore());
	MemoryPool<uchar>* pBufMgr = NULL;

	if (!pCore) {
		ILOGE("Faild to get core");
		return NULL;
	} else {
		pBufMgr = static_cast<MemoryPool<uchar>*>(pCore->GetBufferManager());
	}

	if (!pBufMgr) {
		ILOGE("Failed to get memory pool");
		return NULL;
	} else {
		return (void*)pBufMgr->RevertBuffer(static_cast<uchar*>(pBuf));
	}
}
