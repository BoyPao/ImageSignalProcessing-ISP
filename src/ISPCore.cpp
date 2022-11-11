// License: GPL-3.0-or-later
/*
 * ISP core function implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPCore.h"
#include "ISPSingleton.h"

#if DBG_OPENCV_ON
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo.hpp"

using namespace cv;
#endif

/* For img display with UI window */
#ifdef LINUX_SYSTEM
#include <sys/ioctl.h>
#include <termios.h>
#endif

#define SRCNAME "Source"
#define RESNAME "Result"
#define TMPNAME "Temp"

#define WAIT_ACTIVE_GAP_US 1000

void* CoreFunc(void);

ISPCore::ISPCore():
	mThreadParam(NULL),
	mExit(false),
	mState(CORE_IDLE)
{
	Init();
}

ISPCore::~ISPCore()
{
	DeInit();
}

void ISPCore::Init()
{
	int32_t rt = ISP_SUCCESS;

	if (mState != CORE_IDLE) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid core state:%d", mState);
	}

	if(SUCCESS(rt)) {
		if (!mParamMgr) {
			mParamMgr = std::make_shared<ISPParamManager>();
		}
	}

	if (SUCCESS(rt)) {
		if (!mFileMgr) {
			mFileMgr = std::make_shared<FileManager>();
		}
		rt = mFileMgr->Init();
	}

	if (SUCCESS(rt)) {
		if (!mItfWrapper) {
			mItfWrapper = std::make_shared<InterfaceWrapper>();
		}
		rt = mItfWrapper->Init();
	}

	if (SUCCESS(rt)) {
		if (!mListMgr) {
			mListMgr = std::make_shared<ISPListManager>();
		}
		rt = mListMgr->Init(mParamMgr.get(), mItfWrapper.get());
	}

	if (SUCCESS(rt)) {
		if (!mBufferMgr) {
			mBufferMgr = std::make_shared<MemoryPool<uchar>>();
		}
	}

	if (SUCCESS(rt)) {
		mState = CORE_INITED;
		mThread = thread(CoreFunc);
	}

	return;
}

void ISPCore::DeInit()
{
	mExit = true;
	if (mState != CORE_IDLE) {
		mThread.join();
		mState = CORE_IDLE;
	}
	mThreadParam = NULL;
}

bool ISPCore::IsActive()
{
	return mState == CORE_PROCESSING;
}

void* ISPCore::GetParamManager()
{
	return (mState == CORE_PROCESSING) ? (void*) mParamMgr.get() : NULL;
}

void* ISPCore::GetFileManager()
{
	return (mState == CORE_PROCESSING) ? (void*) mFileMgr.get() : NULL;
}

void* ISPCore::GetListManager()
{
	return (mState == CORE_PROCESSING) ? (void*) mListMgr.get() : NULL;
}

void* ISPCore::GetBufferManager()
{
	return (mState == CORE_PROCESSING) ? (void*) mBufferMgr.get() : NULL;
}

int32_t ISPCore::Process(IOInfo *pInfo, ...)
{
	int32_t rt = ISP_SUCCESS;

	mThreadParam = (void*)pInfo;
	mState = mState == CORE_INITED ? mState + 1 : mState;

	return rt;
}

void* ISPAlloc(size_t size, ...)
{
	ISPCore* pCore = ISPSingleton::GetInstance();
	MemoryPool<uchar>* pBufMgr = NULL;

	if (!pCore) {
		ILOGE("Faild to get core");
		return NULL;
	} else {
		pBufMgr = static_cast<MemoryPool<uchar>*>(pCore->GetBufferManager());
	}

	if (!pBufMgr) {
		ILOGE("Failed to get memory pool");
		return NULL;
	} else {
		return pBufMgr->RequireBuffer(size);
	}
}

void* ISPFree(void* pBuf, ...)
{
	ISPCore* pCore = ISPSingleton::GetInstance();
	MemoryPool<uchar>* pBufMgr = NULL;

	if (!pCore) {
		ILOGE("Faild to get core");
		return NULL;
	} else {
		pBufMgr = static_cast<MemoryPool<uchar>*>(pCore->GetBufferManager());
	}

	if (!pBufMgr) {
		ILOGE("Failed to get memory pool");
		return pBuf;
	} else {
		return (void*)pBufMgr->RevertBuffer(static_cast<uchar*>(pBuf));
	}
}

void* CoreFunc(void)
{
	int32_t rt = ISP_SUCCESS;

	ISPCore* core = ISPSingleton::GetInstance();
	ISPParamManager* pParamManager = NULL;
	FileManager* pFileManager = NULL;
	ISPListManager* pListManager = NULL;
	MemoryPool<uchar>* pBufferManager = NULL;
	IOInfo* pInfo = NULL;
	int32_t numPixel = 0;
	int32_t alignedW = 0;
	int32_t bufferSize = 0;
	void* mipiRawData;
	void* rawData;
	void* bgrData;
	void* yuvData;
	void* postData;
	InputInfo inputInfo = { 0 };
	OutputInfo outputInfo = { 0 };
	MediaInfo mediaInfo = { 0 };
	int32_t listId = 0;
	int32_t frameNum = 0;
	int32_t waitActiveCnt = 0;

	while(!core->IsActive()) {
		if (core->mExit) {
			return NULL;
		}
		waitActiveCnt++;
		usleep(WAIT_ACTIVE_GAP_US);
		ILOGDC("Waiting for core ready... (%d)", waitActiveCnt);
	}

	if (!core->mThreadParam) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Cannot get IO info");
	}

	if (SUCCESS(rt)) {
		pParamManager = static_cast<ISPParamManager*>(core->GetParamManager());
		pFileManager = static_cast<FileManager*>(core->GetFileManager());
		pListManager = static_cast<ISPListManager*>(core->GetListManager());
		pBufferManager = static_cast<MemoryPool<uchar>*>(core->GetBufferManager());
		if (!pParamManager || !pFileManager || !pListManager || !pBufferManager) {
			rt = ISP_FAILED;
			ILOGE("ParamManager:%p FileManager:%p ListManager:%p BufferManager:%p",
					pParamManager, pFileManager, pListManager, pBufferManager);
		} else {
			pInfo = static_cast<IOInfo*>(core->mThreadParam);
			rt = pFileManager->Input(*pInfo);
			if (rt != ISP_SUCCESS) {
				return NULL;
			}
		}
	}

	if (SUCCESS(rt)) {
		rt = pFileManager->GetIOInfo(&mediaInfo);
		if (SUCCESS(rt)) {
			rt = pParamManager->SetMediaInfo(&mediaInfo);
		}
	}

	if (SUCCESS(rt)) {
		numPixel = mediaInfo.img.width * mediaInfo.img.height;
		alignedW = ALIGNx(mediaInfo.img.width, mediaInfo.img.bitspp, CHECK_PACKAGED(mediaInfo.img.rawFormat), mediaInfo.img.stride);
		bufferSize = alignedW * mediaInfo.img.height;
		ILOGI("Image size:%dx%d pixelNum:%d", mediaInfo.img.width, mediaInfo.img.height, numPixel);
		ILOGI("Align (%d,%d) bufferSize:%d", alignedW, mediaInfo.img.height, bufferSize);

		mipiRawData = static_cast<void*>(pBufferManager->RequireBuffer(bufferSize));
		rawData		= static_cast<void*>(pBufferManager->RequireBuffer(numPixel * sizeof(uint16_t) / sizeof (uchar)));
		bgrData		= static_cast<void*>(pBufferManager->RequireBuffer(numPixel * 3 * sizeof(uint16_t) / sizeof(uchar)));
		yuvData		= static_cast<void*>(pBufferManager->RequireBuffer(numPixel * 3));
		postData	= static_cast<void*>(pBufferManager->RequireBuffer(numPixel * 3));
		ILOGDC("buffer addr(%p %p %p %p %p)", mipiRawData, rawData, bgrData, yuvData, postData);
		if (!mipiRawData || !rawData || !bgrData || !yuvData || !postData) {
			rt = ISP_MEMORY_ERROR;
			ILOGE("Failed to alloc buffers!");
		}
	}

	if (SUCCESS(rt)) {
		inputInfo.type = INPUT_FILE_TYPE_RAW;
		outputInfo.imgInfo.type = OUTPUT_FILE_TYPE_BMP;
		rt = pParamManager->GetImgDimension(&outputInfo.imgInfo.width, &outputInfo.imgInfo.height);
		outputInfo.imgInfo.channels = 3;
		outputInfo.videoInfo.type = OUTPUT_FILE_TYPE_AVI;
		rt = pParamManager->GetVideoFPS(&outputInfo.videoInfo.fps);
		rt = pParamManager->GetVideoFrameNum(&outputInfo.videoInfo.frameNum);
		rt = pParamManager->GetImgDimension(&outputInfo.videoInfo.width, &outputInfo.videoInfo.height);

		rt = pFileManager->SetInputInfo(inputInfo);
		rt = pFileManager->SetOutputInfo(outputInfo);
	}

	if (SUCCESS(rt)) {
		rt = pListManager->CreateList((uint16_t*)rawData, (uint16_t*)bgrData, (uint8_t*)yuvData, (uint8_t*)postData, LIST_CFG_DEFAULT, &listId);
	}

	if (SUCCESS(rt)) {
		if (mediaInfo.type >= VIDEO_MEDIA && mediaInfo.type < MEDIA_TYPE_NUM) {
			rt = pFileManager->CreateVideo(postData);
		}
	}

	if (SUCCESS(rt)) {
		if (mediaInfo.type == IMAGE_MEDIA) {
			frameNum = 1;
		}
		else if (mediaInfo.type >= VIDEO_MEDIA && mediaInfo.type < MEDIA_TYPE_NUM) {
			rt = pParamManager->GetVideoFrameNum(&frameNum);
		}

		for (int32_t frameCount = 1; frameCount <= frameNum; frameCount++) {
			ILOGI("L%d =========================== %d(%d) ==========================", listId, frameCount, frameNum);
			if (SUCCESS(rt)) {
				rt = pFileManager->ReadData((uint8_t*)mipiRawData, bufferSize);
				if (SUCCESS(rt)) {
					rt = pFileManager->Mipi10decode((void*)mipiRawData, (void*)rawData, &mediaInfo.img);
				}
			}

			if (SUCCESS(rt)) {
				rt = pListManager->StartById(listId);
				if (!SUCCESS(rt)) {
					ILOGE("Failed to start list:%d %d", listId, rt);
				}
			}

			if (SUCCESS(rt)) {
				if (mediaInfo.type >= VIDEO_MEDIA && mediaInfo.type < MEDIA_TYPE_NUM) {
					pFileManager->SaveVideoData(frameCount);
				}
			}
		}
	}

	if (SUCCESS(rt)) {
		if (mediaInfo.type == IMAGE_MEDIA || mediaInfo.type == IMAGE_AND_VIDEO_MEDIA) {
			rt = pFileManager->SaveImgData((uint8_t*)postData);
		}
	}

	if (SUCCESS(rt)) {
		if (mediaInfo.type >= VIDEO_MEDIA && mediaInfo.type < MEDIA_TYPE_NUM) {
			rt = pFileManager->DestroyVideo();
		}
	}

	if (SUCCESS(rt)) {
		/* Show the result */
		int32_t winSizey;
		bool supportWin = false;
#ifdef LINUX_SYSTEM
		winsize winSize;
		ioctl(STDIN_FILENO, TIOCGWINSZ, &winSize);
		if (!winSize.ws_row) {
			ILOGW("Cannot get terminal size(%d,%d)! Skip img show", winSize.ws_col, winSize.ws_row);
		}
		else {
			winSizey = winSize.ws_row;
			supportWin = true;
		}
#elif defined WIN32_SYSTEM
		winSizey = GetSystemMetrics(SM_CYSCREEN);
		supportWin = true;
#endif

		if (supportWin) {
			int32_t showSizex = 0, showSizey = 0;
			showSizey = winSizey * 2 / 3;
			showSizex = showSizey * mediaInfo.img.width / mediaInfo.img.height;
			ILOGDC("Display size(%dx%d)", showSizex, showSizey);
#if DBG_OPENCV_ON
			Mat dst = Mat(mediaInfo.img.height, mediaInfo.img.width, CV_8UC3, Scalar(0, 0, 0));
			for (int32_t row = 0; row < mediaInfo.img.height; row++) {
				for (int32_t col = 0; col < mediaInfo.img.width; col++) {
					dst.data[row * mediaInfo.img.width * 3 + col * 3] = static_cast<uchar*>(postData)[row * mediaInfo.img.width + col];
					dst.data[row * mediaInfo.img.width * 3 + col * 3 + 1] = static_cast<uchar*>(postData)[numPixel + row * mediaInfo.img.width + col];
					dst.data[row * mediaInfo.img.width * 3 + col * 3 + 2] = static_cast<uchar*>(postData)[2 * numPixel + row * mediaInfo.img.width + col];
				}
			}
			namedWindow(RESNAME, 0);
			resizeWindow(RESNAME, showSizex, showSizey);
			imshow(RESNAME, dst);
			waitKey(0); /* for the possibility of interacting with window, keep the value as 0 */

			if (!dst.empty()) {
				dst.release();
			}
#endif
		}
	}

	return NULL;

}
