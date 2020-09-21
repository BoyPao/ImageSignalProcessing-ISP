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

#define SOURCE "Source"
#define RESNAME "Result"
#define TEMP "Temp"

#define INPUTPATH "C:\\Users\\penghao6\\Desktop\\1MCC_IMG_20181229_001526_1.RAW"   
#define OUTPUTPATH "C:\\Users\\penghao6\\Desktop\\output.BMP"
#define LOGPATH "C:\\Users\\penghao6\\Desktop\\output.txt"
#define WIDTH 1920
#define HEIGHT 1080

//#define LOG_SWITCH_ON 

void main() {
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
	unsigned char* data = new unsigned char[numPixel * 5 / 4];
	int* decodedata = new int[numPixel];
	int* BGRdata = new int[numPixel * 3];
	int* Bdata = BGRdata;
	int* Gdata = Bdata + numPixel;
	int* Rdata = Gdata + numPixel;

	unsigned char* rgb = new unsigned char[numPixel * 3];
	unsigned char* r = rgb;
	unsigned char* g = r + numPixel;
	unsigned char* b = g + numPixel;
	Mat dst(HEIGHT, WIDTH, CV_8UC3, Scalar(0, 0, 0));
	int i, j;
	for (i = 0; i < 3 * numPixel; i++) {
		BGRdata[i] = 0;
	}

	result = pImgFileManager->ReadRawData(data, Mipi10Bit);
	if (result == ISPSuccess) {
		int Imgsizex, Imgsizey;
		int Winsizex, Winsizey;
		Winsizex = GetSystemMetrics(SM_CXSCREEN);
		Winsizey = GetSystemMetrics(SM_CYSCREEN);
		Imgsizey = Winsizey * 2 / 3;
		Imgsizex = Imgsizey * WIDTH / HEIGHT;

		bool is_WB1st_Gamma2nd = true;
		const float Rgain = is_WB1st_Gamma2nd ? 1.994 : 1.378;
		const float Ggain = is_WB1st_Gamma2nd ? 1.0 : 1.0;
		const float Bgain = is_WB1st_Gamma2nd ? 2.372 : 1.493;

		//Mipi10 decode
		result = Mipi10decode(data, decodedata,pImgFileManager->GetInputImgInfo().rawSize);

#ifdef LOG_SWITCH_ON
		intDataSaveAsText(decodedata, HEIGHT, WIDTH, LOGPATH);
#endif

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
		result = node->Init(BLC);
		result = node->Process((void*)decodedata, /*argNum*/0);
		result = node->Init(LSC);
		result = node->Process((void*)decodedata, /*argNum*/0);
		//Bayer Space Process
		result = ReadChannels(decodedata, Bdata, Gdata, Rdata);//pick up RGB  channels from Raw
		result = node->Init(GCC);
		result = node->Process((void*)Gdata, /*argNum*/0);
		result = Demosaic(decodedata, Bdata, Gdata, Rdata);

		//RGB Space Process
		result = node->Init(WB);
		result = node->Process((void*)BGRdata, /*argNum*/0);
		result = node->Init(CC);
		result = node->Process((void*)BGRdata, /*argNum*/0);
		result = node->Init(GAMMA);
		result = node->Process((void*)BGRdata, /*argNum*/0);

		Compress10to8(Bdata, b);
		Compress10to8(Gdata, g);
		Compress10to8(Rdata, r);

		//Fill a BMP data use three channals data
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				dst.data[i * WIDTH * 3 + 3 * j] = (unsigned int)b[i * WIDTH + j];
				dst.data[i * WIDTH * 3 + 3 * j + 1] = (unsigned int)g[i * WIDTH + j];
				dst.data[i * WIDTH * 3 + 3 * j + 2] = (unsigned int)r[i * WIDTH + j];
			}
		}

		Mat YUV, NRYUV;
		YUV = dst.clone();
		cvtColor(YUV, YUV, COLOR_BGR2YCrCb, 0);

		//YUV Space Process
		result = node->Init(SWNR);
		result = node->Process((void*)YUV.data, /*argNum*/2, Imgsizey, Imgsizex);
		result = node->Init(SHARPNESS);
		result = node->Process((void*)YUV.data, /*argNum*/0);

		cvtColor(YUV, dst, COLOR_YCrCb2BGR, 0);

		//Save the result
		Mat output;
		output = dst.clone();
		result = pImgFileManager->SaveBMP(output.data, output.channels());
		delete pImgFileManager;
		delete[] data;
		delete[] decodedata;
		delete[] BGRdata;
		delete[] rgb;
		namedWindow(RESNAME, 0);
		resizeWindow(RESNAME, Imgsizex, Imgsizey);
		imshow(RESNAME, output);
		waitKey(0);
	}
	cin >> i;
}

ISPResult Demosaic(int* data, int* B, int* G, int* R) {
	ISPResult result = ISPSuccess;
	FirstPixelInsertProcess(data, B);
	TwoGPixelInsertProcess(data, G);
	LastPixelInsertProcess(data, R);
	cout << __FUNCTION__<< " finished" << endl;
	return result;
}


ISPResult Mipi10decode(unsigned char* src, int* dst, unsigned int rawSize) {
	ISPResult result = ISPSuccess;
	int i;
	for (i = 0; i < rawSize; i += 5) {
		dst[i * 4 / 5] = ((int)src[i] << 2) + (src[i + 4] & 0x3);
		dst[i * 4 / 5 + 1] = ((int)src[i + 1] << 2) + ((src[i + 4] >> 2) & 0x3);
		dst[i * 4 / 5 + 2] = ((int)src[i + 2] << 2) + ((src[i + 4] >> 4) & 0x3);
		dst[i * 4 / 5 + 3] = ((int)src[i + 3] << 2) + ((src[i + 4] >> 6) & 0x3);

		if (dst[i * 4 / 5] < 0) {
			dst[i * 4 / 5] = 0;
		} else if (dst[i * 4 / 5] > 255 * 4) {
			dst[i * 4 / 5] = 255 * 4;
		}

		if (dst[i * 4 / 5 + 1] < 0) {
			dst[i * 4 / 5 + 1] = 0;
		}
		else if (dst[i * 4 / 5 +1] > 255 * 4) {
			dst[i * 4 / 5+ 1] = 255 * 4;
		}

		if (dst[i * 4 / 5 + 2] < 0) {
			dst[i * 4 / 5 + 2] = 0;
		}
		else if (dst[i * 4 / 5 + 2] > 255 * 4) {
			dst[i * 4 / 5 + 2] = 255 * 4;
		}

		if (dst[i * 4 / 5 + 3] < 0) {
			dst[i * 4 / 5 + 3] = 0;
		}
		else if (dst[i * 4 / 5 + 3] > 255 * 4) {
			dst[i * 4 / 5 + 3] = 255 * 4;
		}
	}
	cout << __FUNCTION__ << " finished" << endl;
	return result;
}

ISPResult ReadChannels(int* data, int* B, int* G, int* R) {
	ISPResult result = ISPSuccess;
	int i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i % 2 == 0 && j % 2 == 0)
				B[i * WIDTH + j] = data[i * WIDTH + j];
			if ((i % 2 == 0 && j % 2 == 1) || (i % 2 == 1 && j % 2 == 0))
				G[i * WIDTH + j] = data[i * WIDTH + j];
			if (i % 2 == 1 && j % 2 == 1)
				R[i * WIDTH + j] = data[i * WIDTH + j];
		}
	}
	cout << __FUNCTION__ << " finished" << endl;
	return result;
}

void FirstPixelInsertProcess(int* src, int* dst) {
	int i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i % 2 == 0 && j % 2 == 1 && j > 0 && j < WIDTH - 1) {
				dst[i * WIDTH + j] = (src[i * WIDTH + j - 1] +
					src[i * WIDTH + j + 1]) / 2;
			}
			if (i % 2 == 1 && j % 2 == 0 && i > 0 && i < HEIGHT - 1) {
				dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j] +
					src[(i + 1) * WIDTH + j]) / 2;
			}
		}
	}
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i % 2 == 1 && j % 2 == 1 && j > 0 &&
				j < WIDTH - 1 && i > 0 && i < HEIGHT - 1) {
				dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j - 1] +
					src[(i - 1) * WIDTH + j + 1] +
					src[(i + 1) * WIDTH + j - 1] +
					src[(i + 1) * WIDTH + j + 1]) / 4;
			}
		}
	}
	//cout << " First Pixel Insert Process finished " << endl;
}

void TwoGPixelInsertProcess(int* src, int* dst) {
	int i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i % 2 == 0 && j % 2 == 0 && j > 0 &&
				j < WIDTH - 1 && i > 0 && i < HEIGHT - 1) {
				dst[i * WIDTH + j] = (src[i * WIDTH + j - 1] +
					src[i * WIDTH + j + 1] +
					src[(i - 1) * WIDTH + j] +
					src[(i + 1) * WIDTH + j]) / 4;
			}
			if (i % 2 == 1 && j % 2 == 1 && j > 0 && j < WIDTH - 1 && i > 0 && i < HEIGHT - 1) {
				dst[i * WIDTH + j] = (src[i * WIDTH + j - 1] +
					src[i * WIDTH + j + 1] +
					src[(i - 1) * WIDTH + j] +
					src[(i + 1) * WIDTH + j]) / 4;
			}
		}
	}
	//cout << " TWO Green Pixel Insert Process finished " << endl;
}

void LastPixelInsertProcess(int* src, int* dst) {
	int i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i % 2 == 1 && j % 2 == 0 && j > 0 && j < WIDTH - 1) {
				dst[i * WIDTH + j] = (src[i * WIDTH + j - 1] +
					src[i * WIDTH + j + 1]) / 2;
			}
			if (i % 2 == 0 && j % 2 == 1 && i > 0 && i < HEIGHT - 1) {
				dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j] +
					src[(i + 1) * WIDTH + j]) / 2;
			}
		}
	}
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (i % 2 == 0 && j % 2 == 0 && j > 0 &&
				j < WIDTH - 1 && i > 0 && i < HEIGHT - 1) {
				dst[i * WIDTH + j] = (src[(i - 1) * WIDTH + j - 1] +
					src[(i - 1) * WIDTH + j + 1] +
					src[(i + 1) * WIDTH + j - 1] +
					src[(i + 1) * WIDTH + j + 1]) / 4;
			}
		}
	}
	//cout << " Last Pixel Insert Process finished " << endl;
}

void Compress10to8(int* src, unsigned char* dst) {
	int i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if ((src[i * WIDTH + j] >> 2) > 255) {
				dst[i * WIDTH + j] = 255;
			}
			else if ((src[i * WIDTH + j] >> 2) < 0) {
				dst[i * WIDTH + j] = 0;
			}
			else {
				dst[i * WIDTH + j] = (src[i * WIDTH + j] >> 2) & 255;
			}
		}
	}
}
