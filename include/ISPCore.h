// License: GPL-3.0-or-later
/*
 * ISPCore head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once

#include "Utils.h"
#include "FileManager.h"
#include "ParamManager.h"
#include "ISPListManager.h"
#include "BufferManager.h"

enum CORE_STATE {
	CORE_IDLE = 0,
	CORE_INITED,
	CORE_CONFIGED,
	CORE_STATE_NUM
};

class ISPCore {
	public:
		ISPCore();
		~ISPCore();
		ISPResult Init();
		ISPResult Config();
		void* GetParamManager();
		void* GetFileManager();
		void* GetListManager();
		void* GetBufferManager();

	private:
		std::shared_ptr<ISPParamManager> mParamMgr;
		std::shared_ptr<FileManager> mFileMgr;
		std::shared_ptr<InterfaceWrapper> mItfWrapper;
		std::shared_ptr<ISPListManager> mListMgr;
		std::shared_ptr<MemoryPool<char>> mBufferMgr;
		CORE_STATE mState;
};

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
			mBufferMgr = std::make_shared<MemoryPool<char>>();
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

void SetCore(void* pCore);
void* GetCore();
