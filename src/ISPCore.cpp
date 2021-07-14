//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ISPCore.cpp
// @brief: ISP main function implementation
//////////////////////////////////////////////////////////////////////////////////////

#include "ISPCore.h"
#include "ISPList.h"
#include "FileManager.h"
#include "ParamManager.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo.hpp"

using namespace cv;

#define SOURCE "Source"
#define RESNAME "Result"
#define TEMP "Temp"

//#define INPUTPATH "D:\\test_project\\ISP\\local\\ISP-Local\\ISP-Local\\1MCC_IMG_20181229_001526_1.RAW"   
#define INPUTPATH "D:\\test_project\\ISP\\local\\ISP-Local\\ISP-Local\\20210103062220_input_4000x3000_0.raw"
#define OUTPUTPATH "D:\\test_project\\ISP\\local\\output\\output.BMP"

ISPResult Mipi10decode(void* src, void* dst, IMG_INFO* info);

int main() {
	ISPResult result = ISP_SUCCESS;

	ISPParamManager paramManager;
	IMG_INFO imgInfo = { 0 };
	imgInfo.width		= 4000;
	imgInfo.height		= 3000;
	imgInfo.bitspp		= 10;
	imgInfo.stride		= 16;
	imgInfo.packaged	= true;
	imgInfo.rawType		= RAW10_MIPI_RGGB;
	paramManager.SetIMGInfo(&imgInfo);

	InputImgInfo inputInfo;
	OutputImgInfo outputInfo;
	char inputPath[FILE_PATH_MAX_SIZE] = { '\0' };
	char outputPath[FILE_PATH_MAX_SIZE] = { '\0' };
	strcpy_s(inputPath, INPUTPATH);
	strcpy_s(outputPath, OUTPUTPATH);
	inputInfo.pInputPath = inputPath;
	outputInfo.pOutputPath = outputPath;
	paramManager.GetIMGDimension(&outputInfo.width, &outputInfo.hight);
	
	ImageFileManager* pImgFileManager = nullptr;
	if (pImgFileManager == nullptr) {
		pImgFileManager = new ImageFileManager;
	}
	pImgFileManager->Init();
	pImgFileManager->SetInputImgInfo(inputInfo);
	pImgFileManager->SetOutputImgInfo(outputInfo);

	int32_t numPixel = imgInfo.width * imgInfo.height;
	int32_t alignedW = ALIGNx(imgInfo.width, imgInfo.bitspp, imgInfo.packaged, imgInfo.stride);
	int32_t bufferSize = alignedW * imgInfo.height;
	ISPLogi("(%d,%d) pixelNum:%d", imgInfo.width, imgInfo.height, numPixel);
	ISPLogi("Align (%d,%d) bufferSize:%d", alignedW, imgInfo.height, bufferSize);

	uint8_t* mipiRawData = new uint8_t[bufferSize];
	uint16_t* rawData = new uint16_t[numPixel];
	uint16_t* bgrData = new uint16_t[numPixel * 3];
	uint16_t* bData = bgrData;
	uint16_t* gData = bData + numPixel;
	uint16_t* rData = gData + numPixel;
	memset(bgrData, 0x0, numPixel * 3);
	Mat dst(imgInfo.height, imgInfo.width, CV_8UC3, Scalar(0, 0, 0));
	int32_t i, j;
	result = pImgFileManager->ReadRawData(mipiRawData, bufferSize, Mipi10Bit);
	if (SUCCESS(result)) {
		//Mipi10 decode
		//result = Mipi10decode((void*)mipiRawData, (void*)rawData, pImgFileManager->GetInputImgInfo().rawSize);
		result = Mipi10decode((void*)mipiRawData, (void*)rawData, &imgInfo);

		if (SUCCESS(result)) {
			ISPList<uint16_t, uint16_t, uint8_t>* pIspList = new ISPList<uint16_t, uint16_t, uint8_t>;
			pIspList->Init(rawData, bgrData, dst.data, &paramManager);
			pIspList->CreatISPList();
			pIspList->Process();

			//Save the result
			cvtColor(dst, dst, COLOR_YCrCb2BGR, 0);
			delete pIspList;
		}

		result = pImgFileManager->SaveBMP(dst.data, dst.channels());
		delete pImgFileManager;
		delete[] mipiRawData;
		delete[] rawData;
		delete[] bgrData;

		int32_t Imgsizex, Imgsizey;
		int32_t Winsizex, Winsizey;
		Winsizex = GetSystemMetrics(SM_CXSCREEN);
		Winsizey = GetSystemMetrics(SM_CYSCREEN);
		Imgsizey = Winsizey * 2 / 3;
		Imgsizex = Imgsizey * imgInfo.width / imgInfo.height;
		namedWindow(RESNAME, 0);
		resizeWindow(RESNAME, Imgsizex, Imgsizey);
		imshow(RESNAME, dst);
		waitKey(0);
	}
	scanf_s("%d", &i);

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
						(static_cast<uint8_t*>(src)[row * alignedW + col + 4] & 0x3) & 0x3ff;
					static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 1] =
						((static_cast<uint8_t*>(src)[row * alignedW + col + 1] & 0xffff) << leftShift) |
						((static_cast<uint8_t*>(src)[row * alignedW + col + 4] >> 2) & 0x3) & 0x3ff;
					static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 2] =
						((static_cast<uint8_t*>(src)[row * alignedW + col + 2] & 0xffff) << leftShift) |
						((static_cast<uint8_t*>(src)[row * alignedW + col + 4] >> 4) & 0x3) & 0x3ff;
					static_cast<uint16_t*>(dst)[row * info->width + col * BITS_PER_WORD / info->bitspp + 3] =
						((static_cast<uint8_t*>(src)[row * alignedW + col + 3] & 0xffff) << leftShift) |
						((static_cast<uint8_t*>(src)[row * alignedW + col + 4] >> 6) & 0x3) & 0x3ff;
				}
			}
		}
		ISPLogi("finished");
	}

	return result;
}