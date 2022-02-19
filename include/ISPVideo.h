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

enum VIDEO_STATE{
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

	ISPResult Init(void* pData);
	ISPResult CreateThread(void* pThreadParam);
	ISPResult DestroyThread();
	ISPResult GetSrc(cv::Mat** ppSrc);

	ISPResult Record(cv::VideoWriter* pRecorder);
	ISPResult Lock();
	ISPResult Unlock();
	ISPResult Wait();
	ISPResult Notify();

private:
	ISPResult StatusTransform();

	cv::Mat* pSrc;
	VIDEO_STATE mState;

	thread mThread;
	mutex mMutex;
	condition_variable mCond;
};

void* VideoEncodeFunc(void* pVideo);
