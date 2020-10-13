//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ImageSignalProcess.cpp
// @brief: ISP main function implementation
//////////////////////////////////////////////////////////////////////////////////////

#include "ImageFileManager.h"
#include "ImageSignalProcess.h"
#include "Pipeline.h"
#include "Param.h"

using namespace cv;
using namespace std;

#define DUMP_NEEDED false

#define SOURCE "Source"
#define RESNAME "Result"
#define TEMP "Temp"

#define INPUTPATH "C:\\Users\\penghao6\\Desktop\\1MCC_IMG_20181229_001526_1.RAW"   
#define OUTPUTPATH "C:\\Users\\penghao6\\Desktop\\output.BMP"
#define LOGPATH "C:\\Users\\penghao6\\Desktop\\output.txt"
#define WIDTH 1920
#define HEIGHT 1080


int main() {
	ISPResult result = ISPSuccess;

	ISPParameter param;
	InputImgInfo inputInfo;
	OutputImgInfo outputInfo;
	string inputPath = INPUTPATH;
	string outputPath = OUTPUTPATH;
	inputInfo.pInputPath = const_cast<char*>(inputPath.c_str());
	outputInfo.pOutputPath = const_cast<char*>(outputPath.c_str());
	param.GetIMGDimension(&outputInfo.width, &outputInfo.hight);
	
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
	uint8_t* rgb8bit = new uint8_t[numPixel * 3];
	uint8_t* r8bit = rgb8bit;
	uint8_t* g8bit = r8bit + numPixel;
	uint8_t* b8bit = g8bit + numPixel;
	memset(rgb8bit, 0x0, numPixel * 3);
	Mat dst(HEIGHT, WIDTH, CV_8UC3, Scalar(0, 0, 0));
	int32_t i, j;

	result = pImgFileManager->ReadRawData(mipiRawData, numPixel * 5 / 4, Mipi10Bit);
	if (result == ISPSuccess) {
		int32_t Imgsizex, Imgsizey;
		int32_t Winsizex, Winsizey;
		Winsizex = GetSystemMetrics(SM_CXSCREEN);
		Winsizey = GetSystemMetrics(SM_CYSCREEN);
		Imgsizey = Winsizey * 2 / 3;
		Imgsizex = Imgsizey * WIDTH / HEIGHT;

		//Mipi10 decode
		result = Mipi10decode((void*)mipiRawData, (void*)rawData,pImgFileManager->GetInputImgInfo().rawSize);
		
		bool BLCen, LSCen, GICen, WBen, CCen, Gammaen, SWNRen, Sharpen;
		BLCen = true;
		LSCen = true;
		GICen = true;
		WBen = true;
		CCen = true;
		Gammaen = true;
		SWNRen = true;
		Sharpen = true;
		ISPNode* node = new ISPNode;
		//Bayer Space Process
		result = node->Init(BLC);
		result = node->Process((void*)rawData, /*argNum*/0);
		result = node->Init(LSC);
		result = node->Process((void*)rawData, /*argNum*/0);
		result = node->Init(GCC);
		result = node->Process((void*)rawData, /*argNum*/0);
		result = ReadChannels(rawData, bData, gData, rData);//pick up RGB  channels from Raw

#if  DUMP_NEEDED
		if (result == ISPSuccess) {
			DumpImgDataAsText((void*)gData, HEIGHT, WIDTH, sizeof(uint16_t), LOGPATH);
		}
#endif
		result = Demosaic(rawData, bData, gData, rData);

		//RGB Space Process
		result = node->Init(WB);
		result = node->Process((void*)bgrData, /*argNum*/0);
		result = node->Init(CC);
		result = node->Process((void*)bgrData, /*argNum*/0);
		result = node->Init(GAMMA);
		result = node->Process((void*)bgrData, /*argNum*/0);

		Compress10to8(bData, b8bit, numPixel, true);
		Compress10to8(gData, g8bit, numPixel, true);
		Compress10to8(rData, r8bit, numPixel, true);
		
		//Fill a BMP data use three channals data
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				dst.data[i * WIDTH * 3 + 3 * j] = (unsigned int)b8bit[i * WIDTH + j];
				dst.data[i * WIDTH * 3 + 3 * j + 1] = (unsigned int)g8bit[i * WIDTH + j];
				dst.data[i * WIDTH * 3 + 3 * j + 2] = (unsigned int)r8bit[i * WIDTH + j];
			}
		}

		Mat YUV, NRYUV;
		YUV = dst.clone();
		cvtColor(YUV, YUV, COLOR_BGR2YCrCb, 0);

		//YUV Space Process
		result = node->Init(SWNR);
		result = node->Process((void*)YUV.data, /*argNum*/2, Imgsizey, Imgsizex);
		result = node->Init(SHARPNESS);
		//result = node->Process((void*)YUV.data, /*argNum*/0);

		cvtColor(YUV, dst, COLOR_YCrCb2BGR, 0);

		//Save the result
		Mat output;
		output = dst.clone();
		result = pImgFileManager->SaveBMP(output.data, output.channels());
		delete pImgFileManager;
		delete[] mipiRawData;
		delete[] rawData;
		delete[] bgrData;
		delete[] rgb8bit;
		namedWindow(RESNAME, 0);
		resizeWindow(RESNAME, Imgsizex, Imgsizey);
		imshow(RESNAME, output);
		waitKey(0);
	}
	cin >> i;

	return 0;
}

ISPResult Demosaic(uint16_t* data, uint16_t* B, uint16_t* G, uint16_t* R) {
	ISPResult result = ISPSuccess;
	FirstPixelInsertProcess(data, B);
	TwoGPixelInsertProcess(data, G);
	LastPixelInsertProcess(data, R);
	cout << __FUNCTION__<< " finished" << endl;
	return result;
}


ISPResult Mipi10decode(void* src, void* dst, int32_t rawSize) {
	ISPResult result = ISPSuccess;

	for (int32_t i = 0; i < rawSize; i += 5) {
		static_cast<uint16_t*>(dst)[i * 4 / 5] =
			((static_cast<uint8_t*>(src)[i] & 0xffff) << 2) |
			(static_cast<uint8_t*>(src) [i + 4] &0x3) & 0x3ff;
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
	cout << __FUNCTION__ << " finished" << endl;
	return result;
}

ISPResult ReadChannels(uint16_t* data, uint16_t* B, uint16_t* G, uint16_t* R) {
	ISPResult result = ISPSuccess;
	int32_t i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i % 2 == 0 && j % 2 == 0) {
				B[i * WIDTH + j] = data[i * WIDTH + j];
				G[i * WIDTH + j] = 0;
				R[i * WIDTH + j] = 0;
			}
			else if ((i % 2 == 0 && j % 2 == 1) || (i % 2 == 1 && j % 2 == 0)) {
				B[i * WIDTH + j] = 0;
				G[i * WIDTH + j] = data[i * WIDTH + j];
				R[i * WIDTH + j] = 0;
			} 
			else {
				B[i * WIDTH + j] = 0;
				G[i * WIDTH + j] = 0;
				R[i * WIDTH + j] = data[i * WIDTH + j];
			}
		}
	}
	cout << __FUNCTION__ << " finished" << endl;
	return result;
}

void FirstPixelInsertProcess(uint16_t* src, uint16_t* dst) {
	int32_t i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i % 2 == 0 && j % 2 == 1) {
				if (j == WIDTH - 1) {
					dst[i * WIDTH + j] = src[i * WIDTH + j - 1];
				}
				else {
					dst[i * WIDTH + j] = (src[i * WIDTH + j - 1] +
						src[i * WIDTH + j + 1]) / 2;
				}
			}
			if (i % 2 == 1 && j % 2 == 0) {
				if (i == HEIGHT - 1) {
					dst[i * WIDTH + j] = src[(i - 1) * WIDTH + j];
				}
				else {
					dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j] +
						src[(i + 1) * WIDTH + j]) / 2;
				}
			}
		}
	}
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i % 2 == 1 && j % 2 == 1){
				if (j < WIDTH - 1 && i < HEIGHT - 1) {
					dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j - 1] +
						src[(i - 1) * WIDTH + j + 1] +
						src[(i + 1) * WIDTH + j - 1] +
						src[(i + 1) * WIDTH + j + 1]) / 4;
				}
				else if (i == HEIGHT - 1 && j < WIDTH - 1) {
					dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j - 1] +
						src[(i - 1) * WIDTH + j + 1]) / 2;
				}
				else if (i < HEIGHT - 1 && j == WIDTH - 1) {
					dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j - 1] +
						src[(i + 1) * WIDTH + j - 1]) / 2;
				}
				else {
					dst[i * WIDTH + j] = src[(i - 1) * WIDTH + j - 1];
				}
					
			}
		}
	}
	//cout << " First Pixel Insert Process finished " << endl;
}

void TwoGPixelInsertProcess(uint16_t* src, uint16_t* dst) {
	int32_t i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i == 0 && j == 0) {
				dst[i * WIDTH + j] = (src[(i + 1) * WIDTH + j] +
					src[i * WIDTH + j + 1]) / 2;
			}
			else if (i == 0 && j > 0 && j % 2 == 0) {
				dst[i * WIDTH + j] = (src[i * WIDTH + j - 1] +
					src[(i + 1) * WIDTH + j] +
					src[i * WIDTH + j + 1]) / 3;
			}
			else if (i > 0 && j == 0 && i % 2 == 0) {
				dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j] +
					src[i * WIDTH + j + 1] +
					src[(i + 1) * WIDTH + j]) / 3;
			}
			else if (i == HEIGHT - 1 && j < WIDTH - 1 && j % 2 == 1) {
				dst[i * WIDTH + j] = (src[i * WIDTH + j - 1] +
					src[(i - 1) * WIDTH + j] +
					src[i * WIDTH + j + 1]) / 3;
			}
			else if (i < HEIGHT - 1 && j == WIDTH - 1 && i % 2 == 1) {
				dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j] +
					src[i * WIDTH + j - 1] +
					src[(i + 1) * WIDTH + j]) / 3;
			}
			else if (i == HEIGHT - 1 && j == WIDTH - 1) {
				dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j] +
					src[i * WIDTH + j - 1]) / 2;
			}
			else if ((i % 2 == 0 && j % 2 == 0) ||
				(i % 2 == 1 && j % 2 == 1)){
				dst[i * WIDTH + j] = (src[i * WIDTH + j - 1] +
					src[i * WIDTH + j + 1] +
					src[(i - 1) * WIDTH + j] +
					src[(i + 1) * WIDTH + j]) / 4;
			}
		}
	}
	//cout << " TWO Green Pixel Insert Process finished " << endl;
}

void LastPixelInsertProcess(uint16_t* src, uint16_t* dst) {
	int32_t i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i % 2 == 1 && j % 2 == 0) {
				if (j == 0) {
					dst[i * WIDTH + j] = src[i * WIDTH + j + 1];
				}
				else {
					dst[i * WIDTH + j] = (src[i * WIDTH + j - 1] +
						src[i * WIDTH + j + 1]) / 2;
				}
			}
			if (i % 2 == 0 && j % 2 == 1) {
				if (i == 0) {
					dst[i * WIDTH + j] = src[(i + 1) * WIDTH + j];
				}
				else {
					dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j] +
						src[(i + 1) * WIDTH + j]) / 2;
				}
			}
		}
	}
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i % 2 == 0 && j % 2 == 0) {
				if (i > 0 && j > 0) {
					dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j - 1] +
						src[(i - 1) * WIDTH + j + 1] +
						src[(i + 1) * WIDTH + j - 1] +
						src[(i + 1) * WIDTH + j + 1]) / 4;
				}
				else if (i == 0 && j > 0) {
					dst[i * WIDTH + j] = (src[(i + 1) * WIDTH + j - 1] +
						src[(i + 1) * WIDTH + j + 1]) / 2;
				}
				else if (i > 0 && j == 0) {
					dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j + 1] +
						src[(i + 1) * WIDTH + j + 1]) / 2;
				}
				else {
					dst[i * WIDTH + j] = src[(i + 1) * WIDTH + j + 1];
				}
			}
		}
	}
	//cout << " Last Pixel Insert Process finished " << endl;
}

void Compress10to8(uint16_t* src, unsigned char* dst, int32_t size, bool need_420_521)
{
	if (need_420_521) {
		double temp;
		uint16_t tempInt;
		for (int i = 0; i < size; i++) {
			temp = src[i] / 1024.0 * 256.0;
			tempInt = (uint16_t)(temp * 10);
			tempInt = tempInt % 10;
			if (tempInt < 5 || temp > 255) {
				dst[i] = (uint8_t)temp & 0xff;
			}
			else {
				dst[i] = ((uint8_t)temp + 1) & 0xff;
			}
		}
	}
	else {
		for (int i = 0; i < size; i++) {
			dst[i] = (src[i] >> 2) & 0xff;
		}
	}
}
