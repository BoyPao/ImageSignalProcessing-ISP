// License: GPL-3.0-or-later
/*
 * ISP core function implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPCore.h"

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

ISPCore* ISPCore::GetInstance()
{
	static ISPCore gInstance;
	return &gInstance;
}


ISPCore::ISPCore():
	mExit(false),
	mState(CORE_IDLE)
{
	memset(&mThreadParam, 0, sizeof(IOInfo));
	mThread = thread(CoreFunc);
	mState = CORE_INITED;
}

ISPCore::~ISPCore()
{
	mExit = true;
	if (mState != CORE_IDLE) {
		mThread.join();
		mState = CORE_IDLE;
	}
	InterfaceWrapper::RemoveInstance();
}

bool ISPCore::IsActive()
{
	return mState == CORE_PROCESSING;
}

bool ISPCore::NeedExit()
{
	return mExit;
}

int32_t ISPCore::Process(void *pInfo, ...)
{
	int32_t rt = ISP_SUCCESS;

	memcpy(&mThreadParam, pInfo, sizeof(IOInfo));
	mState = mState == CORE_INITED ? mState + 1 : mState;

	return rt;
}

void* ISPCore::GetThreadParam()
{
	return &mThreadParam;
}

void* ISPAlloc(size_t size, ...)
{
	return MemoryPool<uchar>::GetInstance()->RequireBuffer(size);
}

void* ISPFree(void* pBuf, ...)
{
	return (void*)MemoryPool<uchar>::GetInstance()->RevertBuffer(static_cast<uchar*>(pBuf));
}

int32_t ImgShow(void *data, size_t w, size_t h)
{
	int32_t rt = ISP_SUCCESS;

	if (!data) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Input is null");
		return rt;
	}

	int32_t winSizey;
	bool supportWin = false;
#ifdef LINUX_SYSTEM
	winsize winSize;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &winSize);
	if (!winSize.ws_row) {
		ILOGW("Cannot get terminal size(%d,%d)! Skip img show",
				winSize.ws_col, winSize.ws_row);
	} else {
		winSizey = winSize.ws_row;
		supportWin = true;
	}
#elif defined WIN32_SYSTEM
	winSizey = GetSystemMetrics(SM_CYSCREEN);
	supportWin = true;
#endif

	if (supportWin) {
		int32_t pixelNum = w * h;
		int32_t showSizex = 0, showSizey = 0;
		showSizey = winSizey * 2 / 3;
		showSizex = showSizey * w / h;
		ILOGDC("Display size(%dx%d)", showSizex, showSizey);
#if DBG_OPENCV_ON
		Mat img = Mat(h, w, CV_8UC3, Scalar(0, 0, 0));
		for (size_t row = 0; row < h; row++) {
			for (size_t col = 0; col < w; col++) {
				img.data[row * w * 3 + col * 3] =
					static_cast<uchar*>(data)[row * w + col];
				img.data[row * w * 3 + col * 3 + 1] =
					static_cast<uchar*>(data)[pixelNum + row * w + col];
				img.data[row * w * 3 + col * 3 + 2] =
					static_cast<uchar*>(data)[2 * pixelNum + row * w + col];
			}
		}
		namedWindow("Image", 0);
		resizeWindow("Image", showSizex, showSizey);
		imshow("Image", img);
		waitKey(0); /* for the possibility of interacting with window, keep the value as 0 */

		if (!img.empty()) {
			img.release();
		}
#endif
	}

	return rt;
}

void* CoreFunc(void)
{
	int32_t waitActiveCnt = 0;
	while(!ISPCore::GetInstance()->IsActive()) {
		if (ISPCore::GetInstance()->NeedExit()) {
			return NULL;
		}
		waitActiveCnt++;
		usleep(WAIT_ACTIVE_GAP_US);
		ILOGDC("Waiting for ISP ready... (%d)", waitActiveCnt);
	}

	if (!InterfaceWrapper::GetInstance()) {
		return NULL;
	}

	IOInfo *pInfo = static_cast<IOInfo*>(ISPCore::GetInstance()->GetThreadParam());
	if (FileManager::GetInstance()->Input(*pInfo) != ISP_SUCCESS) {
		return NULL;
	}

	MediaInfo mediaInfo = { 0 };
	if (!SUCCESS(FileManager::GetInstance()->GetIOInfo(&mediaInfo))) {
		return NULL;
	}
	if (!SUCCESS(ISPParamManager::GetInstance()->SetMediaInfo(&mediaInfo))) {
		return NULL;
	}

	int32_t numPixel = 0, alignedW = 0, bufferSize = 0;
	numPixel = mediaInfo.img.width * mediaInfo.img.height;
	alignedW = ALIGNx(mediaInfo.img.width, mediaInfo.img.bitspp,
			CHECK_PACKAGED(mediaInfo.img.rawFormat), mediaInfo.img.stride);
	bufferSize = alignedW * mediaInfo.img.height;
	ILOGI("Image size:%dx%d pixelNum:%d",
			mediaInfo.img.width, mediaInfo.img.height, numPixel);
	ILOGI("Align (%d,%d) bufferSize:%d",
			alignedW, mediaInfo.img.height, bufferSize);

	void *mipiRawData = NULL, *rawData = NULL, *bgrData = NULL, *yuvData = NULL, *postData = NULL;
	mipiRawData = static_cast<void*>(MemoryPool<uchar>::GetInstance()->RequireBuffer(bufferSize));
	rawData		= static_cast<void*>(MemoryPool<uchar>::GetInstance()->RequireBuffer(numPixel * sizeof(uint16_t) / sizeof (uchar)));
	bgrData		= static_cast<void*>(MemoryPool<uchar>::GetInstance()->RequireBuffer(numPixel * 3 * sizeof(uint16_t) / sizeof(uchar)));
	yuvData		= static_cast<void*>(MemoryPool<uchar>::GetInstance()->RequireBuffer(numPixel * 3));
	postData	= static_cast<void*>(MemoryPool<uchar>::GetInstance()->RequireBuffer(numPixel * 3));
	if (!mipiRawData || !rawData || !bgrData || !yuvData || !postData) {
		ILOGE("Failed to alloc buffers!(%p %p %p %p %p)",
				mipiRawData, rawData, bgrData, yuvData, postData);
		return NULL;
	}

	InputInfo inputInfo = { 0 };
	OutputInfo outputInfo = { 0 };
	inputInfo.type = INPUT_FILE_TYPE_RAW;
	outputInfo.imgInfo.type = OUTPUT_FILE_TYPE_BMP;
	if (!SUCCESS(ISPParamManager::GetInstance()->GetImgDimension(&outputInfo.imgInfo.width,
					&outputInfo.imgInfo.height))) {
		return NULL;
	}
	outputInfo.imgInfo.channels = 3;
	outputInfo.videoInfo.type = OUTPUT_FILE_TYPE_AVI;
	if (!SUCCESS(ISPParamManager::GetInstance()->GetVideoFPS(&outputInfo.videoInfo.fps))) {
		return NULL;
	}
	if (!SUCCESS(ISPParamManager::GetInstance()->GetVideoFrameNum(&outputInfo.videoInfo.frameNum))) {
		return NULL;
	}
	if (!SUCCESS(ISPParamManager::GetInstance()->GetImgDimension(&outputInfo.videoInfo.width,
					&outputInfo.videoInfo.height))) {
		return NULL;
	}

	if (!SUCCESS(FileManager::GetInstance()->SetInputInfo(inputInfo))) {
		return NULL;
	}
	if (!SUCCESS(FileManager::GetInstance()->SetOutputInfo(outputInfo))) {
		return NULL;
	}

	int32_t listId = 0;
	if (!SUCCESS(ISPListManager::GetInstance()->CreateList((uint16_t*)rawData,
					(uint16_t*)bgrData,
					(uint8_t*)yuvData,
					(uint8_t*)postData,
					LIST_CFG_DEFAULT, &listId))) {
		return NULL;
	}

	if (mediaInfo.type >= VIDEO_MEDIA && mediaInfo.type < MEDIA_TYPE_NUM) {
		if (!SUCCESS(FileManager::GetInstance()->CreateVideo(postData))) {
			return NULL;
		}
	}

	int32_t frameNum = 0;
	if (mediaInfo.type == IMAGE_MEDIA) {
		frameNum = 1;
	}
	else if (mediaInfo.type >= VIDEO_MEDIA && mediaInfo.type < MEDIA_TYPE_NUM) {
		if (!SUCCESS(ISPParamManager::GetInstance()->GetVideoFrameNum(&frameNum))) {
			return NULL;
		}
	}

	for (int32_t frameCount = 1; frameCount <= frameNum; frameCount++) {
		ILOGI("L%d =========================== %d(%d) ==========================",
				listId, frameCount, frameNum);
		if (!SUCCESS(FileManager::GetInstance()->ReadData((uint8_t*)mipiRawData,
						bufferSize))) {
			return NULL;
		}
		if (!SUCCESS(FileManager::GetInstance()->Mipi10decode((void*)mipiRawData,
						(void*)rawData, &mediaInfo.img))) {
			return NULL;
		}

		if (!SUCCESS(ISPListManager::GetInstance()->StartById(listId))) {
			return NULL;
		}

		if (mediaInfo.type >= VIDEO_MEDIA && mediaInfo.type < MEDIA_TYPE_NUM) {
			if (!SUCCESS(FileManager::GetInstance()->SaveVideoData(frameCount))) {
				ILOGE("Faild to save video for F:%d", frameCount);
				return NULL;
			}
		}
	}

	if (!SUCCESS(FileManager::GetInstance()->SaveImgData((uint8_t*)postData))) {
		return NULL;
	}

	if (mediaInfo.type >= VIDEO_MEDIA && mediaInfo.type < MEDIA_TYPE_NUM) {
		if (!SUCCESS(FileManager::GetInstance()->DestroyVideo())) {
			return NULL;
		}
	}

	if (!SUCCESS(ISPListManager::GetInstance()->DestroyListbyId(listId))) {
		return NULL;
	}

	ImgShow(postData, mediaInfo.img.width, mediaInfo.img.height);

	mipiRawData = MemoryPool<uchar>::GetInstance()->RevertBuffer(static_cast<uchar*>(mipiRawData));
	rawData		= MemoryPool<uchar>::GetInstance()->RevertBuffer(static_cast<uchar*>(rawData));
	bgrData		= MemoryPool<uchar>::GetInstance()->RevertBuffer(static_cast<uchar*>(bgrData));
	yuvData		= MemoryPool<uchar>::GetInstance()->RevertBuffer(static_cast<uchar*>(yuvData));
	postData	= MemoryPool<uchar>::GetInstance()->RevertBuffer(static_cast<uchar*>(postData));

	InterfaceWrapper::RemoveInstance();
	return NULL;
}
