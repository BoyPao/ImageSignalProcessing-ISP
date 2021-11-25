//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ISPVideo.h
// @brief: ISPVideo head file.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <pthread.h>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "Utils.h"
#include "ParamManager.h"
#include "FileManager.h"

using namespace cv;

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

	ISPResult Init(Mat* pData, ImageFileManager* pFM, ISPParamManager* pPM);
	ISPResult CreateThread();
	ISPResult DestroyThread();
	ISPResult GetVideoInfo(OutputVideoInfo* pInfo);
	ISPResult GetSrc(Mat** ppSrc);

	ISPResult Lock();
	ISPResult Unlock();
	ISPResult Wait();
	ISPResult Notify();

private:
	ISPResult StatusTransform();

	Mat* pSrc;
	ImageFileManager* pFileMgr;
	ISPParamManager* pParamMgr;
	VIDEO_STATE mState;

	pthread_t mThread;
	pthread_mutex_t mMutex;
	pthread_cond_t mCond;
};

void* VideoEncodeFunc(void* pVideo);
