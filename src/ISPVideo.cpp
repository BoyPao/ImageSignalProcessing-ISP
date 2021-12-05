//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ISPVideo.cpp
// @brief: Support video record.
//////////////////////////////////////////////////////////////////////////////////////

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
	ISPResult result = ISP_SUCCESS;

	return result;
}

ISPResult ISPVideo::Init(void* pData)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != VIDEO_NEW) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid state:%d", mState);
	}

	if (SUCCESS(result)) {
		if (!pData) {
			ISPLoge("Invalid param!");
			result = ISP_INVALID_PARAM;
		}
	}

	if (SUCCESS(result)) {
		pSrc = static_cast<Mat*>(pData);
	}

	if (SUCCESS(result)) {
		mState = VIDEO_INITED;
	}

	return result;
}


ISPResult ISPVideo::CreateThread(void* pThreadParam)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != VIDEO_INITED) {
		result = ISP_STATE_ERROR;
		ISPLoge("Invalid state:%d", mState);
	}

	if (SUCCESS(result)) {
		mThread = thread(VideoEncodeFunc, pThreadParam);
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
		mThread.join();
	}

	if (SUCCESS(result)) {
		ISPLogd("video thread exit");
		mState = VIDEO_INITED;
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

ISPResult ISPVideo::Record(VideoWriter* pRecorder)
{
	ISPResult result = ISP_SUCCESS;

	if (!pRecorder) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Input param is null");
	}

	if (SUCCESS(result))
	{
		unique_lock <mutex> lock(mMutex);
		mCond.wait(lock);
		*pRecorder << *pSrc;
	}

	return result;
}

ISPResult ISPVideo::Notify()
{
	ISPResult result = ISP_SUCCESS;

	if (SUCCESS(result))
	{
		unique_lock <mutex> lock(mMutex);
		mCond.notify_one();
	}

	return result;
}

void* VideoEncodeFunc(void* threadParam)
{
	ISPResult result = ISP_SUCCESS;
	VideoThreadParam* pParam = static_cast<VideoThreadParam*>(threadParam);
	ISPVideo* pISPVideo = nullptr;
	FileManager* pFileMgr = nullptr;
	OutputVideoInfo info = { 0 };
	Mat* pSrc = nullptr;

	if (!pParam) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invalid thread param!");
	} else {
		pISPVideo = static_cast<ISPVideo*>(pParam->pVideo);
		pFileMgr = static_cast<FileManager*>(pParam->pFileMgr);
		if (!pISPVideo || !pFileMgr) {
			result = ISP_INVALID_PARAM;
			ISPLoge("Invalid param!");
		}
	}

	if (SUCCESS(result)) {
		result = pFileMgr->GetOutputVideoInfo(&info);
	}

	if (SUCCESS(result)) {
		result = pISPVideo->GetSrc(&pSrc);
	}

	if (SUCCESS(result)) {
		VideoWriter vWriter(info.path, VideoWriter::fourcc('M', 'J', 'P', 'G'), info.fps, Size(pSrc->cols, pSrc->rows));
		if (vWriter.isOpened()) {
			for (int32_t frameCount = 1; frameCount <= info.frameNum; frameCount++) {
				result = pISPVideo->Record(&vWriter);
				ISPLogd("Recording F:%d (%ds)", frameCount, frameCount / info.fps);
				if (frameCount == info.frameNum) {
					ISPLogi("Video output path:%s", info.path);
				}
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
