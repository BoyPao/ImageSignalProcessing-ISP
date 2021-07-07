//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ImageSignalProcess.cpp
// @brief: ISP main function implementation
//////////////////////////////////////////////////////////////////////////////////////

#include "FileManager.h"
#include "ISPCore.h"
#include "ISPList.h"
#include "ParamManager.h"

using namespace cv;

#define SOURCE "Source"
#define RESNAME "Result"
#define TEMP "Temp"

#define INPUTPATH "D:\\test_project\\ISP\\local\\ISP-Local\\ISP-Local\\1MCC_IMG_20181229_001526_1.RAW"   
#define OUTPUTPATH "D:\\test_project\\ISP\\local\\output\\output.BMP"
#define WIDTH 1920
#define HEIGHT 1080


int main() {
	ISPResult result = ISP_SUCCESS;

	ISPParamManager paramManager;
	int32_t width = WIDTH;
	int32_t height = HEIGHT;
	paramManager.SetIMGDimension(&width, &height);

	InputImgInfo inputInfo;
	OutputImgInfo outputInfo;
	string inputPath = INPUTPATH;
	string outputPath = OUTPUTPATH;
	inputInfo.pInputPath = const_cast<char*>(inputPath.c_str());
	outputInfo.pOutputPath = const_cast<char*>(outputPath.c_str());
	paramManager.GetIMGDimension(&outputInfo.width, &outputInfo.hight);
	
	ImageFileManager* pImgFileManager = nullptr;
	if (pImgFileManager == nullptr) {
		pImgFileManager = new ImageFileManager;
	}
	pImgFileManager->Init();
	pImgFileManager->SetInputImgInfo(inputInfo);
	pImgFileManager->SetOutputImgInfo(outputInfo);

	size_t numPixel = WIDTH * HEIGHT;
	uint8_t* mipiRawData = new uint8_t[numPixel * 5 / 4];
	uint16_t* rawData = new uint16_t[numPixel];
	uint16_t* bgrData = new uint16_t[numPixel * 3];
	uint16_t* bData = bgrData;
	uint16_t* gData = bData + numPixel;
	uint16_t* rData = gData + numPixel;
	memset(bgrData, 0x0, numPixel * 3);
	Mat dst(HEIGHT, WIDTH, CV_8UC3, Scalar(0, 0, 0));
	int32_t i, j;

	result = pImgFileManager->ReadRawData(mipiRawData, numPixel * 5 / 4, Mipi10Bit);
	if (SUCCESS(result)) {
		//Mipi10 decode
		result = Mipi10decode((void*)mipiRawData, (void*)rawData,pImgFileManager->GetInputImgInfo().rawSize);

		ISPList<uint16_t, uint16_t, uint8_t>* pIspList = new ISPList<uint16_t, uint16_t, uint8_t>;
		pIspList->Init(rawData, bgrData, dst.data, &paramManager);
		pIspList->CreatISPList();
		pIspList->Process();

		//Save the result
		cvtColor(dst, dst, COLOR_YCrCb2BGR, 0);
		result = pImgFileManager->SaveBMP(dst.data, dst.channels());
		delete pImgFileManager;
		delete[] mipiRawData;
		delete[] rawData;
		delete[] bgrData;
		delete pIspList;

		int32_t Imgsizex, Imgsizey;
		int32_t Winsizex, Winsizey;
		Winsizex = GetSystemMetrics(SM_CXSCREEN);
		Winsizey = GetSystemMetrics(SM_CYSCREEN);
		Imgsizey = Winsizey * 2 / 3;
		Imgsizex = Imgsizey * WIDTH / HEIGHT;
		namedWindow(RESNAME, 0);
		resizeWindow(RESNAME, Imgsizex, Imgsizey);
		imshow(RESNAME, dst);
		waitKey(0);
	}
	scanf_s("%d", &i);

	return 0;
}


ISPResult Mipi10decode(void* src, void* dst, int32_t rawSize) {
	ISPResult result = ISP_SUCCESS;

	for (int32_t i = 0; i < rawSize; i += 5) {
		static_cast<uint16_t*>(dst)[i * 4 / 5] =
			((static_cast<uint8_t*>(src)[i] & 0xffff) << 2) |
			(static_cast<uint8_t*>(src)[i + 4] & 0x3) & 0x3ff;
		static_cast<uint16_t*>(dst)[i * 4 / 5 + 1] =
			((static_cast<uint8_t*>(src)[i + 1] & 0xffff) << 2) |
			((static_cast<uint8_t*>(src)[i + 4] >> 2) & 0x3) & 0x3ff;
		static_cast<uint16_t*>(dst)[i * 4 / 5 + 2] =
			((static_cast<uint8_t*>(src)[i + 2] & 0xffff) << 2) |
			((static_cast<uint8_t*>(src)[i + 4] >> 4) & 0x3) & 0x3ff;
		static_cast<uint16_t*>(dst)[i * 4 / 5 + 3] =
			((static_cast<uint8_t*>(src)[i + 3] & 0xffff) << 2) |
			((static_cast<uint8_t*>(src)[i + 4] >> 6) & 0x3) & 0x3ff;
	}
	ISPLogi("finished");
	return result;
}