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
#include "ISPVideo.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo.hpp"

/* For img display in window */
#ifdef LINUX_SYSTEM
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#endif

using namespace cv;

#define SOURCE "Source"
#define RESNAME "Result"
#define TEMP "Temp"

#ifdef LINUX_SYSTEM
//#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/1MCC_IMG_20181229_001526_1.raw"
//#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/20210103062220_input_4000x3000_0.raw"
//#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/PD_4096x768.raw"
//#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/20210103062220_package_4000x3000_0.raw"
#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/4000_3000_unpackaged_GRBG.raw"
#define IMG_OUTPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/out/img_output.bmp"
#define VIDEO_OUTPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/out/video_output.avi"
#elif defined WIN32_SYSTEM
#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\1MCC_IMG_20181229_001526_1.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\20210103062220_input_4000x3000_0.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\PD_4096x768.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\20210103062220_package_4000x3000_0.raw"
//#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\4000_3000_unpackaged.raw"
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

	ISPParamManager* pParamManager = nullptr;
	ImageFileManager* pImgFileManager = nullptr;
	ISPListManager* pISPListMgr = nullptr;
	ISPVideo* pVideo = nullptr;
	int32_t numPixel = 0;
	int32_t alignedW = 0;
	int32_t bufferSize = 0;
	uint8_t* mipiRawData = nullptr;
	uint16_t* rawData = nullptr;
	uint16_t* bgrData = nullptr;
	uint8_t* yuvData = nullptr;
	Mat dst;
	InputImgInfo inputInfo;
	OutputImgInfo imgOutputPath;
	OutputVideoInfo videoOutputInfo;
	char inputPath[FILE_PATH_MAX_SIZE] = { '\0' };
	char outputPath[FILE_PATH_MAX_SIZE] = { '\0' };
	char videoOutputPath[FILE_PATH_MAX_SIZE] = { '\0' };
	IMG_INFO imgInfo = { 0 };
	int32_t listId = 0;
	int32_t frameNum = 0;
	int32_t fps = 0;
	MEDIA_TYPES mediaType = IMAGE_MEDIA;
//	MEDIA_TYPES mediaType = IMAGE_AND_VIDEO_MEDIA;

	/* Set raw info here */
	imgInfo.width		= 4000;
	imgInfo.height		= 3000;
	imgInfo.bitspp		= 10;
	imgInfo.stride		= 16;
	imgInfo.rawFormat	= UNPACKAGED_RAW10_LSB;
	imgInfo.bayerOrder	= GBRG;

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
			pParamManager = new ISPParamManager;
		}
		result = pParamManager->SetIMGInfo(&imgInfo);
	}

	if (SUCCESS(result)) {
		strcpy(inputPath, INPUT_PATH);
		strcpy(outputPath, IMG_OUTPUT_PATH);
		strcpy(videoOutputPath, VIDEO_OUTPUT_PATH);
		inputInfo.pInputPath = inputPath;
		imgOutputPath.pOutputPath = outputPath;
		videoOutputInfo.pOutputPath = videoOutputPath;
		videoOutputInfo.fps = fps;
		videoOutputInfo.frameNum = frameNum;

		result = pParamManager->GetIMGDimension(&imgOutputPath.width, &imgOutputPath.hight);
		result = pParamManager->GetIMGDimension(&videoOutputInfo.width, &videoOutputInfo.hight);

		if (!pImgFileManager) {
			pImgFileManager = new ImageFileManager;
		}
		result = pImgFileManager->Init();
		if (SUCCESS(result)) {
			result = pImgFileManager->SetInputImgInfo(inputInfo);
			result = pImgFileManager->SetOutputImgInfo(imgOutputPath);
			result = pImgFileManager->SetOutputVideoInfo(videoOutputInfo);
		}
	}

	if (SUCCESS(result)) {
		numPixel = imgInfo.width * imgInfo.height;
		alignedW = ALIGNx(imgInfo.width, imgInfo.bitspp, CHECK_PACKAGED(imgInfo.rawFormat), imgInfo.stride);
		bufferSize = alignedW * imgInfo.height;
		ISPLogi("Image size:%dx%d pixelNum:%d", imgInfo.width, imgInfo.height, numPixel);
		ISPLogi("Align (%d,%d) bufferSize:%d", alignedW, imgInfo.height, bufferSize);

		mipiRawData = new uint8_t[bufferSize];
		rawData = new uint16_t[numPixel];
		bgrData = new uint16_t[numPixel * 3];
		yuvData = new uint8_t[numPixel * 3];
		dst = Mat(imgInfo.height, imgInfo.width, CV_8UC3, Scalar(0, 0, 0));
		if (mipiRawData && rawData && bgrData && yuvData && !dst.empty()) {
			memset(mipiRawData, 0x0, bufferSize);
			memset(rawData, 0x0, numPixel);
			memset(bgrData, 0x0, numPixel * 3);
			memset(yuvData, 0x0, numPixel * 3);
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("Failed to alloc buffers!");
		}
	}

	if (SUCCESS(result)) {
		if (!pISPListMgr) {
			pISPListMgr = new ISPListManager;
		}
		result = pISPListMgr->Init(pParamManager);
		if (SUCCESS(result)) {
			result = pISPListMgr->CreateList(rawData, bgrData, yuvData, dst.data, LIST_CFG_DEFAULT, &listId);
		}
	}

	if (SUCCESS(result)) {
		if (!pVideo) {
			pVideo = new ISPVideo;
		}
		if (mediaType >= VIDEO_MEDIA && mediaType < MEDIA_TYPE_NUM) {
			result = pVideo->Init(&dst, pImgFileManager, pParamManager);
			if (SUCCESS(result)) {
				result = pVideo->CreateThread();
			}
			ISPLogi("Video fps:%d total frame:%d", fps, frameNum);
		}
	}

	if (SUCCESS(result)) {
		for (int32_t frameCount = 1; frameCount <= frameNum; frameCount++) {
			ISPLogi("=========================== %d(%d) ==========================", frameCount, frameNum);
			if (SUCCESS(result)) {
				result = pImgFileManager->ReadRawData(mipiRawData, bufferSize);
			}

			if (SUCCESS(result)) {
				result = Mipi10decode((void*)mipiRawData, (void*)rawData, &imgInfo);
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
					pVideo->Notify();
					ISPLogd("Notify F:%d", frameCount);
				}
			}
		}
	}

	if (SUCCESS(result)) {
		if (mediaType == IMAGE_MEDIA || mediaType == IMAGE_AND_VIDEO_MEDIA) {
			result = pImgFileManager->SaveBMP(dst.data, dst.channels());
		}
	}

	if (SUCCESS(result)) {
		if (mediaType >= VIDEO_MEDIA && mediaType < MEDIA_TYPE_NUM) {
			result = pVideo->DestroyThread();
		}

	}

	if (SUCCESS(result)) {
		result = pISPListMgr->DestoryAllList();
	}

	if (pVideo) {
		delete pVideo;
	}

	if (pISPListMgr) {
		delete pISPListMgr;
	}

	if (pParamManager) {
		delete pParamManager;
	}

	if (pImgFileManager) {
		delete pImgFileManager;
	}

	if (mipiRawData) {
		delete[] mipiRawData;
	}

	if (rawData) {
		delete[] rawData;
	}

	if (bgrData) {
		delete[] bgrData;
	}

	if (yuvData) {
		delete[] yuvData;
	}

	if (SUCCESS(result)) {
		/* Show the result */
		int32_t winSizey;
		bool supportWin = false;
#ifdef LINUX_SYSTEM
		int fd;
		struct fb_var_screeninfo fbVar;
		fd = open("/dev/fb0", O_RDWR);
		if (fd < 0) {
			ISPLogw("Cannot open screen drv! Skip img show");
		}
		else {
			ioctl(fd, FBIOGET_VSCREENINFO, &fbVar);
			winSizey = fbVar.yres;
			close(fd);
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
								((((static_cast<uint8_t*>(src)[row * alignedW + col + 1] & 0x03) & 0xffff) << 8) & 0xffff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 1] =
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 1] >> 2) & (0xff >> 2)) & 0xffff) |
								((((static_cast<uint8_t*>(src)[row * alignedW + col + 2] & 0x0f) & 0xffff) << 6) & 0xffff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 2] =
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 2] >> 4) & (0xff >> 4)) & 0xffff) |
								((((static_cast<uint8_t*>(src)[row * alignedW + col + 3] & 0x3f) & 0xffff) << 4) & 0xffff);
							static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 3] =
								(((static_cast<uint8_t*>(src)[row * alignedW + col + 3] >> 6) & (0xff >> 6)) & 0xffff) |
								((((static_cast<uint8_t*>(src)[row * alignedW + col + 4] & 0xff) & 0xffff) << 2) & 0xffff);
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
//				for (int32_t row = 0; row < info->height; row++) {
//					for (int32_t col = 0; col < alignedW; col += 2) {
//						static_cast<uint16_t*>(dst)[row * info->width + col / 2] =
//							((static_cast<uint8_t*>(src)[row * alignedW + col] >> (BITS_PER_WORD - leftShift)) & 0xffff) |
//							((static_cast<uint8_t*>(src)[row * alignedW + col + 1] & 0xffff) << leftShift);
//					}
//				}
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
