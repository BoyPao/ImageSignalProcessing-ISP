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

/* Set raw path here */
#ifdef LINUX_SYSTEM
#define INPUT_PATH "/home2/penghao/test_project/ISP/ISP/res/1MCC_IMG_20181229_001526_1.raw"
//#define INPUT_PATH "/home/penghao/HAO/test_project/ISP/ISP/res/20210103062220_packaged_4000x3000_0.raw"
//#define INPUT_PATH "/home/penghao/HAO/test_project/ISP/ISP/res/PD_4096x768.raw"
//#define INPUT_PATH "/home/penghao/HAO/test_project/ISP/ISP/res/4000_3000_unpackaged_GRBG.raw"
//#define INPUT_PATH "/home/penghao/HAO/test_project/ISP/ISP/res/4000_3000_unpackaged_MSB_GRBG.raw"
#define IMG_OUTPUT_PATH "/home2/penghao/test_project/ISP/ISP/res/out/img_output.bmp"
#define VIDEO_OUTPUT_PATH "/home2/penghao/test_project/ISP/ISP/res/out/video_output.avi"
#elif defined WIN32_SYSTEM
#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\1MCC_IMG_20181229_001526_1.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\20210103062220_packaged_4000x3000_0.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\PD_4096x768.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\4000_3000_unpackaged_GRBG.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\4000_3000_unpackaged_MSB_GRBG.raw"
#define IMG_OUTPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\img_output.bmp"
#define VIDEO_OUTPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\video_output.avi"
#endif

int main() {
	ISPResult rt = ISP_SUCCESS;

	ISPCore core;
	ISPParamManager* pParamManager = nullptr;
	FileManager* pFileManager = nullptr;
	ISPListManager* pListManager = nullptr;
	int32_t numPixel = 0;
	int32_t alignedW = 0;
	int32_t bufferSize = 0;
	std::shared_ptr<uint8_t> mipiRawData;
	std::shared_ptr<uint16_t> rawData;
	std::shared_ptr<uint16_t> bgrData;
	std::shared_ptr<uint8_t> yuvData;
	Mat dst;
	InputInfo inputInfo = { 0 };
	OutputInfo outputInfo = { 0 };
	MEDIA_INFO mediaInfo = { 0 };
	int32_t listId = 0;
	int32_t frameNum = 0;

	/* Set media type here */
	mediaInfo.type = IMAGE_MEDIA;
//	mediaInfo.type = IMAGE_AND_VIDEO_MEDIA;

	/* Set raw info here */
	mediaInfo.img.width			= 1920;
	mediaInfo.img.height		= 1080;
	mediaInfo.img.bitspp		= 10;
	mediaInfo.img.stride		= 0;
	mediaInfo.img.rawFormat		= ANDROID_RAW10;
	mediaInfo.img.bayerOrder	= BGGR;

	/* Set video info here */
	mediaInfo.video.fps			= 30;
	mediaInfo.video.frameNum	= 30;

	rt = core.Init();
	if (SUCCESS(rt)) {
		pParamManager = static_cast<ISPParamManager*>(core.GetParamManager());
		pFileManager = static_cast<FileManager*>(core.GetFileManager());
		pListManager = static_cast<ISPListManager*>(core.GetListManager());
		if (!pParamManager || !pFileManager || !pListManager) {
			rt = ISP_FAILED;
			ILOGE("ParamManager:%p FileManager:%p ListManager:%p",
					pParamManager, pFileManager, pListManager);
		}
	}

	if (SUCCESS(rt)) {
		rt = pParamManager->SetMediaInfo(&mediaInfo);
	}

	if (SUCCESS(rt)) {
		numPixel = mediaInfo.img.width * mediaInfo.img.height;
		alignedW = ALIGNx(mediaInfo.img.width, mediaInfo.img.bitspp, CHECK_PACKAGED(mediaInfo.img.rawFormat), mediaInfo.img.stride);
		bufferSize = alignedW * mediaInfo.img.height;
		ILOGI("Image size:%dx%d pixelNum:%d", mediaInfo.img.width, mediaInfo.img.height, numPixel);
		ILOGI("Align (%d,%d) bufferSize:%d", alignedW, mediaInfo.img.height, bufferSize);

		mipiRawData = std::shared_ptr<uint8_t>(new uint8_t[bufferSize], [] (uint8_t *p) { delete[] p; });
		rawData		= std::shared_ptr<uint16_t>(new uint16_t[numPixel], [] (uint16_t *p) { delete[] p; });
		bgrData		= std::shared_ptr<uint16_t>(new uint16_t[numPixel * 3], [] (uint16_t *p) { delete[] p; });
		yuvData		= std::shared_ptr<uint8_t>(new uint8_t[numPixel * 3], [] (uint8_t *p) { delete[] p; });
		dst			= Mat(mediaInfo.img.height, mediaInfo.img.width, CV_8UC3, Scalar(0, 0, 0));
		if (mipiRawData && rawData && bgrData && yuvData && !dst.empty()) {
			memset(mipiRawData.get(), 0x0, bufferSize);
			memset(rawData.get(), 0x0, numPixel);
			memset(bgrData.get(), 0x0, numPixel * 3);
			memset(yuvData.get(), 0x0, numPixel * 3);
			ILOGDC("buffer addr(%p %p %p %p %p)", mipiRawData.get(), rawData.get(), bgrData.get(), yuvData.get(), dst.data);
		}
		else {
			rt = ISP_MEMORY_ERROR;
			ILOGE("Failed to alloc buffers!");
		}
	}

	if (SUCCESS(rt)) {
		strcpy(inputInfo.path, INPUT_PATH);
		inputInfo.type = INPUT_FILE_TYPE_RAW;

		strcpy(outputInfo.imgInfo.path, IMG_OUTPUT_PATH);
		outputInfo.imgInfo.type = OUTPUT_FILE_TYPE_BMP;
		rt = pParamManager->GetImgDimension(&outputInfo.imgInfo.width, &outputInfo.imgInfo.hight);
		outputInfo.imgInfo.channels = dst.channels();

		strcpy(outputInfo.videoInfo.path, VIDEO_OUTPUT_PATH);
		outputInfo.videoInfo.type = OUTPUT_FILE_TYPE_AVI;
		rt = pParamManager->GetVideoFPS(&outputInfo.videoInfo.fps);
		rt = pParamManager->GetVideoFrameNum(&outputInfo.videoInfo.frameNum);
		rt = pParamManager->GetImgDimension(&outputInfo.videoInfo.width, &outputInfo.videoInfo.hight);

		rt = pFileManager->SetInputInfo(inputInfo);
		rt = pFileManager->SetOutputInfo(outputInfo);
	}

	if (SUCCESS(rt)) {
		rt = pListManager->CreateList(rawData.get(), bgrData.get(), yuvData.get(), dst.data, LIST_CFG_DEFAULT, &listId);
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
				rt = pFileManager->ReadData(mipiRawData.get(), bufferSize);
				if (SUCCESS(rt)) {
					rt = pFileManager->Mipi10decode((void*)mipiRawData.get(), (void*)rawData.get(), &mediaInfo.img);
				}
			}

			if (SUCCESS(rt)) {
				if (SUCCESS(rt)) {
					/* ====================================================================== */
					/* |	Process steps can be switch on/off here as you wish				| */
					/* |	eg: ISPListMgr.EnableNodebyType(listId, PROCESS_CC);			| */
					/* |	eg: ISPListMgr.DisableNodebyType(listId, PROCESS_GAMMA);		| */
					/* ====================================================================== */
				}

				if (SUCCESS(rt)) {
					rt = pListManager->StartById(listId);
					if (!SUCCESS(rt)) {
						ILOGE("Failed to start list:%d %d", listId, rt);
					}
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
			waitKey(0); //for the possibility of interacting with window, keep the value as 0.
		}
	}

	if (!dst.empty()) {
		dst.release();
	}

	return 0;
}
