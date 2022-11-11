// License: GPL-3.0-or-later
/*
 * ISPVidoe aims to support video record in ISP.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPVideo.h"
#include "FileManager.h"

using namespace cv;
using namespace std;

ISPVideo::ISPVideo():
	pSrc(nullptr),
	mState(VIDEO_NEW)
{
}

ISPVideo::~ISPVideo()
{
}

int32_t ISPVideo::StatusTransform()
{
	int32_t rt = ISP_SUCCESS;

	return rt;
}

int32_t ISPVideo::Init(void* pData)
{
	int32_t rt = ISP_SUCCESS;

	if (mState != VIDEO_NEW) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid state:%d", mState);
	}

	if (SUCCESS(rt)) {
		if (!pData) {
			ILOGE("Invalid param!");
			rt = ISP_INVALID_PARAM;
		}
	}

	if (SUCCESS(rt)) {
		pSrc = pData;
	}

	if (SUCCESS(rt)) {
		mState = VIDEO_INITED;
	}

	return rt;
}


int32_t ISPVideo::CreateThread(void* pThreadParam)
{
	int32_t rt = ISP_SUCCESS;

	if (mState != VIDEO_INITED) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid state:%d", mState);
	}

	if (SUCCESS(rt)) {
		mThread = thread(VideoEncodeFunc, pThreadParam);
	}

	if (SUCCESS(rt)) {
		ILOGD("video thread start running");
		mState = VIDEO_READY;
	}

	return rt;
}

int32_t ISPVideo::DestroyThread()
{
	int32_t rt = ISP_SUCCESS;

	if (mState == VIDEO_LOCK || mState == VIDEO_WAIT_FRAME_DONE) {
		ILOGI("Waite video thread finish", mState);
	}

	if (SUCCESS(rt)) {
		mThread.join();
	}

	if (SUCCESS(rt)) {
		ILOGD("video thread exit");
		mState = VIDEO_INITED;
	}

	return rt;
}

int32_t ISPVideo::Record(VideoWriter* pRecorder, int32_t w, int32_t h)
{
	int32_t rt = ISP_SUCCESS;

	if (!pRecorder) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input param is null");
	}

	if (!w || !h) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid param");
	}

	if (SUCCESS(rt))
	{
		unique_lock <mutex> lock(mMutex);
		mCond.wait(lock);
		/* TODO: add shared buffer R&W lock */
		Mat src = Mat(h, w, CV_8UC3, Scalar(0, 0, 0));
		for (int32_t row = 0; row < h; row++) {
			for (int32_t col = 0; col < w; col++) {
				src.data[row * w * 3 + col * 3] = static_cast<uchar*>(pSrc)[row * w + col];
				src.data[row * w * 3 + col * 3 + 1] = static_cast<uchar*>(pSrc)[w * h + row * w + col];
				src.data[row * w * 3 + col * 3 + 2] = static_cast<uchar*>(pSrc)[2 * w * h + row * w + col];
			}
		}
		*pRecorder << src;
	}

	return rt;
}

int32_t ISPVideo::Notify()
{
	int32_t rt = ISP_SUCCESS;

	if (SUCCESS(rt))
	{
		unique_lock <mutex> lock(mMutex);
		mCond.notify_one();
	}

	return rt;
}

void* VideoEncodeFunc(void* threadParam)
{
	int32_t rt = ISP_SUCCESS;
	VideoThreadParam* pParam = static_cast<VideoThreadParam*>(threadParam);
	ISPVideo* pISPVideo = nullptr;
	FileManager* pFileMgr = nullptr;
	OutputVideoInfo info = { 0 };

	if (!pParam) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid thread param!");
	} else {
		pISPVideo = static_cast<ISPVideo*>(pParam->pVideo);
		pFileMgr = static_cast<FileManager*>(pParam->pFileMgr);
		if (!pISPVideo || !pFileMgr) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid param!");
		}
	}

	if (SUCCESS(rt)) {
		rt = pFileMgr->GetOutputVideoInfo(&info);
	}

	if (SUCCESS(rt)) {
		VideoWriter vWriter(info.path, VideoWriter::fourcc('M', 'J', 'P', 'G'), info.fps, Size(info.width, info.height));
		if (vWriter.isOpened()) {
			for (int32_t frameCount = 1; frameCount <= info.frameNum; frameCount++) {
				rt = pISPVideo->Record(&vWriter, info.width, info.height);
				ILOGD("Recording F:%d (%ds)", frameCount, frameCount / info.fps);
				if (frameCount == info.frameNum) {
					ILOGI("Video output path:%s", info.path);
				}
			}
		}
		else {
			ILOGE("Failed to initialize VideoWriter!");
		}
	}
	else {
		ILOGE("Video func not init!");
	}

	return 0;
}
