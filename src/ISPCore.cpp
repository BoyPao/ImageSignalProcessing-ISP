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
#include <pthread.h>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo.hpp"

using namespace cv;

#define SOURCE "Source"
#define RESNAME "Result"
#define TEMP "Temp"

#define INPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/1MCC_IMG_20181229_001526_1.raw"
//#define INPUT_PATH "D:\\test_project\\ISP\\local\\ISP-Local\\ISP-Local\\20210103062220_input_4000x3000_0.raw"
#define OUTPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/out/output.bmp"
#define VIDEO_OUTPUT_PATH "/home2/penghao/test_porject/ISP/ISP/res/out/video_output.avi"

ISPResult Mipi10decode(void* src, void* dst, IMG_INFO* info);
void* VideoEncodingFunc(void* args);

enum MEDIA_TYPE {
	IMAGE_MEDIA = 0,
	VIDEO_MEDIA,
	IMAGE_AND_VIDEO_MEDIA,
	MEDIA_TYPE_NUM
};

pthread_cond_t gCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;
int32_t* pFrameNum = nullptr;
int32_t* pFPS = nullptr;
Mat* pDst = nullptr;

int main() {
	ISPResult result = ISP_SUCCESS;

	ISPParamManager* pParamManager = nullptr;
	ImageFileManager* pImgFileManager = nullptr;
	ISPListManager* pISPListMgr = nullptr;
	int32_t numPixel = 0;
	int32_t alignedW = 0;
	int32_t bufferSize = 0;
	uint8_t* mipiRawData = nullptr;
	uint16_t* rawData = nullptr;
	uint16_t* bgrData = nullptr;
	uint8_t* yuvData = nullptr;
	Mat dst;
	InputImgInfo inputInfo;
	OutputImgInfo outputInfo;
	char inputPath[FILE_PATH_MAX_SIZE] = { '\0' };
	char outputPath[FILE_PATH_MAX_SIZE] = { '\0' };
	IMG_INFO imgInfo = { 0 };
	int32_t listId = 0;
	int32_t frameNum = 0;
	int32_t fps = 0;
	pthread_t videoThread;
	//MEDIA_TYPE mediaType = IMAGE_MEDIA;
	MEDIA_TYPE mediaType = IMAGE_AND_VIDEO_MEDIA;

	//Set raw info
	imgInfo.width = 1920;
	imgInfo.height = 1080;
	imgInfo.bitspp = 10;
	imgInfo.stride = 0;
	imgInfo.packaged = true;
	imgInfo.rawType = RAW10_MIPI_BGGR;

	if (!pParamManager) {
		pParamManager = new ISPParamManager;
	}
	result = pParamManager->SetIMGInfo(&imgInfo);

	if (SUCCESS(result)) {
		strcpy(inputPath, INPUT_PATH);
		strcpy(outputPath, OUTPUT_PATH);
		inputInfo.pInputPath = inputPath;
		outputInfo.pOutputPath = outputPath;
		result = pParamManager->GetIMGDimension(&outputInfo.width, &outputInfo.hight);

		if (!pImgFileManager) {
			pImgFileManager = new ImageFileManager;
		}
		pImgFileManager->Init();
		result = pImgFileManager->SetInputImgInfo(inputInfo);
		result = pImgFileManager->SetOutputImgInfo(outputInfo);
	}

	if (SUCCESS(result)) {
		numPixel = imgInfo.width * imgInfo.height;
		alignedW = ALIGNx(imgInfo.width, imgInfo.bitspp, imgInfo.packaged, imgInfo.stride);
		bufferSize = alignedW * imgInfo.height;
		ISPLogi("(%d,%d) pixelNum:%d", imgInfo.width, imgInfo.height, numPixel);
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
		if (mediaType == IMAGE_MEDIA) {
			frameNum = 1;
		}
		else if (mediaType >= VIDEO_MEDIA && mediaType < MEDIA_TYPE_NUM) {
			if (!pthread_mutex_init(&gMutex, NULL)) {
				if (pthread_cond_init(&gCond, NULL)) {
					result = ISP_FAILED;
				}
			}
			else {
				result = ISP_FAILED;
			}

			if (SUCCESS(result)) {
				if (pthread_create(&videoThread, NULL, VideoEncodingFunc, NULL)) {
					result = ISP_FAILED;
				}
			}

			if (SUCCESS(result)) {
				/* TODO: method of video param configuration */
				frameNum = 90;
				fps = 30;
				pFrameNum = & frameNum;
				pFPS = &fps;
				pDst = &dst;
			}
		}
	}

	if (SUCCESS(result)) {
		for (int32_t frameCount = 1; frameCount <= frameNum; frameCount++) {
			ISPLogd("=========================== %d(%d) ==========================", frameCount, frameNum);
			if (SUCCESS(result)) {
				result = pImgFileManager->ReadRawData(mipiRawData, bufferSize, Mipi10Bit);
			}

			if (SUCCESS(result)) {
				result = Mipi10decode((void*)mipiRawData, (void*)rawData, &imgInfo);
			}

			if (SUCCESS(result)) {
				if (SUCCESS(result)) {
					//===================================================================
					//|	Process steps can be switch on/off here as you wish				|
					//|	eg: ISPListMgr.EnableNodebyType(listId, PROCESS_CC);			|
					//|	eg: ISPListMgr.DisableNodebyType(listId, PROCESS_CST_RAW2RGB);	|
					//===================================================================
				}

				if (SUCCESS(result)) {
					result = pISPListMgr->StartById(listId);
					if (!SUCCESS(result)) {
						ISPLoge("Failed to start list:%d %d", listId, result);
					}
				}

			}

			if (mediaType >= VIDEO_MEDIA && mediaType < MEDIA_TYPE_NUM) {
				pthread_mutex_lock(&gMutex);
				pthread_cond_signal(&gCond);
				ISPLogd("signal F:%d", frameCount);
				pthread_mutex_unlock(&gMutex);

			}
		}
	}

	if (SUCCESS(result)) {
		if (mediaType >= VIDEO_MEDIA && mediaType < MEDIA_TYPE_NUM) {
			if(pthread_join(videoThread, NULL)) {
				ISPLoge("join video thread failed!");
			}
			pthread_mutex_destroy(&gMutex);
			pthread_cond_destroy(&gCond);
		}
	}

	if (SUCCESS(result)) {
		if (mediaType == IMAGE_MEDIA || mediaType == IMAGE_AND_VIDEO_MEDIA) {
			result = pImgFileManager->SaveBMP(dst.data, dst.channels());
		}
	}

	if (SUCCESS(result)) {
		result = pISPListMgr->DestoryAllList();
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
		//Show the result
		/*
		int32_t Imgsizex, Imgsizey;
		int32_t Winsizex, Winsizey;
		Winsizex = GetSystemMetrics(SM_CXSCREEN);
		Winsizey = GetSystemMetrics(SM_CYSCREEN);
		Imgsizey = Winsizey * 2 / 3;
		Imgsizex = Imgsizey * imgInfo.width / imgInfo.height;
		namedWindow(RESNAME, 0);
		resizeWindow(RESNAME, Imgsizex, Imgsizey);
		imshow(RESNAME, dst);
		waitKey(0); //for the possibility of interacting with window, keep the value as 0.
		*/
	}

	if (!dst.empty()) {
		dst.release();
	}

	//For windows not exit CMD
	/*
	int32_t i;
	scanf("%d", &i);
	*/
	return 0;
}

ISPResult Mipi10decode(void* src, void* dst, IMG_INFO* info)
{
	ISPResult result = ISP_SUCCESS;

	int32_t leftShift = 0;
	int32_t alignedW = ALIGNx(info->width, info->bitspp, info->packaged, info->stride);
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
		ISPLogi("finished");
	}

	return result;
}

void* VideoEncodingFunc(void* args)
{
	if (pFrameNum && pFPS && pDst) {
		VideoWriter vWriter(VIDEO_OUTPUT_PATH, VideoWriter::fourcc('M', 'J', 'P', 'G'), *pFPS, Size(pDst->cols, pDst->rows));
		if (vWriter.isOpened()) {
			for (int32_t frameCount = 1; frameCount <= *pFrameNum; frameCount++) {
				pthread_mutex_lock(&gMutex);
				pthread_cond_wait(&gCond, &gMutex);
				vWriter << *pDst;
				ISPLogi("Recording F:%d (%ds)", frameCount, frameCount / *pFPS);
				if (frameCount == *pFrameNum) {
					ISPLogi("Record finish");
				}
				pthread_mutex_unlock(&gMutex);
			}
		}
		else {
			ISPLoge("Failed to initialize VideoWriter!");
		}
	}
	else {
		ISPLoge("Video func not init!");
	}

	return args;
}
