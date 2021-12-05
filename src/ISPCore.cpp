//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ISPCore.cpp
// @brief: ISP main function implementation.
//////////////////////////////////////////////////////////////////////////////////////

#include "ISPCore.h"
#include "FileManager.h"
#include "ParamManager.h"
#include "ISPListManager.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo.hpp"

/* For img display in window */
#ifdef LINUX_SYSTEM
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#endif

using namespace cv;

#define SOURCE "Source"
#define RESNAME "Result"
#define TEMP "Temp"

#ifdef LINUX_SYSTEM
#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/1MCC_IMG_20181229_001526_1.raw"
//#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/20210103062220_packaged_4000x3000_0.raw"
//#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/PD_4096x768.raw"
//#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/20210103062220_package_4000x3000_0.raw"
//#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/4000_3000_unpackaged_GRBG.raw"
//#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/4000_3000_unpackaged_MSB_GRBG.raw"
#define IMG_OUTPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/out/img_output.bmp"
#define VIDEO_OUTPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/out/video_output.avi"
#elif defined WIN32_SYSTEM
#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\1MCC_IMG_20181229_001526_1.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\20210103062220_packaged_4000x3000_0.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\PD_4096x768.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\20210103062220_package_4000x3000_0.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\4000_3000_unpackaged_GRBG.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\4000_3000_unpackaged_MSB_GRBG.raw"
#define IMG_OUTPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\img_output.bmp"
#define VIDEO_OUTPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\out\\video_output.avi"
#endif

ISPResult Mipi10decode(void* src, void* dst, IMG_INFO* info);

enum MEDIA_TYPES {
	IMAGE_MEDIA = 0,
	VIDEO_MEDIA,
	IMAGE_AND_VIDEO_MEDIA,
	MEDIA_TYPE_NUM
};

int main() {
	ISPResult result = ISP_SUCCESS;

	std::shared_ptr<ISPParamManager> pParamManager;
	std::shared_ptr<FileManager> pFileManager;
	std::unique_ptr<ISPListManager> pISPListMgr;
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
	IMG_INFO imgInfo = { 0 };
	int32_t listId = 0;
	int32_t frameNum = 0;
	int32_t fps = 0;
	MEDIA_TYPES mediaType = IMAGE_MEDIA;
//	MEDIA_TYPES mediaType = IMAGE_AND_VIDEO_MEDIA;

	/* Set raw info here */
	imgInfo.width		= 1920;
	imgInfo.height		= 1080;
	imgInfo.bitspp		= 10;
	imgInfo.stride		= 0;
	imgInfo.rawFormat	= ANDROID_RAW10;
	imgInfo.bayerOrder	= BGGR;

	if (SUCCESS(result)) {
		if (mediaType == IMAGE_MEDIA) {
			frameNum = 1;
		}
		else if (mediaType >= VIDEO_MEDIA && mediaType < MEDIA_TYPE_NUM) {
			/* TODO: method of video param configuration */
			frameNum = 30;
			fps = 30;
		}
	}

	if (SUCCESS(result)) {
		if (!pParamManager) {
			pParamManager = std::make_shared<ISPParamManager>();
		}
		result = pParamManager->SetIMGInfo(&imgInfo);
	}

	if (SUCCESS(result)) {
		numPixel = imgInfo.width * imgInfo.height;
		alignedW = ALIGNx(imgInfo.width, imgInfo.bitspp, CHECK_PACKAGED(imgInfo.rawFormat), imgInfo.stride);
		bufferSize = alignedW * imgInfo.height;
		ISPLogi("Image size:%dx%d pixelNum:%d", imgInfo.width, imgInfo.height, numPixel);
		ISPLogi("Align (%d,%d) bufferSize:%d", alignedW, imgInfo.height, bufferSize);

		mipiRawData = std::shared_ptr<uint8_t>(new uint8_t[bufferSize], [] (uint8_t *p) { delete[] p; });
		rawData		= std::shared_ptr<uint16_t>(new uint16_t[numPixel], [] (uint16_t *p) { delete[] p; });
		bgrData		= std::shared_ptr<uint16_t>(new uint16_t[numPixel * 3], [] (uint16_t *p) { delete[] p; });
		yuvData		= std::shared_ptr<uint8_t>(new uint8_t[numPixel * 3], [] (uint8_t *p) { delete[] p; });
		dst			= Mat(imgInfo.height, imgInfo.width, CV_8UC3, Scalar(0, 0, 0));
		if (mipiRawData && rawData && bgrData && yuvData && !dst.empty()) {
			memset(mipiRawData.get(), 0x0, bufferSize);
			memset(rawData.get(), 0x0, numPixel);
			memset(bgrData.get(), 0x0, numPixel * 3);
			memset(yuvData.get(), 0x0, numPixel * 3);
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("Failed to alloc buffers!");
		}
	}

	if (SUCCESS(result)) {
		if (!pFileManager) {
			pFileManager = std::make_shared<FileManager>();
		}
		result = pFileManager->Init();
		if (SUCCESS(result)) {
			strcpy(inputInfo.path, INPUT_PATH);
			inputInfo.type = INPUT_FILE_TYPE_RAW;
			result = pFileManager->SetInputInfo(inputInfo);
		}
		if (SUCCESS(result)) {
			strcpy(outputInfo.imgInfo.path, IMG_OUTPUT_PATH);
			outputInfo.imgInfo.type = OUTPUT_FILE_TYPE_BMP;
			result = pParamManager->GetIMGDimension(&outputInfo.imgInfo.width, &outputInfo.imgInfo.hight);
			outputInfo.imgInfo.channels = dst.channels();

			strcpy(outputInfo.videoInfo.path, VIDEO_OUTPUT_PATH);
			outputInfo.videoInfo.type = OUTPUT_FILE_TYPE_AVI;
			outputInfo.videoInfo.fps = fps;
			outputInfo.videoInfo.frameNum = frameNum;
			result = pParamManager->GetIMGDimension(&outputInfo.videoInfo.width, &outputInfo.videoInfo.hight);

			result = pFileManager->SetOutputInfo(outputInfo);
		}
	}

	if (SUCCESS(result)) {
		if (!pISPListMgr) {
			pISPListMgr = std::make_unique<ISPListManager>();
		}
		result = pISPListMgr->Init(pParamManager.get());
		if (SUCCESS(result)) {
			result = pISPListMgr->CreateList(rawData.get(), bgrData.get(), yuvData.get(), dst.data, LIST_CFG_DEFAULT, &listId);
		}
	}

	if (SUCCESS(result)) {
		if (mediaType >= VIDEO_MEDIA && mediaType < MEDIA_TYPE_NUM) {
			pFileManager->CreateVideo(&dst);
		}
	}

	if (SUCCESS(result)) {
		for (int32_t frameCount = 1; frameCount <= frameNum; frameCount++) {
			ISPLogi("=========================== %d(%d) ==========================", frameCount, frameNum);
			if (SUCCESS(result)) {
				result = pFileManager->ReadData(mipiRawData.get(), bufferSize);
			}

			if (SUCCESS(result)) {
				result = Mipi10decode((void*)mipiRawData.get(), (void*)rawData.get(), &imgInfo);
			}

			if (SUCCESS(result)) {
				if (SUCCESS(result)) {
					/* ====================================================================== */
					/* |	Process steps can be switch on/off here as you wish				| */
					/* |	eg: ISPListMgr.EnableNodebyType(listId, PROCESS_CC);			| */
					/* |	eg: ISPListMgr.DisableNodebyType(listId, PROCESS_CST_RAW2RGB);	| */
					/* ====================================================================== */
				}

				if (SUCCESS(result)) {
					result = pISPListMgr->StartById(listId);
					if (!SUCCESS(result)) {
						ISPLoge("Failed to start list:%d %d", listId, result);
					}
				}

			}

			if (SUCCESS(result)) {
				if (mediaType >= VIDEO_MEDIA && mediaType < MEDIA_TYPE_NUM) {
					pFileManager->SaveVideoData(frameCount);
				}
			}
		}
	}

	if (SUCCESS(result)) {
		if (mediaType == IMAGE_MEDIA || mediaType == IMAGE_AND_VIDEO_MEDIA) {
			result = pFileManager->SaveImgData(dst.data);
		}
	}

	if (SUCCESS(result)) {
		if (mediaType >= VIDEO_MEDIA && mediaType < MEDIA_TYPE_NUM) {
			result = pFileManager->DestroyVideo();
		}
	}

	if (SUCCESS(result)) {
		result = pISPListMgr->DestoryAllList();
	}

	if (SUCCESS(result)) {
		/* Show the result */
		int32_t winSizey;
		bool supportWin = false;
#ifdef LINUX_SYSTEM
		winsize winSize;
		ioctl(STDIN_FILENO, TIOCGWINSZ, &winSize);
		if (!winSize.ws_row) {
			ISPLogw("Cannot get terminal size(%d,%d)! Skip img show", winSize.ws_col, winSize.ws_row);
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
			showSizex = showSizey * imgInfo.width / imgInfo.height;
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

ISPResult Mipi10decode(void* src, void* dst, IMG_INFO* info)
{
	ISPResult result = ISP_SUCCESS;

	int32_t leftShift = 0;
	int32_t alignedW = ALIGNx(info->width, info->bitspp, CHECK_PACKAGED(info->rawFormat), info->stride);
	if (!info) {
		result = ISP_INVALID_PARAM;
	}

	if (SUCCESS(result)) {
		leftShift = info->bitspp - BITS_PER_WORD;
		if (leftShift < 0 || leftShift > 8)
		{
			result = ISP_INVALID_PARAM;
		}
	}

	if (SUCCESS(result)) {
		switch (info->rawFormat) {
			case ANDROID_RAW10:
				for (int32_t row = 0; row < info->height; row++) {
					for (int32_t col = 0; col < alignedW; col += 5) {
						if (col * BITS_PER_WORD / info->bitspp < info->width && row < info->height) {
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp] =
								((static_cast<uint8_t*>(src)[row * alignedW + col] & 0xffff) << leftShift) |
								((static_cast<uint8_t*>(src)[row * alignedW + col + 4] & 0x3) & 0x3ff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 1] =
								((static_cast<uint8_t*>(src)[row * alignedW + col + 1] & 0xffff) << leftShift) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 4] >> 2) & 0x3) & 0x3ff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 2] =
								((static_cast<uint8_t*>(src)[row * alignedW + col + 2] & 0xffff) << leftShift) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 4] >> 4) & 0x3) & 0x3ff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 3] =
								((static_cast<uint8_t*>(src)[row * alignedW + col + 3] & 0xffff) << leftShift) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 4] >> 6) & 0x3) & 0x3ff);
						}
					}
				}
				break;
			case ORDINAL_RAW10:
				for (int32_t row = 0; row < info->height; row++) {
					for (int32_t col = 0; col < alignedW; col += 5) {
						if (col * BITS_PER_WORD / info->bitspp < info->width && row < info->height) {
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp] =
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 0] >> 0) & (0xff >> 0)) & 0xffff) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 1] & 0x03) << 8) & 0xffff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 1] =
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 1] >> 2) & (0xff >> 2)) & 0xffff) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 2] & 0x0f) << 6) & 0xffff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 2] =
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 2] >> 4) & (0xff >> 4)) & 0xffff) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 3] & 0x3f) << 4) & 0xffff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 3] =
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 3] >> 6) & (0xff >> 6)) & 0xffff) |
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 4] & 0xff) << 2) & 0xffff);
						}
					}
				}
				break;
			case UNPACKAGED_RAW10_LSB:
				for (int32_t row = 0; row < info->height; row++) {
					for (int32_t col = 0; col < alignedW; col += 2) {
						static_cast<uint16_t*>(dst)[row * info->width + col / 2] =
							(static_cast<uint8_t*>(src)[row * alignedW + col] & 0xffff) |
							(((static_cast<uint8_t*>(src)[row * alignedW + col + 1] & 0x3) & 0xffff) << BITS_PER_WORD);
					}
				}
			case UNPACKAGED_RAW10_MSB:
				for (int32_t row = 0; row < info->height; row++) {
					for (int32_t col = 0; col < alignedW; col += 2) {
						static_cast<uint16_t*>(dst)[row * info->width + col / 2] =
							((static_cast<uint8_t*>(src)[row * alignedW + col] >> (BITS_PER_WORD - leftShift)) & 0xffff) |
							((static_cast<uint8_t*>(src)[row * alignedW + col + 1] & 0xffff) << leftShift);
					}
				}
				break;
			case RAW_FORMAT_NUM:
			default:
				ISPLoge("Not support raw type:%d", info->rawFormat);
				break;
		}
		ISPLogi("finished");
	}

	return result;
}
