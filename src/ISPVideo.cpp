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

ISPResult ISPVideo::StatusTransform()
{
	ISPResult rt = ISP_SUCCESS;

	return rt;
}

ISPResult ISPVideo::Init(void* pData)
{
	ISPResult rt = ISP_SUCCESS;

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
		pSrc = static_cast<Mat*>(pData);
	}

	if (SUCCESS(rt)) {
		mState = VIDEO_INITED;
	}

	return rt;
}


ISPResult ISPVideo::CreateThread(void* pThreadParam)
{
	ISPResult rt = ISP_SUCCESS;

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

ISPResult ISPVideo::DestroyThread()
{
	ISPResult rt = ISP_SUCCESS;

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

ISPResult ISPVideo::GetSrc(Mat** ppSrc)
{
	ISPResult rt = ISP_SUCCESS;

	if (SUCCESS(rt)) {
		*ppSrc = pSrc;
	}

	return rt;
}

ISPResult ISPVideo::Record(VideoWriter* pRecorder)
{
	ISPResult rt = ISP_SUCCESS;

	if (!pRecorder) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input param is null");
	}

	if (SUCCESS(rt))
	{
		unique_lock <mutex> lock(mMutex);
		mCond.wait(lock);
		*pRecorder << *pSrc;
	}

	return rt;
}

ISPResult ISPVideo::Notify()
{
	ISPResult rt = ISP_SUCCESS;

	if (SUCCESS(rt))
	{
		unique_lock <mutex> lock(mMutex);
		mCond.notify_one();
	}

	return rt;
}

void* VideoEncodeFunc(void* threadParam)
{
	ISPResult rt = ISP_SUCCESS;
	VideoThreadParam* pParam = static_cast<VideoThreadParam*>(threadParam);
	ISPVideo* pISPVideo = nullptr;
	FileManager* pFileMgr = nullptr;
	OutputVideoInfo info = { 0 };
	Mat* pSrc = nullptr;

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
		rt = pISPVideo->GetSrc(&pSrc);
	}

	if (SUCCESS(rt)) {
		VideoWriter vWriter(info.path, VideoWriter::fourcc('M', 'J', 'P', 'G'), info.fps, Size(pSrc->cols, pSrc->rows));
		if (vWriter.isOpened()) {
			for (int32_t frameCount = 1; frameCount <= info.frameNum; frameCount++) {
				rt = pISPVideo->Record(&vWriter);
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
