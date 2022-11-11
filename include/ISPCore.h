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

#include <thread>
#include <mutex>
#include <condition_variable>

enum CORE_STATE {
	CORE_IDLE = 0,
	CORE_INITED,
	CORE_PROCESSING,
	CORE_STATE_NUM
};

class ISPCore {
	public:
		ISPCore();
		~ISPCore();
		void *GetParamManager();
		void *GetFileManager();
		void *GetListManager();
		void *GetBufferManager();
		int32_t Process(IOInfo *pInfo, ...);
		bool IsActive();

		void *mThreadParam;
		bool mExit;

	private:
		void Init();
		void DeInit();
		std::shared_ptr<ISPParamManager> mParamMgr;
		std::shared_ptr<FileManager> mFileMgr;
		std::shared_ptr<InterfaceWrapper> mItfWrapper;
		std::shared_ptr<ISPListManager> mListMgr;
		std::shared_ptr<MemoryPool<uchar>> mBufferMgr;
		int32_t mState;
		thread mThread;
};

void* ISPAlloc(size_t size, ...);
void* ISPFree(void* pBuf, ...);
