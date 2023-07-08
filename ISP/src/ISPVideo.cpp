// License: GPL-3.0-or-later
/*
 * ISPVideo aims to support video record in ISP.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPVideo.h"
#include "FileManager.h"

#if DBG_OPENCV_ON
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
#endif
using namespace std;

ISPVideo::ISPVideo():
	pSrc(NULL),
	mState(VIDEO_STATE_NEW)
{
}

ISPVideo::~ISPVideo()
{
}

int32_t ISPVideo::Init(void* pData)
{
	int32_t rt = ISP_SUCCESS;

	if (mState != VIDEO_STATE_NEW) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid state:%d", mState);
		return rt;
	}

	if (!pData) {
		ILOGE("Invalid param!");
		rt = ISP_INVALID_PARAM;
		return rt;
	}

	pSrc = pData;
	mState = VIDEO_STATE_INITED;

	return rt;
}


int32_t ISPVideo::CreateThread(void* pThreadParam)
{
	int32_t rt = ISP_SUCCESS;

	if (mState != VIDEO_STATE_INITED) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid state:%d", mState);
		return rt;
	}

	mThread = thread(VideoEncodeFunc, pThreadParam);
	ILOGD("video thread start running");
	mState = VIDEO_STATE_READY;

	return rt;
}

int32_t ISPVideo::DestroyThread()
{
	int32_t rt = ISP_SUCCESS;

	if (mState == VIDEO_STATE_LOCK || mState == VIDEO_STATE_WAIT_FRAME_DONE) {
		ILOGI("Waite video thread finish", mState);
	}

	mThread.join();
	ILOGD("video thread exit");
	mState = VIDEO_STATE_INITED;

	return rt;
}

int32_t ISPVideo::Record(void* pRecorder, int32_t w, int32_t h)
{
	int32_t rt = ISP_SUCCESS;

	if (!w || !h) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid param");
		return rt;
	}

	{
		unique_lock <mutex> lock(mMutex);
		mCond.wait(lock);
#if DBG_OPENCV_ON
		if (!pRecorder) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Input param is null");
			return rt;
		}

		VideoWriter* pVR = static_cast<VideoWriter*>(pRecorder);
		if (pVR->isOpened()) {
			Mat src = Mat(h, w, CV_8UC3, Scalar(0, 0, 0));
			for (int32_t row = 0; row < h; row++) {
				for (int32_t col = 0; col < w; col++) {
					src.data[row * w * 3 + col * 3] = static_cast<uchar*>(pSrc)[row * w + col];
					src.data[row * w * 3 + col * 3 + 1] = static_cast<uchar*>(pSrc)[w * h + row * w + col];
					src.data[row * w * 3 + col * 3 + 2] = static_cast<uchar*>(pSrc)[2 * w * h + row * w + col];
				}
			}
			*pVR << src;
		}
#else
		ILOGW("Not support video recording");
#endif
	}

	return rt;
}

int32_t ISPVideo::Notify()
{
	int32_t rt = ISP_SUCCESS;

	{
		unique_lock <mutex> lock(mMutex);
		mCond.notify_one();
	}

	return rt;
}

void* VideoEncodeFunc(void* threadParam)
{
	VideoThreadParam* pParam = static_cast<VideoThreadParam*>(threadParam);
	ISPVideo* pISPVideo = NULL;
	OutputVideoInfo info = { 0 };
	void* pWriter = NULL;

	if (!pParam) {
		ILOGE("Invalid thread param!");
		return NULL;
	}

	pISPVideo = static_cast<ISPVideo*>(pParam->pVideo);
	if (!pISPVideo) {
		ILOGE("Invalid param!");
		return NULL;
	}

	if(!SUCCESS(FileManager::GetInstance()->GetOutputVideoInfo(&info))) {
		return NULL;
	}

#if DBG_OPENCV_ON
	pWriter = (void*) new VideoWriter(info.path, VideoWriter::fourcc('M', 'J', 'P', 'G'), info.fps, Size(info.width, info.height));
#endif
	if (!pWriter) {
		ILOGE("Failed to initialize VideoWriter!");
		return NULL;
	}

	for (int32_t frameCount = 1; frameCount <= info.frameNum; frameCount++) {
		pISPVideo->Record(pWriter, info.width, info.height);
		ILOGD("Recording F:%d (%ds)", frameCount, frameCount / info.fps);
		if (frameCount == info.frameNum) {
			ILOGI("Video output path:%s", info.path);
		}
	}

	if (pWriter) {
#if DBG_OPENCV_ON
		delete static_cast<VideoWriter*>(pWriter);
#endif
	}

	return 0;
}
