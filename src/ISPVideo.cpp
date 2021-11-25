//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ISPVideo.cpp
// @brief: Support video record.
//////////////////////////////////////////////////////////////////////////////////////

#include "ISPVideo.h"

using namespace cv;

ISPVideo::ISPVideo():
	pSrc(nullptr),
	pFileMgr(nullptr),
	pParamMgr(nullptr),
	mState(VIDEO_NEW)
{
}

ISPVideo::~ISPVideo()
{
}

ISPResult ISPVideo::StatusTransform()
{
	ISPResult result = ISP_SUCCESS;

	return result;
}

ISPResult ISPVideo::Init(Mat* pData, ImageFileManager* pFM, ISPParamManager* pPM)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != VIDEO_NEW) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (!pData || !pPM || !pFM) {
			ISPLoge("Invalid param!");
			result = ISP_INVALID_PARAM;
		}
	}

	if (SUCCESS(result)) {
		pSrc = pData;
		pFileMgr = pFM;
		pParamMgr = pPM;
	}

	if (SUCCESS(result)) {
		mState = VIDEO_INITED;
	}

	return result;
}


ISPResult ISPVideo::CreateThread()
{
	ISPResult result = ISP_SUCCESS;

	if (mState != VIDEO_INITED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (pthread_mutex_init(&mMutex, NULL)) {
			result = ISP_FAILED;
			ISPLoge("Failed to init mutex!");
		}
	}

	if (SUCCESS(result)) {
		if (pthread_cond_init(&mCond, NULL)) {
			result = ISP_FAILED;
			ISPLoge("Failed to init condition!");
		}
	}

	if (SUCCESS(result)) {
		if (pthread_create(&mThread, NULL, VideoEncodeFunc, (void*)this)) {
			result = ISP_FAILED;
			ISPLoge("Failed to create thread!");
		}
	}

	if (SUCCESS(result)) {
		ISPLogd("video thread start running");
		mState = VIDEO_READY;
	}

	return result;
}

ISPResult ISPVideo::DestroyThread()
{
	ISPResult result = ISP_SUCCESS;

	if (mState == VIDEO_LOCK || mState == VIDEO_WAIT_FRAME_DONE) {
		ISPLogi("Waite video thread finish", mState);
	}

	if (SUCCESS(result)) {
		if (pthread_join(mThread, NULL)) {
			result = ISP_FAILED;
			ISPLoge("Wait tid:%d join failed!", mThread);
		}
	}

	if (SUCCESS(result)) {
		if (pthread_mutex_destroy(&mMutex)) {
			result = ISP_FAILED;
			ISPLoge("Failed to destroy mutex");
		}
	}

	if (SUCCESS(result)) {
		if (pthread_cond_destroy(&mCond)) {
			result = ISP_FAILED;
			ISPLoge("Failed to destroy condition");
		}
	}

	if (SUCCESS(result)) {
		ISPLogd("video thread exit");
		mState = VIDEO_INITED;
	}

	return result;
}

ISPResult ISPVideo::GetVideoInfo(OutputVideoInfo* pInfo)
{
	ISPResult result = ISP_SUCCESS;

	if (!pInfo) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input param is null");
	}

	if (SUCCESS(result)) {
		*pInfo = pFileMgr->GetOutputVideoInfo();
	}

	return result;
}

ISPResult ISPVideo::GetSrc(Mat** ppSrc)
{
	ISPResult result = ISP_SUCCESS;

	if (SUCCESS(result)) {
		*ppSrc = pSrc;
	}

	return result;
}

ISPResult ISPVideo::Lock()
{
	ISPResult result = ISP_SUCCESS;

	if (SUCCESS(result)) {
		pthread_mutex_lock(&mMutex);
		mState = VIDEO_LOCK;
	}

	return result;
}

ISPResult ISPVideo::Unlock()
{
	ISPResult result = ISP_SUCCESS;

	if (SUCCESS(result)) {
		pthread_mutex_unlock(&mMutex);
		mState = VIDEO_READY;
	}

	return result;
}

ISPResult ISPVideo::Wait()
{
	ISPResult result = ISP_SUCCESS;

	if (SUCCESS(result)) {
		pthread_cond_wait(&mCond, &mMutex);
		mState = VIDEO_WAIT_FRAME_DONE;
	}

	return result;
}

ISPResult ISPVideo::Notify()
{
	ISPResult result = ISP_SUCCESS;

	if (SUCCESS(result)) {
		result = Lock();
		pthread_cond_signal(&mCond);
		result = Unlock();
	}

	return result;
}

void* VideoEncodeFunc(void* pVideo)
{
	ISPResult result = ISP_SUCCESS;
	ISPVideo* pISPVideo = static_cast<ISPVideo*>(pVideo);
	OutputVideoInfo info = {0};
	Mat* pSrc = nullptr;

	if (!pISPVideo) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invalid param!");
	}

	if (SUCCESS(result)) {
		result = pISPVideo->GetVideoInfo(&info);
	}

	if (SUCCESS(result)) {
		result = pISPVideo->GetSrc(&pSrc);
	}

	if (SUCCESS(result)) {
		VideoWriter vWriter(info.pOutputPath, VideoWriter::fourcc('M', 'J', 'P', 'G'), info.fps, Size(pSrc->cols, pSrc->rows));
		if (vWriter.isOpened()) {
			for (int32_t frameCount = 1; frameCount <= info.frameNum; frameCount++) {
				result = pISPVideo->Lock();
				result = pISPVideo->Wait();
				vWriter << *pSrc;
				ISPLogd("Recording F:%d (%ds)", frameCount, frameCount / info.fps);
				if (frameCount == info.frameNum) {
					ISPLogi("Record finish");
				}
				result = pISPVideo->Unlock();
			}
		}
		else {
			ISPLoge("Failed to initialize VideoWriter!");
		}
	}
	else {
		ISPLoge("Video func not init!");
	}

	return 0;
}
