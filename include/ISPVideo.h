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

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "Utils.h"
#include "ParamManager.h"

enum VIDEO_STATE {
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

	int32_t Record(cv::VideoWriter* pRecorder, int32_t w, int32_t h);
	int32_t Lock();
	int32_t Unlock();
	int32_t Wait();
	int32_t Notify();

	void* pSrc;

private:
	int32_t StatusTransform();

	VIDEO_STATE mState;

	thread mThread;
	mutex mMutex;
	condition_variable mCond;
};

void* VideoEncodeFunc(void* pVideo);
