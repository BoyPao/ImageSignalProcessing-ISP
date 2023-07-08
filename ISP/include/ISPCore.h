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
#include "MemPool.h"
#include "ISPItf.h"

#include <thread>
#include <mutex>
#include <condition_variable>

enum CoreState {
	CORE_IDLE = 0,
	CORE_INITED,
	CORE_PROCESSING,
	CORE_STATE_NUM
};

class ISPCore : public ISPItf {
	public:
		static ISPCore* GetInstance();
		virtual int32_t Process(void *pInfo, ...);
		virtual bool IsActive();
		virtual bool NeedExit();
		virtual void* GetThreadParam();

	private:
		ISPCore();
		virtual ~ISPCore();

		bool mExit;
		int32_t mState;
		thread mThread;
		IOInfo mThreadParam;
};

void* ISPAlloc(size_t size, ...);
void* ISPFree(void* pBuf, ...);
