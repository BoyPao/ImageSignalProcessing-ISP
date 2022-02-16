//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ISPCore.h
// @brief: ISPCore head file.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Utils.h"
#include "FileManager.h"
#include "ParamManager.h"
#include "ISPListManager.h"

enum CORE_STATE {
	CORE_IDLE = 0,
	CORE_INITED,
	CORE_CONFIGED,
	CORE_STATE_NUM
};

class ISPCore {
	public:
		ISPCore();
		ISPResult Init();
		ISPResult Config();
		void* GetParamManager();
		void* GetFileManager();
		void* GetListManager();

	private:
		std::shared_ptr<ISPParamManager> mParamMgr;
		std::shared_ptr<FileManager> mFileMgr;
		std::shared_ptr<InterfaceWrapper> mItfWrapper;
		std::shared_ptr<ISPListManager> mListMgr;
		//TODO: manage buffer
		//std::shared_ptr<BufferManager> mBufferMgr;
		CORE_STATE mState;
};

ISPCore::ISPCore():
	mState(CORE_IDLE)
{

}

ISPResult ISPCore::Init()
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != CORE_IDLE) {
		rt = ISP_FAILED;
		ISPLoge("Invalid core state:%d", mState);
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
	}

	if (SUCCESS(rt)) {
		if (!mListMgr) {
			mListMgr = std::make_shared<ISPListManager>();
		}
		rt = mListMgr->Init(mParamMgr.get(), mItfWrapper.get());
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
