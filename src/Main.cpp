// License: GPL-3.0-or-later
/*
 * ISP main function implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPCore.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo.hpp"

/* For img display with UI window */
#ifdef LINUX_SYSTEM
#include <sys/ioctl.h>
#include <termios.h>
#endif

using namespace cv;

#define SRCNAME "Source"
#define RESNAME "Result"
#define TMPNAME "Temp"

static ISPCore* gpCore = NULL;
void SetCore(void* pCore)
{
	 gpCore = static_cast<ISPCore*>(pCore);
}

void* GetCore()
{
	return gpCore;
}

int main(int argc, char* argv[], char* envp[]) {
	ISPResult rt = ISP_SUCCESS;

	ISPCore core;
	ISPParamManager* pParamManager = nullptr;
	FileManager* pFileManager = nullptr;
	ISPListManager* pListManager = nullptr;
	MemoryPool<uchar>* pBufferManager = nullptr;
	IO_INFO ioInfo = { 0 };
	int32_t numPixel = 0;
	int32_t alignedW = 0;
	int32_t bufferSize = 0;
	void* mipiRawData;
	void* rawData;
	void* bgrData;
	void* yuvData;
	Mat dst;
	InputInfo inputInfo = { 0 };
	OutputInfo outputInfo = { 0 };
	MEDIA_INFO mediaInfo = { 0 };
	int32_t listId = 0;
	int32_t frameNum = 0;

	rt = core.Init();
	if (SUCCESS(rt)) {
		pParamManager = static_cast<ISPParamManager*>(core.GetParamManager());
		pFileManager = static_cast<FileManager*>(core.GetFileManager());
		pListManager = static_cast<ISPListManager*>(core.GetListManager());
		pBufferManager = static_cast<MemoryPool<uchar>*>(core.GetBufferManager());
		if (!pParamManager || !pFileManager || !pListManager || !pBufferManager) {
			rt = ISP_FAILED;
			ILOGE("ParamManager:%p FileManager:%p ListManager:%p BufferManager:%p",
					pParamManager, pFileManager, pListManager, pBufferManager);
		} else {
			ioInfo.argc = argc;
			for (int32_t i = 0; i < ioInfo.argc; i++) {
				ioInfo.argv[i] = argv[i];
				ioInfo.envp[i] = envp[i];
			}
			rt = (ISPResult)pFileManager->Input(ioInfo);
			if (rt == ISP_SKIP) {
				return 0;
			}
			SetCore((void*)& core);
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
		rawData		= static_cast<void*>(pBufferManager->RequireBuffer(numPixel * 2));
		bgrData		= static_cast<void*>(pBufferManager->RequireBuffer(numPixel * 3 * 2));
		yuvData		= static_cast<void*>(pBufferManager->RequireBuffer(numPixel * 3));
		dst			= Mat(mediaInfo.img.height, mediaInfo.img.width, CV_8UC3, Scalar(0, 0, 0));
		if (mipiRawData && rawData && bgrData && yuvData && !dst.empty()) {
			memset((uint8_t*)mipiRawData, 0x0, bufferSize);
			memset((uint16_t*)rawData, 0x0, numPixel);
			memset((uint16_t*)bgrData, 0x0, numPixel * 3);
			memset((uint8_t*)yuvData, 0x0, numPixel * 3);
			ILOGDC("buffer addr(%p %p %p %p %p)", mipiRawData, rawData, bgrData, yuvData, dst.data);
		}
		else {
			rt = ISP_MEMORY_ERROR;
			ILOGE("Failed to alloc buffers!");
		}
	}

	if (SUCCESS(rt)) {
		inputInfo.type = INPUT_FILE_TYPE_RAW;
		outputInfo.imgInfo.type = OUTPUT_FILE_TYPE_BMP;
		rt = pParamManager->GetImgDimension(&outputInfo.imgInfo.width, &outputInfo.imgInfo.hight);
		outputInfo.imgInfo.channels = dst.channels();
		outputInfo.videoInfo.type = OUTPUT_FILE_TYPE_AVI;
		rt = pParamManager->GetVideoFPS(&outputInfo.videoInfo.fps);
		rt = pParamManager->GetVideoFrameNum(&outputInfo.videoInfo.frameNum);
		rt = pParamManager->GetImgDimension(&outputInfo.videoInfo.width, &outputInfo.videoInfo.hight);

		rt = pFileManager->SetInputInfo(inputInfo);
		rt = pFileManager->SetOutputInfo(outputInfo);
	}

	if (SUCCESS(rt)) {
		rt = pListManager->CreateList((uint16_t*)rawData, (uint16_t*)bgrData, (uint8_t*)yuvData, dst.data, LIST_CFG_DEFAULT, &listId);
	}

	if (SUCCESS(rt)) {
		if (mediaInfo.type >= VIDEO_MEDIA && mediaInfo.type < MEDIA_TYPE_NUM) {
			pFileManager->CreateVideo(&dst);
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
			ILOGI("[%d]=========================== %d(%d) ==========================", listId, frameCount, frameNum);
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
			rt = pFileManager->SaveImgData(dst.data);
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
			int32_t showSizex, showSizey;

			showSizey = winSizey * 2 / 3;
			showSizex = showSizey * mediaInfo.img.width / mediaInfo.img.height;
			namedWindow(RESNAME, 0);
			resizeWindow(RESNAME, showSizex, showSizey);
			imshow(RESNAME, dst);
			waitKey(0); /* for the possibility of interacting with window, keep the value as 0 */
		}
	}

	if (!dst.empty()) {
		dst.release();
	}

	return 0;
}
