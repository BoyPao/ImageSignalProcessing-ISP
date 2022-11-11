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
		int32_t Init();
		int32_t Config();
		void* GetParamManager();
		void* GetFileManager();
		void* GetListManager();
		void* GetBufferManager();

	private:
		std::shared_ptr<ISPParamManager> mParamMgr;
		std::shared_ptr<FileManager> mFileMgr;
		std::shared_ptr<InterfaceWrapper> mItfWrapper;
		std::shared_ptr<ISPListManager> mListMgr;
		std::shared_ptr<MemoryPool<uchar>> mBufferMgr;
		CORE_STATE mState;
};

void SetCore(void* pCore);
void* GetCore();
void* ISPAlloc(size_t size, ...);
void* ISPFree(void* pBuf, ...);
