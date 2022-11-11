// License: GPL-3.0-or-later
/*
 * ISPVideo head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

#include "Utils.h"
#include "ParamManager.h"

enum VideoState {
	VIDEO_NEW = 0,
	VIDEO_INITED,
	VIDEO_READY,
	VIDEO_LOCK,
	VIDEO_WAIT_FRAME_DONE
};

class ISPVideo {
public:
	ISPVideo();
	~ISPVideo();

	int32_t Init(void* pData);
	int32_t CreateThread(void* pThreadParam);
	int32_t DestroyThread();

	int32_t Record(void* pRecorder, int32_t w, int32_t h);
	int32_t Lock();
	int32_t Unlock();
	int32_t Wait();
	int32_t Notify();

	void* pSrc;

private:
	int32_t StatusTransform();

	int32_t mState;

	thread mThread;
	mutex mMutex;
	condition_variable mCond;
};

void* VideoEncodeFunc(void* pVideo);
