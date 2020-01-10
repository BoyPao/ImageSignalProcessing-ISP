//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ImageSignalProcess.cpp
// @brief ISP implementation
//////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <math.h>
#include <windows.h>
#include <fstream> 

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo.hpp"

#include "ImageSignalProcess.h"

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

ISPResult Mipidecode(unsigned char* src, int* dst);
ISPResult BlackLevelCorrection(int* data, int BLValue, bool enable);
ISPResult ReadChannels(int* data, int* B, int* G, int* R);
void intDataSaveAsText(int* data, int height, int width, string TextPath);
void CharDataSaveAsText(unsigned char* data, int height, int width, string TextPath);
ISPResult Demosaic(int* data, int* B, int* G, int* R);
void FirstPixelInsertProcess(int* src, int* dst);
void TwoGPixelInsertProcess(int* src, int* dst);
void LastPixelInsertProcess(int* src, int* dst);
void Compress10to8(int* src, unsigned char* dst);
void setBMP(BYTE* data, Mat gamadst);
void saveBMP(BYTE* data, string BMPPath);
ISPResult GammaCorrection(int* B, int* G, int* R, bool enable);
ISPResult GreenImbalanceCorrection(int* gdata, float GICweight, bool enable);
ISPResult Whitebalance(int* B, int* G, int* R, float bgain, float ggain, float rgain, bool enable);
void BF(unsigned char* b, unsigned char* g, unsigned char* r, int dec, int Colorsigma, int Spacesigma, bool enable);
ISPResult ColorCorrection(int* B, int* G, int* R, bool enable);
ISPResult LensShadingCorrection(int* data, bool enable);
void EdgePreservedNR(Mat YUV, Mat NRYUV, float arph, bool enable);

Mat getim(Mat src, int depth, int Imgsizey, int Imgsize, int channel, int strength1, int strength2, int strength3);
Mat WDT(const Mat& _src, const string _wname, const int _level);
Mat waveletDecompose(Mat _src, Mat _lowFilter, Mat _highFilter);
void wavelet(const string _wname, Mat& _lowFilter, Mat& _highFilter);
ISPResult SWNR(Mat YUV, int Imgsizey, int Imgsizex, int strength1, int strength2, int strength3, bool enable);
ISPResult Sharpness(Mat YUV, bool enable);

void main() {
	int Imgsizex, Imgsizey;
	int Winsizex, Winsizey;
	Winsizex = GetSystemMetrics(SM_CXSCREEN);
	Winsizey = GetSystemMetrics(SM_CYSCREEN);
	Imgsizey = Winsizey * 2 / 3;
	Imgsizex = Imgsizey * WIDTH / HEIGHT;
	int i, j;
	//float Rgain = 1.378,Ggain=1.0,Bgain=1.493;// from gama to WB
	float Rgain = 1.994, Ggain = 1.0, Bgain = 2.372;//from WB to gama

	ifstream OpenFile(INPUTPATH, ios::in | ios::binary);
	if (OpenFile.fail())
	{
		cout << "Open RAW failed!" << endl;
		//exit(0);
	}
	else {
		ISPResult result = ISPSuccess;
		size_t numPixel = WIDTH * HEIGHT;
		unsigned char* data = new unsigned char[numPixel * 5 / 4];
		int* decodedata = new int[numPixel];
		int* Bdata = new int[numPixel];
		int* Gdata = new int[numPixel];
		int* Rdata = new int[numPixel];
		unsigned char* r = new unsigned char[numPixel];
		unsigned char* g = new unsigned char[numPixel];
		unsigned char* b = new unsigned char[numPixel];
		Mat dst(HEIGHT, WIDTH, CV_8UC3, Scalar(0, 0, 0));
		for (i = 0; i < numPixel; i++) {
			Bdata[i] = 0;
			Gdata[i] = 0;
			Rdata[i] = 0;
		}

		OpenFile.read((char*)data, WIDTH * HEIGHT * 5 / 4);

		data = reinterpret_cast<unsigned char*>(data);
		result = Mipidecode(data, decodedata);//decode
		//intDataSaveAsText(decodedata, HEIGHT, WIDTH, LOGPATH);

		bool BLCen, LSCen, GICen, WBen, CCen, Gammaen, SWNRen, Sharpen;
		BLCen = true;
		LSCen = true;
		GICen = true;
		WBen = true;
		CCen = true;
		Gammaen = true;
		SWNRen = true;
		Sharpen = false;

		//Bayer Space Process
		result = BlackLevelCorrection(decodedata, 16 * 4, BLCen);
		result = LensShadingCorrection(decodedata, LSCen);
		result = ReadChannels(decodedata, Bdata, Gdata, Rdata);//pick up RGB  channels from Raw
		result = GreenImbalanceCorrection(Gdata, 0.5, GICen);
		result = Demosaic(decodedata, Bdata, Gdata, Rdata);

		//RGB Space Process
		result = Whitebalance(Bdata, Gdata, Rdata, Bgain, Ggain, Rgain, WBen);
		result = ColorCorrection(Bdata, Gdata, Rdata, CCen);
		result = GammaCorrection(Bdata, Gdata, Rdata, Gammaen);

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
		result = SWNR(YUV, Imgsizey, Imgsizex, 20, 20, 50, SWNRen);
		result = Sharpness(YUV, Sharpen);

		cvtColor(YUV, dst, COLOR_YCrCb2BGR, 0);

		//Save the result
		Mat output;
		output = dst.clone();
		BYTE* BMPdata = new BYTE[numPixel * dst.channels()];
		setBMP(BMPdata, output);
		saveBMP(BMPdata, OUTPUTPATH);
		delete BMPdata;
		namedWindow(RESNAME, 0);
		resizeWindow(RESNAME, Imgsizex, Imgsizey);
		imshow(RESNAME, output);
		waitKey(0);
		OpenFile.close();
	}
	cin >> i;
}


void EdgePreservedNR(Mat YUV, Mat NRYUV, float arph, bool enable) {
	if (enable == true) {
		int i, j;
		Mat Edge, YUVEdge, NoEdgeNR, AYUV, BYUV;
		Edge = YUV.clone();
		YUVEdge = YUV.clone();
		NoEdgeNR = YUV.clone();
		AYUV = YUV.clone();
		BYUV = YUV.clone();
		Canny(YUV, Edge, 40, 90, 3);

		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				YUVEdge.data[i * 3 * WIDTH + 3 * j] = YUVEdge.data[i * 3 * WIDTH + 3 * j] & Edge.data[i * WIDTH + j];
				YUVEdge.data[i * 3 * WIDTH + 3 * j + 1] = YUVEdge.data[i * 3 * WIDTH + 3 * j + 1] & Edge.data[i * WIDTH + j];
				YUVEdge.data[i * 3 * WIDTH + 3 * j + 2] = YUVEdge.data[i * 3 * WIDTH + 3 * j + 2] & Edge.data[i * WIDTH + j];
			}
		}
		bilateralFilter(YUV, NoEdgeNR, 5, 5, 9, BORDER_DEFAULT);
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				NoEdgeNR.data[i * 3 * WIDTH + 3 * j] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j] & (~Edge.data[i * WIDTH + j]);
				NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 1] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 1] & (~Edge.data[i * WIDTH + j]);
				NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 2] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 2] & (~Edge.data[i * WIDTH + j]);
			}
		}
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				AYUV.data[i * 3 * WIDTH + 3 * j] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j] + YUVEdge.data[i * 3 * WIDTH + 3 * j];
				AYUV.data[i * 3 * WIDTH + 3 * j + 1] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 1] + YUVEdge.data[i * 3 * WIDTH + 3 * j + 1];
				AYUV.data[i * 3 * WIDTH + 3 * j + 2] = NoEdgeNR.data[i * 3 * WIDTH + 3 * j + 2] + YUVEdge.data[i * 3 * WIDTH + 3 * j + 2];
			}
		}
		blur(YUV, BYUV, Size(3, 3));
		//namedWindow(TEMP, 0);
		//resizeWindow(TEMP, Imgsizex, Imgsizey);
		//imshow(TEMP, AYUV);

		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				NRYUV.data[i * 3 * WIDTH + 3 * j] = (unsigned int)AYUV.data[i * 3 * WIDTH + 3 * j] * arph + (unsigned int)BYUV.data[i * 3 * WIDTH + 3 * j] * (1 - arph);
				NRYUV.data[i * 3 * WIDTH + 3 * j + 1] = (unsigned int)AYUV.data[i * 3 * WIDTH + 3 * j + 1] * arph + (unsigned int)BYUV.data[i * 3 * WIDTH + 3 * j + 1] * (1 - arph);
				NRYUV.data[i * 3 * WIDTH + 3 * j + 2] = (unsigned int)AYUV.data[i * 3 * WIDTH + 3 * j + 2] * arph + (unsigned int)BYUV.data[i * 3 * WIDTH + 3 * j + 2] * (1 - arph);
				//NRYUV.data[i * 3 * WIDTH + 3 * j + 1] =127;
				//NRYUV.data[i * 3 * WIDTH + 3 * j + 2] =127;
			}
		}
	}
}

ISPResult Demosaic(int* data, int* B, int* G, int* R) {
	ISPResult result = ISPSuccess;
	FirstPixelInsertProcess(data, B);
	TwoGPixelInsertProcess(data, G);
	LastPixelInsertProcess(data, R);
	cout << " Demosaic finished" << endl;
	return result;
}

void intDataSaveAsText(int* data, int height, int width, string TextPath) {
	int i, j;
	ofstream OutFile(TextPath);
	for (i = 0; i < height; i++) {
		OutFile << i << ": ";
		for (j = 0; j < width; j++) {
			OutFile << data[i * width + j] << ' ';
		}
		OutFile << endl;
	}
	OutFile.close();
	cout << " Data saved as TXT finished " << endl;
}

void CharDataSaveAsText(unsigned char* data, int height, int width, string TextPath) {
	int i, j;
	ofstream OutFile(TextPath);
	for (i = 0; i < height; i++) {
		OutFile << i << ": ";
		for (j = 0; j < width; j++) {
			OutFile << (int)data[i * width + j] << ' ';
		}
		OutFile << endl;
	}
	OutFile.close();
	cout << " Data saved as TXT finished " << endl;
}

ISPResult Mipidecode(unsigned char* src, int* dst) {
	ISPResult result = ISPSuccess;
	int i;
	for (i = 0; i < WIDTH * HEIGHT * 5 / 4; i += 5) {
		dst[i * 4 / 5] = ((int)src[i] << 2) + (src[i + 4] & 0x3);
		dst[i * 4 / 5 + 1] = ((int)src[i + 1] << 2) + ((src[i + 4] >> 2) & 0x3);
		dst[i * 4 / 5 + 2] = ((int)src[i + 2] << 2) + ((src[i + 4] >> 4) & 0x3);
		dst[i * 4 / 5 + 3] = ((int)src[i + 3] << 2) + ((src[i + 4] >> 6) & 0x3);
	}
	cout << " Mipi decode finished " << endl;
	return result;
}

ISPResult BlackLevelCorrection(int* data, int BLValue, bool enable) {
	ISPResult result = ISPSuccess;
	if (enable == true) {
		int i;
		int temp = 0;
		for (i = 0; i < WIDTH * HEIGHT; i++) {
			temp = data[i] - BLValue;
			if (temp < 0)
				temp = 0;
			data[i] = temp;
		}
		cout << " BLC finished" << endl;
	} else {
		cout << " BLC disabled" << endl;
	}
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
	cout << " Read RGB channels finished " << endl;
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

void setBMP(BYTE* data, Mat datasrc)
{
	int j = 0;
	int row = 0;
	int col = 0;
	int size;
	BYTE temp;
	size = WIDTH * HEIGHT * datasrc.channels();
	memset(data, 0x00, size);
	if (datasrc.channels() == 3) {
		for (int i = 0; i < WIDTH * HEIGHT; i++) {
			data[i * 3] = datasrc.data[i * 3];
			data[i * 3 + 1] = datasrc.data[i * 3 + 1];
			data[i * 3 + 2] = datasrc.data[i * 3 + 2];
		}
		//矩阵反转
		while (j < 3 * WIDTH * HEIGHT - j) {
			temp = data[3 * WIDTH * HEIGHT - j - 1];
			data[3 * WIDTH * HEIGHT - j - 1] = data[j];
			data[j] = temp;
			j++;
		}
		//图像镜像翻转
		for (row = 0; row < HEIGHT; row++) {
			while (col < 3 * WIDTH - col) {
				temp = data[row * 3 * WIDTH + 3 * WIDTH - col - 1];
				data[3 * row * WIDTH + 3 * WIDTH - col - 1] = data[3 * row * WIDTH + col];
				data[3 * row * WIDTH + col] = temp;
				col++;
			}
			col = 0;
		}
	}
	//else if (datasrc.channels() == 1) {
	//	for (int i = 0; i < WIDTH * HEIGHT; i++) {
	//		data[i] = datasrc.data[i];
	//	}
		//矩阵反转
		/*
		while (j < 3 * WIDTH * HEIGHT - j) {
			temp = data[3 * WIDTH * HEIGHT - j - 1];
			data[3 * WIDTH * HEIGHT - j - 1] = data[j];
			data[j] = temp;
			j++;
		}
		//图像镜像翻转
		for (row = 0; row < HEIGHT; row++) {
			while (col < 3 * WIDTH - col) {
				temp = data[row * 3 * WIDTH + 3 * WIDTH - col - 1];
				data[3 * row*WIDTH + 3 * WIDTH - col - 1] = data[3 * row*WIDTH + col];
				data[3 * row*WIDTH + col] = temp;
				col++;
			}
			col = 0;
		}*/

		//}
}

void saveBMP(BYTE* data, string BMPPath) {
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER headerinfo;
	int size = WIDTH * HEIGHT * 3;

	header.bfType = 0x4D42;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size;
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	headerinfo.biSize = sizeof(BITMAPINFOHEADER);
	headerinfo.biHeight = HEIGHT;
	headerinfo.biWidth = WIDTH;
	headerinfo.biPlanes = 1;
	headerinfo.biBitCount = 24;
	headerinfo.biCompression = 0; //BI_RGB
	headerinfo.biSizeImage = size;
	headerinfo.biClrUsed = 0;
	headerinfo.biClrImportant = 0;
	cout << " BMPPath: " << BMPPath << endl;
	ofstream out(BMPPath, ios::binary);
	out.write((char*)& header, sizeof(BITMAPFILEHEADER));
	out.write((char*)& headerinfo, sizeof(BITMAPINFOHEADER));
	out.write((char*)data, size);
	out.close();
	cout << " BMP saved " << endl;
}

void GenerateGammaLookUpTable(unsigned int* lut) {
	lut[0] = 0; lut[1] = 4; lut[2] = 9; lut[3] = 13; lut[4] = 18; lut[5] = 21; lut[6] = 24; lut[7] = 27;
	lut[8] = 30; lut[9] = 33; lut[10] = 36; lut[11] = 39; lut[12] = 42; lut[13] = 45; lut[14] = 48; lut[15] = 51;
	lut[16] = 54; lut[17] = 57; lut[18] = 60; lut[19] = 63; lut[20] = 66; lut[21] = 69; lut[22] = 72; lut[23] = 75;
	lut[24] = 78; lut[25] = 81; lut[26] = 85; lut[27] = 88; lut[28] = 92; lut[29] = 95; lut[30] = 99; lut[31] = 102;
	lut[32] = 106; lut[33] = 109; lut[34] = 112; lut[35] = 115; lut[36] = 118; lut[37] = 121; lut[38] = 125; lut[39] = 129;
	lut[40] = 133; lut[41] = 136; lut[42] = 140; lut[43] = 143; lut[44] = 147; lut[45] = 150; lut[46] = 153; lut[47] = 156;
	lut[48] = 159; lut[49] = 162; lut[50] = 165; lut[51] = 168; lut[52] = 171; lut[53] = 174; lut[54] = 177; lut[55] = 180;
	lut[56] = 183; lut[57] = 186; lut[58] = 189; lut[59] = 192; lut[60] = 195; lut[61] = 198; lut[62] = 201; lut[63] = 204;
	lut[64] = 207; lut[65] = 209; lut[66] = 212; lut[67] = 214; lut[68] = 217; lut[69] = 219; lut[70] = 222; lut[71] = 224;
	lut[72] = 227; lut[73] = 230; lut[74] = 233; lut[75] = 236; lut[76] = 239; lut[77] = 241; lut[78] = 244; lut[79] = 246;
	lut[80] = 249; lut[81] = 251; lut[82] = 254; lut[83] = 256; lut[84] = 259; lut[85] = 261; lut[86] = 264; lut[87] = 266;
	lut[88] = 269; lut[89] = 271; lut[90] = 273; lut[91] = 275; lut[92] = 277; lut[93] = 279; lut[94] = 282; lut[95] = 284;
	lut[96] = 287; lut[97] = 289; lut[98] = 292; lut[99] = 294; lut[100] = 297; lut[101] = 299; lut[102] = 301; lut[103] = 303;
	lut[104] = 305; lut[105] = 307; lut[106] = 309; lut[107] = 311; lut[108] = 313; lut[109] = 315; lut[110] = 317; lut[111] = 319;
	lut[112] = 321; lut[113] = 323; lut[114] = 325; lut[115] = 327; lut[116] = 329; lut[117] = 331; lut[118] = 333; lut[119] = 335;
	lut[120] = 337; lut[121] = 339; lut[122] = 341; lut[123] = 343; lut[124] = 345; lut[125] = 346; lut[126] = 348; lut[127] = 349;
	lut[128] = 351; lut[129] = 352; lut[130] = 354; lut[131] = 355; lut[132] = 357; lut[133] = 359; lut[134] = 361; lut[135] = 363;
	lut[136] = 365; lut[137] = 367; lut[138] = 369; lut[139] = 371; lut[140] = 373; lut[141] = 375; lut[142] = 377; lut[143] = 379;
	lut[144] = 381; lut[145] = 382; lut[146] = 384; lut[147] = 386; lut[148] = 388; lut[149] = 389; lut[150] = 391; lut[151] = 392;
	lut[152] = 394; lut[153] = 395; lut[154] = 397; lut[155] = 398; lut[156] = 400; lut[157] = 401; lut[158] = 403; lut[159] = 404;
	lut[160] = 406; lut[161] = 408; lut[162] = 410; lut[163] = 412; lut[164] = 414; lut[165] = 415; lut[166] = 417; lut[167] = 418;
	lut[168] = 420; lut[169] = 421; lut[170] = 423; lut[171] = 424; lut[172] = 426; lut[173] = 428; lut[174] = 430; lut[175] = 432;
	lut[176] = 434; lut[177] = 435; lut[178] = 437; lut[179] = 438; lut[180] = 440; lut[181] = 441; lut[182] = 442; lut[183] = 443;
	lut[184] = 444; lut[185] = 445; lut[186] = 447; lut[187] = 448; lut[188] = 450; lut[189] = 452; lut[190] = 454; lut[191] = 456;
	lut[192] = 458; lut[193] = 459; lut[194] = 461; lut[195] = 462; lut[196] = 464; lut[197] = 465; lut[198] = 466; lut[199] = 467;
	lut[200] = 468; lut[201] = 469; lut[202] = 471; lut[203] = 472; lut[204] = 474; lut[205] = 475; lut[206] = 477; lut[207] = 478;
	lut[208] = 480; lut[209] = 481; lut[210] = 483; lut[211] = 484; lut[212] = 486; lut[213] = 487; lut[214] = 489; lut[215] = 490;
	lut[216] = 492; lut[217] = 493; lut[218] = 494; lut[219] = 495; lut[220] = 496; lut[221] = 497; lut[222] = 499; lut[223] = 500;
	lut[224] = 502; lut[225] = 503; lut[226] = 505; lut[227] = 506; lut[228] = 508; lut[229] = 509; lut[230] = 510; lut[231] = 511;
	lut[232] = 512; lut[233] = 513; lut[234] = 515; lut[235] = 516; lut[236] = 518; lut[237] = 519; lut[238] = 521; lut[239] = 522;
	lut[240] = 524; lut[241] = 525; lut[242] = 526; lut[243] = 527; lut[244] = 528; lut[245] = 529; lut[246] = 531; lut[247] = 532;
	lut[248] = 534; lut[249] = 535; lut[250] = 536; lut[251] = 537; lut[252] = 538; lut[253] = 539; lut[254] = 540; lut[255] = 541;
	lut[256] = 542; lut[257] = 543; lut[258] = 545; lut[259] = 546; lut[260] = 548; lut[261] = 549; lut[262] = 550; lut[263] = 551;
	lut[264] = 552; lut[265] = 553; lut[266] = 555; lut[267] = 556; lut[268] = 558; lut[269] = 559; lut[270] = 561; lut[271] = 562;
	lut[272] = 564; lut[273] = 565; lut[274] = 566; lut[275] = 567; lut[276] = 568; lut[277] = 569; lut[278] = 570; lut[279] = 571;
	lut[280] = 572; lut[281] = 573; lut[282] = 574; lut[283] = 575; lut[284] = 576; lut[285] = 577; lut[286] = 578; lut[287] = 579;
	lut[288] = 580; lut[289] = 581; lut[290] = 582; lut[291] = 583; lut[292] = 584; lut[293] = 585; lut[294] = 586; lut[295] = 587;
	lut[296] = 588; lut[297] = 589; lut[298] = 590; lut[299] = 591; lut[300] = 592; lut[301] = 593; lut[302] = 595; lut[303] = 596;
	lut[304] = 598; lut[305] = 599; lut[306] = 600; lut[307] = 601; lut[308] = 602; lut[309] = 603; lut[310] = 604; lut[311] = 605;
	lut[312] = 606; lut[313] = 607; lut[314] = 608; lut[315] = 609; lut[316] = 610; lut[317] = 611; lut[318] = 612; lut[319] = 613;
	lut[320] = 614; lut[321] = 615; lut[322] = 616; lut[323] = 617; lut[324] = 618; lut[325] = 619; lut[326] = 620; lut[327] = 621;
	lut[328] = 622; lut[329] = 623; lut[330] = 624; lut[331] = 625; lut[332] = 626; lut[333] = 627; lut[334] = 628; lut[335] = 629;
	lut[336] = 630; lut[337] = 630; lut[338] = 631; lut[339] = 631; lut[340] = 632; lut[341] = 633; lut[342] = 634; lut[343] = 635;
	lut[344] = 636; lut[345] = 637; lut[346] = 638; lut[347] = 639; lut[348] = 640; lut[349] = 641; lut[350] = 642; lut[351] = 643;
	lut[352] = 645; lut[353] = 646; lut[354] = 647; lut[355] = 648; lut[356] = 649; lut[357] = 650; lut[358] = 651; lut[359] = 652;
	lut[360] = 653; lut[361] = 654; lut[362] = 655; lut[363] = 656; lut[364] = 657; lut[365] = 658; lut[366] = 659; lut[367] = 660;
	lut[368] = 661; lut[369] = 662; lut[370] = 663; lut[371] = 664; lut[372] = 665; lut[373] = 666; lut[374] = 667; lut[375] = 668;
	lut[376] = 669; lut[377] = 669; lut[378] = 670; lut[379] = 671; lut[380] = 672; lut[381] = 672; lut[382] = 673; lut[383] = 674;
	lut[384] = 675; lut[385] = 676; lut[386] = 677; lut[387] = 678; lut[388] = 679; lut[389] = 679; lut[390] = 680; lut[391] = 680;
	lut[392] = 681; lut[393] = 682; lut[394] = 683; lut[395] = 684; lut[396] = 685; lut[397] = 685; lut[398] = 686; lut[399] = 687;
	lut[400] = 688; lut[401] = 688; lut[402] = 689; lut[403] = 690; lut[404] = 691; lut[405] = 692; lut[406] = 693; lut[407] = 694;
	lut[408] = 695; lut[409] = 696; lut[410] = 697; lut[411] = 698; lut[412] = 699; lut[413] = 699; lut[414] = 700; lut[415] = 700;
	lut[416] = 701; lut[417] = 701; lut[418] = 702; lut[419] = 703; lut[420] = 704; lut[421] = 704; lut[422] = 705; lut[423] = 706;
	lut[424] = 707; lut[425] = 708; lut[426] = 709; lut[427] = 710; lut[428] = 711; lut[429] = 711; lut[430] = 712; lut[431] = 712;
	lut[432] = 713; lut[433] = 713; lut[434] = 714; lut[435] = 715; lut[436] = 716; lut[437] = 716; lut[438] = 717; lut[439] = 718;
	lut[440] = 719; lut[441] = 720; lut[442] = 721; lut[443] = 722; lut[444] = 723; lut[445] = 723; lut[446] = 724; lut[447] = 724;
	lut[448] = 725; lut[449] = 725; lut[450] = 726; lut[451] = 727; lut[452] = 728; lut[453] = 728; lut[454] = 729; lut[455] = 730;
	lut[456] = 731; lut[457] = 732; lut[458] = 733; lut[459] = 734; lut[460] = 735; lut[461] = 735; lut[462] = 736; lut[463] = 736;
	lut[464] = 737; lut[465] = 738; lut[466] = 739; lut[467] = 740; lut[468] = 741; lut[469] = 741; lut[470] = 742; lut[471] = 743;
	lut[472] = 744; lut[473] = 744; lut[474] = 745; lut[475] = 746; lut[476] = 747; lut[477] = 747; lut[478] = 748; lut[479] = 748;
	lut[480] = 749; lut[481] = 749; lut[482] = 750; lut[483] = 751; lut[484] = 752; lut[485] = 752; lut[486] = 753; lut[487] = 754;
	lut[488] = 755; lut[489] = 755; lut[490] = 756; lut[491] = 756; lut[492] = 757; lut[493] = 757; lut[494] = 758; lut[495] = 759;
	lut[496] = 760; lut[497] = 760; lut[498] = 761; lut[499] = 762; lut[500] = 763; lut[501] = 763; lut[502] = 764; lut[503] = 764;
	lut[504] = 765; lut[505] = 765; lut[506] = 766; lut[507] = 767; lut[508] = 768; lut[509] = 768; lut[510] = 769; lut[511] = 770;
	lut[512] = 771; lut[513] = 771; lut[514] = 772; lut[515] = 772; lut[516] = 773; lut[517] = 773; lut[518] = 774; lut[519] = 775;
	lut[520] = 776; lut[521] = 776; lut[522] = 777; lut[523] = 778; lut[524] = 779; lut[525] = 779; lut[526] = 780; lut[527] = 780;
	lut[528] = 781; lut[529] = 781; lut[530] = 782; lut[531] = 783; lut[532] = 784; lut[533] = 784; lut[534] = 785; lut[535] = 786;
	lut[536] = 787; lut[537] = 787; lut[538] = 788; lut[539] = 788; lut[540] = 789; lut[541] = 789; lut[542] = 790; lut[543] = 791;
	lut[544] = 792; lut[545] = 792; lut[546] = 793; lut[547] = 794; lut[548] = 795; lut[549] = 795; lut[550] = 796; lut[551] = 796;
	lut[552] = 797; lut[553] = 797; lut[554] = 798; lut[555] = 799; lut[556] = 800; lut[557] = 800; lut[558] = 801; lut[559] = 802;
	lut[560] = 803; lut[561] = 803; lut[562] = 804; lut[563] = 804; lut[564] = 805; lut[565] = 805; lut[566] = 806; lut[567] = 807;
	lut[568] = 808; lut[569] = 808; lut[570] = 809; lut[571] = 810; lut[572] = 811; lut[573] = 811; lut[574] = 812; lut[575] = 812;
	lut[576] = 813; lut[577] = 813; lut[578] = 814; lut[579] = 815; lut[580] = 816; lut[581] = 816; lut[582] = 817; lut[583] = 818;
	lut[584] = 819; lut[585] = 819; lut[586] = 820; lut[587] = 820; lut[588] = 821; lut[589] = 821; lut[590] = 822; lut[591] = 822;
	lut[592] = 823; lut[593] = 823; lut[594] = 824; lut[595] = 824; lut[596] = 825; lut[597] = 825; lut[598] = 826; lut[599] = 826;
	lut[600] = 827; lut[601] = 827; lut[602] = 828; lut[603] = 828; lut[604] = 829; lut[605] = 829; lut[606] = 830; lut[607] = 830;
	lut[608] = 831; lut[609] = 831; lut[610] = 832; lut[611] = 832; lut[612] = 833; lut[613] = 833; lut[614] = 834; lut[615] = 835;
	lut[616] = 836; lut[617] = 836; lut[618] = 837; lut[619] = 838; lut[620] = 839; lut[621] = 839; lut[622] = 840; lut[623] = 840;
	lut[624] = 841; lut[625] = 841; lut[626] = 842; lut[627] = 843; lut[628] = 844; lut[629] = 844; lut[630] = 845; lut[631] = 845;
	lut[632] = 846; lut[633] = 846; lut[634] = 847; lut[635] = 847; lut[636] = 848; lut[637] = 848; lut[638] = 849; lut[639] = 850;
	lut[640] = 851; lut[641] = 851; lut[642] = 852; lut[643] = 852; lut[644] = 853; lut[645] = 853; lut[646] = 854; lut[647] = 854;
	lut[648] = 855; lut[649] = 855; lut[650] = 856; lut[651] = 856; lut[652] = 857; lut[653] = 857; lut[654] = 858; lut[655] = 858;
	lut[656] = 859; lut[657] = 859; lut[658] = 860; lut[659] = 860; lut[660] = 861; lut[661] = 861; lut[662] = 862; lut[663] = 863;
	lut[664] = 864; lut[665] = 864; lut[666] = 865; lut[667] = 865; lut[668] = 866; lut[669] = 866; lut[670] = 867; lut[671] = 867;
	lut[672] = 868; lut[673] = 868; lut[674] = 869; lut[675] = 869; lut[676] = 870; lut[677] = 870; lut[678] = 871; lut[679] = 871;
	lut[680] = 872; lut[681] = 872; lut[682] = 873; lut[683] = 873; lut[684] = 874; lut[685] = 874; lut[686] = 875; lut[687] = 875;
	lut[688] = 876; lut[689] = 876; lut[690] = 877; lut[691] = 877; lut[692] = 878; lut[693] = 878; lut[694] = 879; lut[695] = 879;
	lut[696] = 880; lut[697] = 880; lut[698] = 881; lut[699] = 881; lut[700] = 882; lut[701] = 882; lut[702] = 883; lut[703] = 883;
	lut[704] = 884; lut[705] = 884; lut[706] = 885; lut[707] = 885; lut[708] = 886; lut[709] = 886; lut[710] = 887; lut[711] = 887;
	lut[712] = 888; lut[713] = 888; lut[714] = 889; lut[715] = 889; lut[716] = 890; lut[717] = 890; lut[718] = 891; lut[719] = 891;
	lut[720] = 892; lut[721] = 892; lut[722] = 893; lut[723] = 893; lut[724] = 894; lut[725] = 894; lut[726] = 895; lut[727] = 895;
	lut[728] = 896; lut[729] = 896; lut[730] = 897; lut[731] = 897; lut[732] = 898; lut[733] = 898; lut[734] = 899; lut[735] = 899;
	lut[736] = 900; lut[737] = 900; lut[738] = 901; lut[739] = 902; lut[740] = 903; lut[741] = 903; lut[742] = 904; lut[743] = 904;
	lut[744] = 905; lut[745] = 905; lut[746] = 906; lut[747] = 906; lut[748] = 907; lut[749] = 907; lut[750] = 908; lut[751] = 908;
	lut[752] = 909; lut[753] = 909; lut[754] = 910; lut[755] = 910; lut[756] = 911; lut[757] = 911; lut[758] = 912; lut[759] = 912;
	lut[760] = 913; lut[761] = 913; lut[762] = 914; lut[763] = 914; lut[764] = 915; lut[765] = 915; lut[766] = 915; lut[767] = 915;
	lut[768] = 916; lut[769] = 916; lut[770] = 917; lut[771] = 917; lut[772] = 918; lut[773] = 918; lut[774] = 919; lut[775] = 919;
	lut[776] = 920; lut[777] = 920; lut[778] = 921; lut[779] = 921; lut[780] = 922; lut[781] = 922; lut[782] = 922; lut[783] = 922;
	lut[784] = 923; lut[785] = 923; lut[786] = 924; lut[787] = 924; lut[788] = 925; lut[789] = 925; lut[790] = 926; lut[791] = 926;
	lut[792] = 927; lut[793] = 927; lut[794] = 928; lut[795] = 928; lut[796] = 929; lut[797] = 929; lut[798] = 930; lut[799] = 930;
	lut[800] = 931; lut[801] = 931; lut[802] = 931; lut[803] = 931; lut[804] = 932; lut[805] = 932; lut[806] = 933; lut[807] = 933;
	lut[808] = 934; lut[809] = 934; lut[810] = 934; lut[811] = 934; lut[812] = 935; lut[813] = 935; lut[814] = 936; lut[815] = 936;
	lut[816] = 937; lut[817] = 937; lut[818] = 938; lut[819] = 938; lut[820] = 939; lut[821] = 939; lut[822] = 940; lut[823] = 940;
	lut[824] = 941; lut[825] = 941; lut[826] = 942; lut[827] = 942; lut[828] = 943; lut[829] = 943; lut[830] = 944; lut[831] = 944;
	lut[832] = 945; lut[833] = 945; lut[834] = 946; lut[835] = 946; lut[836] = 947; lut[837] = 947; lut[838] = 947; lut[839] = 947;
	lut[840] = 948; lut[841] = 948; lut[842] = 949; lut[843] = 949; lut[844] = 950; lut[845] = 950; lut[846] = 951; lut[847] = 951;
	lut[848] = 952; lut[849] = 952; lut[850] = 953; lut[851] = 953; lut[852] = 954; lut[853] = 954; lut[854] = 954; lut[855] = 954;
	lut[856] = 955; lut[857] = 955; lut[858] = 956; lut[859] = 956; lut[860] = 957; lut[861] = 957; lut[862] = 958; lut[863] = 958;
	lut[864] = 959; lut[865] = 959; lut[866] = 960; lut[867] = 960; lut[868] = 961; lut[869] = 961; lut[870] = 962; lut[871] = 962;
	lut[872] = 963; lut[873] = 963; lut[874] = 963; lut[875] = 963; lut[876] = 964; lut[877] = 964; lut[878] = 965; lut[879] = 965;
	lut[880] = 966; lut[881] = 966; lut[882] = 966; lut[883] = 966; lut[884] = 967; lut[885] = 967; lut[886] = 968; lut[887] = 968;
	lut[888] = 969; lut[889] = 969; lut[890] = 970; lut[891] = 970; lut[892] = 971; lut[893] = 971; lut[894] = 971; lut[895] = 971;
	lut[896] = 972; lut[897] = 972; lut[898] = 973; lut[899] = 973; lut[900] = 974; lut[901] = 974; lut[902] = 974; lut[903] = 974;
	lut[904] = 975; lut[905] = 975; lut[906] = 975; lut[907] = 975; lut[908] = 976; lut[909] = 976; lut[910] = 977; lut[911] = 977;
	lut[912] = 978; lut[913] = 978; lut[914] = 979; lut[915] = 979; lut[916] = 980; lut[917] = 980; lut[918] = 981; lut[919] = 981;
	lut[920] = 982; lut[921] = 982; lut[922] = 982; lut[923] = 982; lut[924] = 983; lut[925] = 983; lut[926] = 984; lut[927] = 984;
	lut[928] = 985; lut[929] = 985; lut[930] = 986; lut[931] = 986; lut[932] = 987; lut[933] = 987; lut[934] = 987; lut[935] = 987;
	lut[936] = 988; lut[937] = 988; lut[938] = 989; lut[939] = 989; lut[940] = 990; lut[941] = 990; lut[942] = 991; lut[943] = 991;
	lut[944] = 992; lut[945] = 992; lut[946] = 993; lut[947] = 993; lut[948] = 994; lut[949] = 994; lut[950] = 994; lut[951] = 994;
	lut[952] = 995; lut[953] = 995; lut[954] = 996; lut[955] = 996; lut[956] = 997; lut[957] = 997; lut[958] = 998; lut[959] = 998;
	lut[960] = 999; lut[961] = 999; lut[962] = 1000; lut[963] = 1000; lut[964] = 1001; lut[965] = 1001; lut[966] = 1002; lut[967] = 1002;
	lut[968] = 1003; lut[969] = 1003; lut[970] = 1003; lut[971] = 1003; lut[972] = 1004; lut[973] = 1004; lut[974] = 1004; lut[975] = 1004;
	lut[976] = 1005; lut[977] = 1005; lut[978] = 1006; lut[979] = 1006; lut[980] = 1007; lut[981] = 1007; lut[982] = 1007; lut[983] = 1007;
	lut[984] = 1008; lut[985] = 1008; lut[986] = 1009; lut[987] = 1009; lut[988] = 1010; lut[989] = 1010; lut[990] = 1010; lut[991] = 1010;
	lut[992] = 1011; lut[993] = 1011; lut[994] = 1012; lut[995] = 1012; lut[996] = 1013; lut[997] = 1013; lut[998] = 1014; lut[999] = 1014;
	lut[1000] = 1015; lut[1001] = 1015; lut[1002] = 1015; lut[1003] = 1015; lut[1004] = 1016; lut[1005] = 1016; lut[1006] = 1016; lut[1007] = 1016;
	lut[1008] = 1017; lut[1009] = 1017; lut[1010] = 1018; lut[1011] = 1018; lut[1012] = 1019; lut[1013] = 1019; lut[1014] = 1019; lut[1015] = 1019;
	lut[1016] = 1020; lut[1017] = 1020; lut[1018] = 1021; lut[1019] = 1021; lut[1020] = 1022; lut[1021] = 1022; lut[1022] = 1022; lut[1023] = 1023;
}

ISPResult GammaCorrection(int* B, int* G, int* R, bool enable)
{
	ISPResult result = ISPSuccess;
	unsigned int lut[1024];
	unsigned int* plut;
	plut = lut;

	GenerateGammaLookUpTable(plut);

	if (enable == true) {
		for (int i = 0; i < WIDTH * HEIGHT; i++)
		{
			B[i] = lut[B[i]];
			G[i] = lut[G[i]];
			R[i] = lut[R[i]];
		}
		cout << " Gamma finished" << endl;
	}
	return result;
}



ISPResult GreenImbalanceCorrection(int* gdata, float GICweight, bool enable) {
	ISPResult result = ISPSuccess;
	if (enable == true) {
		int i, j;
		float temp = 1.0;
		for (i = 0; i < HEIGHT; i++) {
			for (j = 1; j < WIDTH; j++) {
				if (i % 2 == 0 && j % 2 == 1 && i > 0 &&
					i < HEIGHT - 1 && j > 0 && j < WIDTH - 1) {
					temp = ((int)gdata[(i - 1) * WIDTH + j - 1] +
						(int)gdata[(i - 1) * WIDTH + j + 1] +
						(int)gdata[(i + 1) * WIDTH + j - 1] +
						(int)gdata[(i + 1) * WIDTH + j + 1]) << 2;
					temp = (gdata[i * WIDTH + j] << 2) - temp / 4.0;
					gdata[i * WIDTH + j] = ((int)((gdata[i * WIDTH + j] << 2) - temp * GICweight)) >> 2;
				}
			}
		}
		/*for (i = 0; i < HEIGHT; i++) {
			for (j = 1; j < WIDTH; j++) {
				if (i % 2 == 1 && j % 2 == 0 && i > 0 && i < HEIGHT - 1 && j>0 && j < WIDTH - 1) {
					temp = ((int)gdata[(i - 1)*WIDTH + j - 1] + (int)gdata[(i - 1)*WIDTH + j + 1] + (int)gdata[(i + 1)*WIDTH + j - 1] + (int)gdata[(i + 1)*WIDTH + j + 1])<<2;
					temp = (gdata[i *WIDTH + j]<<2) - temp / 4.0;
					gdata[i *WIDTH + j] = ((int)((gdata[i *WIDTH + j]<<2)+ temp * GICweight))>>2;
				}
			}
		}*/
		cout << " GIC finished" << endl;
	}
	return result;
}

ISPResult Whitebalance(int* B, int* G, int* R, float bgain, float ggain, float rgain, bool enable) {
	ISPResult result = ISPSuccess;
	if (enable == true) {
		int i;
		for (i = 0; i < WIDTH * HEIGHT; i++)
		{
			B[i] = B[i] * bgain;
			G[i] = G[i] * ggain;
			R[i] = R[i] * rgain;
		}
		cout << " WB finished" << endl;
	}
	return result;
}


void BF(unsigned char* b, unsigned char* g, unsigned char* r, int dec, int Colorsigma, int Spacesigma, bool enable) {
	if (enable == true) {
		Mat ABFsrc(HEIGHT, WIDTH, CV_8UC1);
		Mat ABFb(HEIGHT, WIDTH, CV_8UC1);
		Mat ABFg(HEIGHT, WIDTH, CV_8UC1);
		Mat ABFr(HEIGHT, WIDTH, CV_8UC1);
		ABFsrc.data = b;
		bilateralFilter(ABFsrc, ABFb, dec, Colorsigma, Spacesigma, BORDER_DEFAULT);
		ABFsrc.data = g;
		bilateralFilter(ABFsrc, ABFg, dec, Colorsigma, Spacesigma, BORDER_DEFAULT);
		ABFsrc.data = r;
		bilateralFilter(ABFsrc, ABFr, dec, Colorsigma, Spacesigma, BORDER_DEFAULT);
		for (int i = 0; i < HEIGHT * WIDTH; i++) {
			b[i] = ABFb.data[i];
			g[i] = ABFg.data[i];
			r[i] = ABFr.data[i];
		}
		cout << " ABF finished" << endl;
	}
}



ISPResult ColorCorrection(int* B, int* G, int* R, bool enable) {
	ISPResult result = ISPSuccess;
	Mat A_cc = Mat::zeros(3, 3, CV_32FC1);
	A_cc.at<float>(0, 0) = 1.819;	A_cc.at<float>(0, 1) = -0.248;	A_cc.at<float>(0, 2) = 0.213;
	A_cc.at<float>(1, 0) = -1.069;	A_cc.at<float>(1, 1) = 1.322;	A_cc.at<float>(1, 2) = -1.078;
	A_cc.at<float>(2, 0) = 0.250;	A_cc.at<float>(2, 1) = -0.074;	A_cc.at<float>(2, 2) = 1.865;

	if (enable == true) {
		int i, j;
		Mat Pmatric;
		Mat Oritransform(WIDTH * HEIGHT, 3, CV_32FC1);

		for (i = 0; i < HEIGHT * WIDTH; i++) {
			Oritransform.at<float>(i, 0) = (float)B[i];
			Oritransform.at<float>(i, 1) = (float)G[i];
			Oritransform.at<float>(i, 2) = (float)R[i];
		}

		//ofstream OutFile2("C:\\Users\\penghao6\\Desktop\\变形.txt");
		//OutFile2 << Oritransform;
		//OutFile2.close();

		Pmatric = Oritransform * A_cc;

		//ofstream OutFile3("C:\\Users\\penghao6\\Desktop\\计算结果.txt");
		//OutFile3 << Pmatric;
		//OutFile3.close();

		for (i = 0; i < HEIGHT * WIDTH; i++) {
			if (Pmatric.at<float>(i, 0) > 1023)
				Pmatric.at<float>(i, 0) = 1023;
			if (Pmatric.at<float>(i, 0) < 0)
				Pmatric.at<float>(i, 0) = 0;
			if (Pmatric.at<float>(i, 1) > 1023)
				Pmatric.at<float>(i, 1) = 1023;
			if (Pmatric.at<float>(i, 1) < 0)
				Pmatric.at<float>(i, 1) = 0;
			if (Pmatric.at<float>(i, 2) > 1023)
				Pmatric.at<float>(i, 2) = 1023;
			if (Pmatric.at<float>(i, 2) < 0)
				Pmatric.at<float>(i, 2) = 0;
			B[i] = (int)Pmatric.at<float>(i, 0);
			G[i] = (int)Pmatric.at<float>(i, 1);
			R[i] = (int)Pmatric.at<float>(i, 2);
		}

		//ofstream OutFile4("C:\\Users\\penghao6\\Desktop\\反变形.txt");
		//OutFile4 << Ori;
		//OutFile4.close();

		//ofstream OutFile5("C:\\Users\\penghao6\\Desktop\\反归一化.txt");
		//OutFile5 <<income;
		//OutFile5.close();
		//CharDataSaveAsText(dst.data, "C:\\Users\\penghao6\\Desktop\\output2.txt");
		cout << " CC finished" << endl;
	}
	return result;
}


float LSCinterpolation(float LT, float RT, float LB, float RB, int row, int col) {
	float TempT, TempB, result;
	TempT = LT - (LT - RT) * (col % (WIDTH / 16)) * 16 / WIDTH;
	TempB = LB - (LB - RB) * (col % (WIDTH / 16)) * 16 / WIDTH;
	result = TempT - (TempT - TempB) * (row % (HEIGHT / 12)) * 12 / HEIGHT;
	return result;
}

void GenerateLSCGainTable(float** Rgain, float** Grgain, float** Gbgain, float** Bgain) {
	Rgain[0][0] = 3.26140237; Rgain[0][1] = 2.90829229; Rgain[0][2] = 2.44395447; Rgain[0][3] = 2.14117551; Rgain[0][4] = 1.92580986; Rgain[0][5] = 1.76098931; Rgain[0][6] = 1.64909816; Rgain[0][7] = 1.57704759; Rgain[0][8] = 1.554534; Rgain[0][9] = 1.58389091; Rgain[0][10] = 1.65474868; Rgain[0][11] = 1.77599037; Rgain[0][12] = 1.95483339; Rgain[0][13] = 2.17210364; Rgain[0][14] = 2.49666047; Rgain[0][15] = 2.99027562; Rgain[0][16] = 3.42638421;
	Rgain[1][0] = 3.10735345; Rgain[1][1] = 2.697388; Rgain[1][2] = 2.26802278; Rgain[1][3] = 1.99615026; Rgain[1][4] = 1.77538717; Rgain[1][5] = 1.60802543; Rgain[1][6] = 1.48951221; Rgain[1][7] = 1.41702485; Rgain[1][8] = 1.39288378; Rgain[1][9] = 1.420929; Rgain[1][10] = 1.49541867; Rgain[1][11] = 1.61999846; Rgain[1][12] = 1.79255354; Rgain[1][13] = 2.01902175; Rgain[1][14] = 2.304034; Rgain[1][15] = 2.75090647; Rgain[1][16] = 3.242971;
	Rgain[2][0] = 2.97376251; Rgain[2][1] = 2.51484275; Rgain[2][2] = 2.12860441; Rgain[2][3] = 1.86797869; Rgain[2][4] = 1.64624727; Rgain[2][5] = 1.47517741; Rgain[2][6] = 1.35393524; Rgain[2][7] = 1.28068864; Rgain[2][8] = 1.25363386; Rgain[2][9] = 1.28241861; Rgain[2][10] = 1.35708928; Rgain[2][11] = 1.47977376; Rgain[2][12] = 1.64895618; Rgain[2][13] = 1.8743093; Rgain[2][14] = 2.14584637; Rgain[2][15] = 2.539748; Rgain[2][16] = 3.059384;
	Rgain[3][0] = 2.858949; Rgain[3][1] = 2.38620639; Rgain[3][2] = 2.041443; Rgain[3][3] = 1.77220476; Rgain[3][4] = 1.54693; Rgain[3][5] = 1.37411761; Rgain[3][6] = 1.25212264; Rgain[3][7] = 1.171063; Rgain[3][8] = 1.14400363; Rgain[3][9] = 1.17096663; Rgain[3][10] = 1.25206625; Rgain[3][11] = 1.3736707; Rgain[3][12] = 1.54114032; Rgain[3][13] = 1.765352; Rgain[3][14] = 2.03399444; Rgain[3][15] = 2.38793254; Rgain[3][16] = 2.90239644;
	Rgain[4][0] = 2.7713716; Rgain[4][1] = 2.310951; Rgain[4][2] = 1.97953582; Rgain[4][3] = 1.70900917; Rgain[4][4] = 1.47973192; Rgain[4][5] = 1.30391; Rgain[4][6] = 1.17499089; Rgain[4][7] = 1.09408915; Rgain[4][8] = 1.06599355; Rgain[4][9] = 1.08896232; Rgain[4][10] = 1.17121; Rgain[4][11] = 1.29502475; Rgain[4][12] = 1.46141851; Rgain[4][13] = 1.687468; Rgain[4][14] = 1.960504; Rgain[4][15] = 2.29183125; Rgain[4][16] = 2.79347658;
	Rgain[5][0] = 2.71774817; Rgain[5][1] = 2.27686; Rgain[5][2] = 1.94907153; Rgain[5][3] = 1.6727308; Rgain[5][4] = 1.44130123; Rgain[5][5] = 1.26184094; Rgain[5][6] = 1.13418913; Rgain[5][7] = 1.05143952; Rgain[5][8] = 1.0188297; Rgain[5][9] = 1.04448736; Rgain[5][10] = 1.12113214; Rgain[5][11] = 1.24961007; Rgain[5][12] = 1.41669822; Rgain[5][13] = 1.64373708; Rgain[5][14] = 1.916427; Rgain[5][15] = 2.24438834; Rgain[5][16] = 2.72118878;
	Rgain[6][0] = 2.72102714; Rgain[6][1] = 2.26686883; Rgain[6][2] = 1.9444381; Rgain[6][3] = 1.66600811; Rgain[6][4] = 1.43206453; Rgain[6][5] = 1.25416493; Rgain[6][6] = 1.12143385; Rgain[6][7] = 1.03733242; Rgain[6][8] = 1.00448644; Rgain[6][9] = 1.03098881; Rgain[6][10] = 1.10780609; Rgain[6][11] = 1.23636043; Rgain[6][12] = 1.40724814; Rgain[6][13] = 1.6315949; Rgain[6][14] = 1.90451217; Rgain[6][15] = 2.22804785; Rgain[6][16] = 2.701279;
	Rgain[7][0] = 2.73730564; Rgain[7][1] = 2.29291; Rgain[7][2] = 1.96152222; Rgain[7][3] = 1.68367219; Rgain[7][4] = 1.45214558; Rgain[7][5] = 1.27432334; Rgain[7][6] = 1.142153; Rgain[7][7] = 1.0562; Rgain[7][8] = 1.02521479; Rgain[7][9] = 1.05012667; Rgain[7][10] = 1.12978125; Rgain[7][11] = 1.25924039; Rgain[7][12] = 1.426747; Rgain[7][13] = 1.65111053; Rgain[7][14] = 1.92359591; Rgain[7][15] = 2.247887; Rgain[7][16] = 2.73132014;
	Rgain[8][0] = 2.81994367; Rgain[8][1] = 2.34834957; Rgain[8][2] = 2.004114; Rgain[8][3] = 1.73133981; Rgain[8][4] = 1.49727178; Rgain[8][5] = 1.32004464; Rgain[8][6] = 1.19515443; Rgain[8][7] = 1.10757565; Rgain[8][8] = 1.07641768; Rgain[8][9] = 1.10019207; Rgain[8][10] = 1.18332469; Rgain[8][11] = 1.308249; Rgain[8][12] = 1.47925; Rgain[8][13] = 1.69980335; Rgain[8][14] = 1.96897411; Rgain[8][15] = 2.306974; Rgain[8][16] = 2.80438614;
	Rgain[9][0] = 2.92639732; Rgain[9][1] = 2.44216776; Rgain[9][2] = 2.07634854; Rgain[9][3] = 1.799391; Rgain[9][4] = 1.56975734; Rgain[9][5] = 1.39434123; Rgain[9][6] = 1.27109933; Rgain[9][7] = 1.190723; Rgain[9][8] = 1.15844178; Rgain[9][9] = 1.18562472; Rgain[9][10] = 1.26415074; Rgain[9][11] = 1.38814867; Rgain[9][12] = 1.5586611; Rgain[9][13] = 1.77739751; Rgain[9][14] = 2.04431272; Rgain[9][15] = 2.4038713; Rgain[9][16] = 2.91905713;
	Rgain[10][0] = 3.055297; Rgain[10][1] = 2.585944; Rgain[10][2] = 2.17785; Rgain[10][3] = 1.90352452; Rgain[10][4] = 1.673042; Rgain[10][5] = 1.500913; Rgain[10][6] = 1.37676442; Rgain[10][7] = 1.29876518; Rgain[10][8] = 1.27201283; Rgain[10][9] = 1.29613686; Rgain[10][10] = 1.3717885; Rgain[10][11] = 1.49768281; Rgain[10][12] = 1.66706812; Rgain[10][13] = 1.887339; Rgain[10][14] = 2.15350842; Rgain[10][15] = 2.56109357; Rgain[10][16] = 3.0679822;
	Rgain[11][0] = 3.192101; Rgain[11][1] = 2.78453779; Rgain[11][2] = 2.33802271; Rgain[11][3] = 2.03929257; Rgain[11][4] = 1.8125155; Rgain[11][5] = 1.64407277; Rgain[11][6] = 1.52214587; Rgain[11][7] = 1.44450521; Rgain[11][8] = 1.41926432; Rgain[11][9] = 1.44269049; Rgain[11][10] = 1.520358; Rgain[11][11] = 1.6435467; Rgain[11][12] = 1.81265891; Rgain[11][13] = 2.03104234; Rgain[11][14] = 2.32003713; Rgain[11][15] = 2.78072262; Rgain[11][16] = 3.248023;
	Rgain[12][0] = 3.322562; Rgain[12][1] = 3.01884437; Rgain[12][2] = 2.54817438; Rgain[12][3] = 2.208989; Rgain[12][4] = 1.98511779; Rgain[12][5] = 1.81881928; Rgain[12][6] = 1.70027828; Rgain[12][7] = 1.62241781; Rgain[12][8] = 1.5952847; Rgain[12][9] = 1.62019539; Rgain[12][10] = 1.69814765; Rgain[12][11] = 1.817103; Rgain[12][12] = 1.98954976; Rgain[12][13] = 2.2055006; Rgain[12][14] = 2.55155873; Rgain[12][15] = 3.02624321; Rgain[12][16] = 3.44730616;

	Grgain[0][0] = 3.20395136; Grgain[0][1] = 2.84868073; Grgain[0][2] = 2.42183638; Grgain[0][3] = 2.12311363; Grgain[0][4] = 1.91179454; Grgain[0][5] = 1.756917; Grgain[0][6] = 1.64773118; Grgain[0][7] = 1.58069026; Grgain[0][8] = 1.55754256; Grgain[0][9] = 1.5834856; Grgain[0][10] = 1.645207; Grgain[0][11] = 1.74964738; Grgain[0][12] = 1.90877008; Grgain[0][13] = 2.1196; Grgain[0][14] = 2.41837454; Grgain[0][15] = 2.8678813; Grgain[0][16] = 3.28876615;
	Grgain[1][0] = 3.04708457; Grgain[1][1] = 2.663059; Grgain[1][2] = 2.24919081; Grgain[1][3] = 1.97792578; Grgain[1][4] = 1.76208353; Grgain[1][5] = 1.60036659; Grgain[1][6] = 1.48812246; Grgain[1][7] = 1.417937; Grgain[1][8] = 1.39421225; Grgain[1][9] = 1.41655445; Grgain[1][10] = 1.48318923; Grgain[1][11] = 1.59559619; Grgain[1][12] = 1.75348139; Grgain[1][13] = 1.96583772; Grgain[1][14] = 2.233202; Grgain[1][15] = 2.655679; Grgain[1][16] = 3.10571742;
	Grgain[2][0] = 2.92302465; Grgain[2][1] = 2.48888254; Grgain[2][2] = 2.1116817; Grgain[2][3] = 1.85055757; Grgain[2][4] = 1.63272786; Grgain[2][5] = 1.46424925; Grgain[2][6] = 1.34551013; Grgain[2][7] = 1.27543545; Grgain[2][8] = 1.24784422; Grgain[2][9] = 1.27263653; Grgain[2][10] = 1.34007871; Grgain[2][11] = 1.45550752; Grgain[2][12] = 1.61366773; Grgain[2][13] = 1.82921481; Grgain[2][14] = 2.08797812; Grgain[2][15] = 2.461877; Grgain[2][16] = 2.94127584;
	Grgain[3][0] = 2.8214345; Grgain[3][1] = 2.3663547; Grgain[3][2] = 2.02269816; Grgain[3][3] = 1.75983441; Grgain[3][4] = 1.53516734; Grgain[3][5] = 1.361737; Grgain[3][6] = 1.2384063; Grgain[3][7] = 1.16158283; Grgain[3][8] = 1.13543427; Grgain[3][9] = 1.15850842; Grgain[3][10] = 1.231841; Grgain[3][11] = 1.34799671; Grgain[3][12] = 1.50860322; Grgain[3][13] = 1.7240684; Grgain[3][14] = 1.98546493; Grgain[3][15] = 2.32491326; Grgain[3][16] = 2.80031347;
	Grgain[4][0] = 2.73184371; Grgain[4][1] = 2.289873; Grgain[4][2] = 1.96527112; Grgain[4][3] = 1.69687462; Grgain[4][4] = 1.46764481; Grgain[4][5] = 1.29081559; Grgain[4][6] = 1.161118; Grgain[4][7] = 1.08390057; Grgain[4][8] = 1.05756307; Grgain[4][9] = 1.07895672; Grgain[4][10] = 1.15257657; Grgain[4][11] = 1.27148235; Grgain[4][12] = 1.43292487; Grgain[4][13] = 1.64994228; Grgain[4][14] = 1.915042; Grgain[4][15] = 2.240633; Grgain[4][16] = 2.69434977;
	Grgain[5][0] = 2.69206333; Grgain[5][1] = 2.253345; Grgain[5][2] = 1.93331969; Grgain[5][3] = 1.66203713; Grgain[5][4] = 1.43015265; Grgain[5][5] = 1.24909043; Grgain[5][6] = 1.12103391; Grgain[5][7] = 1.04398239; Grgain[5][8] = 1.015522; Grgain[5][9] = 1.03709757; Grgain[5][10] = 1.10727417; Grgain[5][11] = 1.22759187; Grgain[5][12] = 1.39046443; Grgain[5][13] = 1.61131787; Grgain[5][14] = 1.87466943; Grgain[5][15] = 2.18963933; Grgain[5][16] = 2.630392;
	Grgain[6][0] = 2.67528725; Grgain[6][1] = 2.245041; Grgain[6][2] = 1.92658973; Grgain[6][3] = 1.65386665; Grgain[6][4] = 1.42180026; Grgain[6][5] = 1.2418189; Grgain[6][6] = 1.11271071; Grgain[6][7] = 1.03221583; Grgain[6][8] = 1.00493169; Grgain[6][9] = 1.0286479; Grgain[6][10] = 1.09747708; Grgain[6][11] = 1.216828; Grgain[6][12] = 1.38011265; Grgain[6][13] = 1.59923065; Grgain[6][14] = 1.86305082; Grgain[6][15] = 2.17897844; Grgain[6][16] = 2.61477852;
	Grgain[7][0] = 2.70543; Grgain[7][1] = 2.26843548; Grgain[7][2] = 1.94370532; Grgain[7][3] = 1.6720295; Grgain[7][4] = 1.44359267; Grgain[7][5] = 1.26507568; Grgain[7][6] = 1.13638735; Grgain[7][7] = 1.05558419; Grgain[7][8] = 1.02966225; Grgain[7][9] = 1.05121982; Grgain[7][10] = 1.12055671; Grgain[7][11] = 1.24107945; Grgain[7][12] = 1.40166306; Grgain[7][13] = 1.61662066; Grgain[7][14] = 1.880561; Grgain[7][15] = 2.19829535; Grgain[7][16] = 2.64114118;
	Grgain[8][0] = 2.77835655; Grgain[8][1] = 2.32503271; Grgain[8][2] = 1.98681116; Grgain[8][3] = 1.71911311; Grgain[8][4] = 1.49061728; Grgain[8][5] = 1.31596816; Grgain[8][6] = 1.1931448; Grgain[8][7] = 1.11112535; Grgain[8][8] = 1.0833174; Grgain[8][9] = 1.1038332; Grgain[8][10] = 1.17803967; Grgain[8][11] = 1.29450274; Grgain[8][12] = 1.45461929; Grgain[8][13] = 1.66681635; Grgain[8][14] = 1.927938; Grgain[8][15] = 2.25338554; Grgain[8][16] = 2.71144366;
	Grgain[9][0] = 2.87977815; Grgain[9][1] = 2.42023182; Grgain[9][2] = 2.06020546; Grgain[9][3] = 1.78849733; Grgain[9][4] = 1.567631; Grgain[9][5] = 1.39687121; Grgain[9][6] = 1.27794135; Grgain[9][7] = 1.2006619; Grgain[9][8] = 1.17235661; Grgain[9][9] = 1.19521058; Grgain[9][10] = 1.26618552; Grgain[9][11] = 1.37799537; Grgain[9][12] = 1.53730309; Grgain[9][13] = 1.74319911; Grgain[9][14] = 1.99989152; Grgain[9][15] = 2.34575939; Grgain[9][16] = 2.824801;
	Grgain[10][0] = 3.00670934; Grgain[10][1] = 2.570281; Grgain[10][2] = 2.167367; Grgain[10][3] = 1.89439619; Grgain[10][4] = 1.67329276; Grgain[10][5] = 1.51058328; Grgain[10][6] = 1.39316261; Grgain[10][7] = 1.32073271; Grgain[10][8] = 1.29580414; Grgain[10][9] = 1.31593478; Grgain[10][10] = 1.381983; Grgain[10][11] = 1.49326217; Grgain[10][12] = 1.64755106; Grgain[10][13] = 1.85299551; Grgain[10][14] = 2.108913; Grgain[10][15] = 2.49322033; Grgain[10][16] = 2.96424222;
	Grgain[11][0] = 3.14779449; Grgain[11][1] = 2.760479; Grgain[11][2] = 2.327393; Grgain[11][3] = 2.03506637; Grgain[11][4] = 1.81627846; Grgain[11][5] = 1.65615547; Grgain[11][6] = 1.54346013; Grgain[11][7] = 1.47480285; Grgain[11][8] = 1.45027065; Grgain[11][9] = 1.46974921; Grgain[11][10] = 1.53530085; Grgain[11][11] = 1.64169729; Grgain[11][12] = 1.79380322; Grgain[11][13] = 1.995513; Grgain[11][14] = 2.268367; Grgain[11][15] = 2.69766188; Grgain[11][16] = 3.13380671;
	Grgain[12][0] = 3.27151227; Grgain[12][1] = 2.97488356; Grgain[12][2] = 2.53580165; Grgain[12][3] = 2.202857; Grgain[12][4] = 1.99046743; Grgain[12][5] = 1.83107555; Grgain[12][6] = 1.71881962; Grgain[12][7] = 1.65477848; Grgain[12][8] = 1.630454; Grgain[12][9] = 1.65059066; Grgain[12][10] = 1.71336091; Grgain[12][11] = 1.81254041; Grgain[12][12] = 1.96194947; Grgain[12][13] = 2.165229; Grgain[12][14] = 2.478046; Grgain[12][15] = 2.92654252; Grgain[12][16] = 3.32641459;

	Gbgain[0][0] = 3.25572634; Gbgain[0][1] = 2.89292026; Gbgain[0][2] = 2.4517138; Gbgain[0][3] = 2.14141822; Gbgain[0][4] = 1.91608775; Gbgain[0][5] = 1.75056577; Gbgain[0][6] = 1.6297735; Gbgain[0][7] = 1.55698335; Gbgain[0][8] = 1.53054082; Gbgain[0][9] = 1.55629158; Gbgain[0][10] = 1.62307167; Gbgain[0][11] = 1.74096286; Gbgain[0][12] = 1.90679991; Gbgain[0][13] = 2.12313032; Gbgain[0][14] = 2.435579; Gbgain[0][15] = 2.887383; Gbgain[0][16] = 3.319794;
	Gbgain[1][0] = 3.1139915; Gbgain[1][1] = 2.71121383; Gbgain[1][2] = 2.28292322; Gbgain[1][3] = 2.001553; Gbgain[1][4] = 1.773555; Gbgain[1][5] = 1.60334086; Gbgain[1][6] = 1.47839546; Gbgain[1][7] = 1.40285826; Gbgain[1][8] = 1.37624037; Gbgain[1][9] = 1.39933944; Gbgain[1][10] = 1.46882379; Gbgain[1][11] = 1.58867931; Gbgain[1][12] = 1.75515807; Gbgain[1][13] = 1.97776151; Gbgain[1][14] = 2.256206; Gbgain[1][15] = 2.685162; Gbgain[1][16] = 3.150439;
	Gbgain[2][0] = 2.99251127; Gbgain[2][1] = 2.539546; Gbgain[2][2] = 2.15274119; Gbgain[2][3] = 1.88075721; Gbgain[2][4] = 1.65062714; Gbgain[2][5] = 1.47220039; Gbgain[2][6] = 1.3437233; Gbgain[2][7] = 1.26727808; Gbgain[2][8] = 1.2381109; Gbgain[2][9] = 1.26250851; Gbgain[2][10] = 1.33401537; Gbgain[2][11] = 1.4550693; Gbgain[2][12] = 1.62251461; Gbgain[2][13] = 1.84627759; Gbgain[2][14] = 2.113808; Gbgain[2][15] = 2.500503; Gbgain[2][16] = 2.99223828;
	Gbgain[3][0] = 2.88620067; Gbgain[3][1] = 2.42083025; Gbgain[3][2] = 2.06689; Gbgain[3][3] = 1.792972; Gbgain[3][4] = 1.557844; Gbgain[3][5] = 1.37428069; Gbgain[3][6] = 1.24328971; Gbgain[3][7] = 1.15899968; Gbgain[3][8] = 1.13094807; Gbgain[3][9] = 1.155757; Gbgain[3][10] = 1.23183417; Gbgain[3][11] = 1.35374939; Gbgain[3][12] = 1.52196252; Gbgain[3][13] = 1.74707985; Gbgain[3][14] = 2.01720285; Gbgain[3][15] = 2.36469722; Gbgain[3][16] = 2.85595417;
	Gbgain[4][0] = 2.805475; Gbgain[4][1] = 2.34426785; Gbgain[4][2] = 2.01149154; Gbgain[4][3] = 1.73086941; Gbgain[4][4] = 1.49328375; Gbgain[4][5] = 1.30466914; Gbgain[4][6] = 1.168742; Gbgain[4][7] = 1.08562386; Gbgain[4][8] = 1.05711555; Gbgain[4][9] = 1.07877731; Gbgain[4][10] = 1.155649; Gbgain[4][11] = 1.27911854; Gbgain[4][12] = 1.45085931; Gbgain[4][13] = 1.67629755; Gbgain[4][14] = 1.95046341; Gbgain[4][15] = 2.28564167; Gbgain[4][16] = 2.75199223;
	Gbgain[5][0] = 2.75480866; Gbgain[5][1] = 2.30931067; Gbgain[5][2] = 1.97921109; Gbgain[5][3] = 1.70017242; Gbgain[5][4] = 1.456584; Gbgain[5][5] = 1.266083; Gbgain[5][6] = 1.12949848; Gbgain[5][7] = 1.047226; Gbgain[5][8] = 1.016633; Gbgain[5][9] = 1.03824031; Gbgain[5][10] = 1.11095607; Gbgain[5][11] = 1.23864925; Gbgain[5][12] = 1.40823913; Gbgain[5][13] = 1.63847983; Gbgain[5][14] = 1.9110508; Gbgain[5][15] = 2.23481369; Gbgain[5][16] = 2.68167949;
	Gbgain[6][0] = 2.74190378; Gbgain[6][1] = 2.29949617; Gbgain[6][2] = 1.97337341; Gbgain[6][3] = 1.68797028; Gbgain[6][4] = 1.44639432; Gbgain[6][5] = 1.257866; Gbgain[6][6] = 1.11907864; Gbgain[6][7] = 1.035803; Gbgain[6][8] = 1.0060153; Gbgain[6][9] = 1.02904892; Gbgain[6][10] = 1.10084367; Gbgain[6][11] = 1.2262038; Gbgain[6][12] = 1.39805651; Gbgain[6][13] = 1.6266675; Gbgain[6][14] = 1.89996767; Gbgain[6][15] = 2.22219658; Gbgain[6][16] = 2.66637921;
	Gbgain[7][0] = 2.76845646; Gbgain[7][1] = 2.31891775; Gbgain[7][2] = 1.984939; Gbgain[7][3] = 1.70612419; Gbgain[7][4] = 1.46595716; Gbgain[7][5] = 1.27839935; Gbgain[7][6] = 1.14174712; Gbgain[7][7] = 1.05593383; Gbgain[7][8] = 1.02896273; Gbgain[7][9] = 1.050837; Gbgain[7][10] = 1.12268; Gbgain[7][11] = 1.24922979; Gbgain[7][12] = 1.41899669; Gbgain[7][13] = 1.64322937; Gbgain[7][14] = 1.912983; Gbgain[7][15] = 2.23781657; Gbgain[7][16] = 2.69614768;
	Gbgain[8][0] = 2.8345952; Gbgain[8][1] = 2.37168837; Gbgain[8][2] = 2.02467871; Gbgain[8][3] = 1.74893022; Gbgain[8][4] = 1.50960279; Gbgain[8][5] = 1.325559; Gbgain[8][6] = 1.19417679; Gbgain[8][7] = 1.1083734; Gbgain[8][8] = 1.07775366; Gbgain[8][9] = 1.09952343; Gbgain[8][10] = 1.1761843; Gbgain[8][11] = 1.2994734; Gbgain[8][12] = 1.46681154; Gbgain[8][13] = 1.68724191; Gbgain[8][14] = 1.95761359; Gbgain[8][15] = 2.28763771; Gbgain[8][16] = 2.762187;
	Gbgain[9][0] = 2.93678975; Gbgain[9][1] = 2.462873; Gbgain[9][2] = 2.09207153; Gbgain[9][3] = 1.81134331; Gbgain[9][4] = 1.58088207; Gbgain[9][5] = 1.3999697; Gbgain[9][6] = 1.27424335; Gbgain[9][7] = 1.191744; Gbgain[9][8] = 1.16132; Gbgain[9][9] = 1.18497109; Gbgain[9][10] = 1.25811017; Gbgain[9][11] = 1.37668681; Gbgain[9][12] = 1.54437339; Gbgain[9][13] = 1.75989628; Gbgain[9][14] = 2.024353; Gbgain[9][15] = 2.37870121; Gbgain[9][16] = 2.86460948;
	Gbgain[10][0] = 3.05432677; Gbgain[10][1] = 2.60200286; Gbgain[10][2] = 2.19123387; Gbgain[10][3] = 1.91062164; Gbgain[10][4] = 1.68002653; Gbgain[10][5] = 1.50665259; Gbgain[10][6] = 1.38154352; Gbgain[10][7] = 1.303675; Gbgain[10][8] = 1.27637732; Gbgain[10][9] = 1.29852414; Gbgain[10][10] = 1.36698258; Gbgain[10][11] = 1.4857558; Gbgain[10][12] = 1.64721251; Gbgain[10][13] = 1.8632412; Gbgain[10][14] = 2.12321568; Gbgain[10][15] = 2.51894569; Gbgain[10][16] = 3.000998;
	Gbgain[11][0] = 3.18675113; Gbgain[11][1] = 2.789452; Gbgain[11][2] = 2.345823; Gbgain[11][3] = 2.04380536; Gbgain[11][4] = 1.81311774; Gbgain[11][5] = 1.64452136; Gbgain[11][6] = 1.52401233; Gbgain[11][7] = 1.44757223; Gbgain[11][8] = 1.42193472; Gbgain[11][9] = 1.443355; Gbgain[11][10] = 1.51186883; Gbgain[11][11] = 1.62641156; Gbgain[11][12] = 1.78541982; Gbgain[11][13] = 1.99573874; Gbgain[11][14] = 2.27726436; Gbgain[11][15] = 2.7194345; Gbgain[11][16] = 3.164886;
	Gbgain[12][0] = 3.30671382; Gbgain[12][1] = 2.99426556; Gbgain[12][2] = 2.54958248; Gbgain[12][3] = 2.203216; Gbgain[12][4] = 1.97926247; Gbgain[12][5] = 1.80847907; Gbgain[12][6] = 1.69107234; Gbgain[12][7] = 1.61833107; Gbgain[12][8] = 1.59140134; Gbgain[12][9] = 1.61120772; Gbgain[12][10] = 1.68296742; Gbgain[12][11] = 1.78529263; Gbgain[12][12] = 1.94878912; Gbgain[12][13] = 2.151207; Gbgain[12][14] = 2.4813168; Gbgain[12][15] = 2.93416166; Gbgain[12][16] = 3.34963751;

	Bgain[0][0] = 2.934418; Bgain[0][1] = 2.6059866; Bgain[0][2] = 2.220798; Bgain[0][3] = 1.94021428; Bgain[0][4] = 1.74472368; Bgain[0][5] = 1.600148; Bgain[0][6] = 1.50334871; Bgain[0][7] = 1.44453084; Bgain[0][8] = 1.42655516; Bgain[0][9] = 1.45087564; Bgain[0][10] = 1.51691735; Bgain[0][11] = 1.620021; Bgain[0][12] = 1.77389872; Bgain[0][13] = 1.984156; Bgain[0][14] = 2.26868367; Bgain[0][15] = 2.685018; Bgain[0][16] = 3.086324;
	Bgain[1][0] = 2.780221; Bgain[1][1] = 2.42652345; Bgain[1][2] = 2.055793; Bgain[1][3] = 1.81020319; Bgain[1][4] = 1.61290979; Bgain[1][5] = 1.47168612; Bgain[1][6] = 1.37039435; Bgain[1][7] = 1.312223; Bgain[1][8] = 1.292875; Bgain[1][9] = 1.31676388; Bgain[1][10] = 1.381954; Bgain[1][11] = 1.48782563; Bgain[1][12] = 1.63721418; Bgain[1][13] = 1.83661532; Bgain[1][14] = 2.09502649; Bgain[1][15] = 2.48011971; Bgain[1][16] = 2.89426875;
	Bgain[2][0] = 2.66630936; Bgain[2][1] = 2.266075; Bgain[2][2] = 1.92535543; Bgain[2][3] = 1.69699764; Bgain[2][4] = 1.50311673; Bgain[2][5] = 1.35843492; Bgain[2][6] = 1.26004076; Bgain[2][7] = 1.20359969; Bgain[2][8] = 1.1837939; Bgain[2][9] = 1.20910418; Bgain[2][10] = 1.27213573; Bgain[2][11] = 1.37395608; Bgain[2][12] = 1.51697993; Bgain[2][13] = 1.71529222; Bgain[2][14] = 1.95610869; Bgain[2][15] = 2.29821563; Bgain[2][16] = 2.73281074;
	Bgain[3][0] = 2.56463766; Bgain[3][1] = 2.158125; Bgain[3][2] = 1.849616; Bgain[3][3] = 1.61822569; Bgain[3][4] = 1.42308247; Bgain[3][5] = 1.27978182; Bgain[3][6] = 1.18043447; Bgain[3][7] = 1.12182248; Bgain[3][8] = 1.10311854; Bgain[3][9] = 1.12834954; Bgain[3][10] = 1.19434536; Bgain[3][11] = 1.29403162; Bgain[3][12] = 1.43295217; Bgain[3][13] = 1.6283437; Bgain[3][14] = 1.86529028; Bgain[3][15] = 2.177293; Bgain[3][16] = 2.59969044;
	Bgain[4][0] = 2.49308729; Bgain[4][1] = 2.09290552; Bgain[4][2] = 1.79948926; Bgain[4][3] = 1.56733191; Bgain[4][4] = 1.36901522; Bgain[4][5] = 1.2242583; Bgain[4][6] = 1.12401092; Bgain[4][7] = 1.06499517; Bgain[4][8] = 1.047911; Bgain[4][9] = 1.07009184; Bgain[4][10] = 1.13591647; Bgain[4][11] = 1.23624086; Bgain[4][12] = 1.37226307; Bgain[4][13] = 1.568607; Bgain[4][14] = 1.802117; Bgain[4][15] = 2.0997088; Bgain[4][16] = 2.51218;
	Bgain[5][0] = 2.44614935; Bgain[5][1] = 2.05794668; Bgain[5][2] = 1.77400577; Bgain[5][3] = 1.53863382; Bgain[5][4] = 1.34037089; Bgain[5][5] = 1.19284844; Bgain[5][6] = 1.09375918; Bgain[5][7] = 1.03644383; Bgain[5][8] = 1.01813066; Bgain[5][9] = 1.04016006; Bgain[5][10] = 1.100972; Bgain[5][11] = 1.20247412; Bgain[5][12] = 1.33787072; Bgain[5][13] = 1.534938; Bgain[5][14] = 1.77078509; Bgain[5][15] = 2.0591023; Bgain[5][16] = 2.45250845;
	Bgain[6][0] = 2.45007682; Bgain[6][1] = 2.05073738; Bgain[6][2] = 1.7705425; Bgain[6][3] = 1.53213227; Bgain[6][4] = 1.33331537; Bgain[6][5] = 1.18785942; Bgain[6][6] = 1.08808076; Bgain[6][7] = 1.0271945; Bgain[6][8] = 1.00769913; Bgain[6][9] = 1.03327465; Bgain[6][10] = 1.09398258; Bgain[6][11] = 1.19292164; Bgain[6][12] = 1.33314526; Bgain[6][13] = 1.52600241; Bgain[6][14] = 1.76134288; Bgain[6][15] = 2.0468235; Bgain[6][16] = 2.434896;
	Bgain[7][0] = 2.46845961; Bgain[7][1] = 2.07147646; Bgain[7][2] = 1.78483438; Bgain[7][3] = 1.5487783; Bgain[7][4] = 1.35236824; Bgain[7][5] = 1.206258; Bgain[7][6] = 1.10778213; Bgain[7][7] = 1.04479361; Bgain[7][8] = 1.02893651; Bgain[7][9] = 1.050206; Bgain[7][10] = 1.11128664; Bgain[7][11] = 1.21239; Bgain[7][12] = 1.35215569; Bgain[7][13] = 1.54298; Bgain[7][14] = 1.78045; Bgain[7][15] = 2.06631827; Bgain[7][16] = 2.46595645;
	Bgain[8][0] = 2.53892612; Bgain[8][1] = 2.12474346; Bgain[8][2] = 1.82424378; Bgain[8][3] = 1.59075677; Bgain[8][4] = 1.39276385; Bgain[8][5] = 1.24721909; Bgain[8][6] = 1.14948976; Bgain[8][7] = 1.08533442; Bgain[8][8] = 1.06639385; Bgain[8][9] = 1.08696747; Bgain[8][10] = 1.152703; Bgain[8][11] = 1.25387907; Bgain[8][12] = 1.39364779; Bgain[8][13] = 1.58492756; Bgain[8][14] = 1.8212918; Bgain[8][15] = 2.1178968; Bgain[8][16] = 2.52721024;
	Bgain[9][0] = 2.633789; Bgain[9][1] = 2.21714544; Bgain[9][2] = 1.88529706; Bgain[9][3] = 1.64690328; Bgain[9][4] = 1.45256436; Bgain[9][5] = 1.30626082; Bgain[9][6] = 1.208156; Bgain[9][7] = 1.1472615; Bgain[9][8] = 1.12522137; Bgain[9][9] = 1.14996147; Bgain[9][10] = 1.21406388; Bgain[9][11] = 1.31402481; Bgain[9][12] = 1.45510221; Bgain[9][13] = 1.6442908; Bgain[9][14] = 1.88103974; Bgain[9][15] = 2.19772482; Bgain[9][16] = 2.63523436;
	Bgain[10][0] = 2.75180864; Bgain[10][1] = 2.34643865; Bgain[10][2] = 1.97836983; Bgain[10][3] = 1.73083889; Bgain[10][4] = 1.53518808; Bgain[10][5] = 1.38939548; Bgain[10][6] = 1.289249; Bgain[10][7] = 1.22797668; Bgain[10][8] = 1.2085588; Bgain[10][9] = 1.230665; Bgain[10][10] = 1.29428446; Bgain[10][11] = 1.39603722; Bgain[10][12] = 1.54160321; Bgain[10][13] = 1.731839; Bgain[10][14] = 1.9690479; Bgain[10][15] = 2.323122; Bgain[10][16] = 2.762924;
	Bgain[11][0] = 2.88964343; Bgain[11][1] = 2.5124588; Bgain[11][2] = 2.11312318; Bgain[11][3] = 1.8474673; Bgain[11][4] = 1.64406931; Bgain[11][5] = 1.49958229; Bgain[11][6] = 1.39923692; Bgain[11][7] = 1.33945262; Bgain[11][8] = 1.3191148; Bgain[11][9] = 1.3403734; Bgain[11][10] = 1.40577114; Bgain[11][11] = 1.5079881; Bgain[11][12] = 1.65210688; Bgain[11][13] = 1.84798753; Bgain[11][14] = 2.10569763; Bgain[11][15] = 2.50678158; Bgain[11][16] = 2.92251229;
	Bgain[12][0] = 3.00785327; Bgain[12][1] = 2.7100594; Bgain[12][2] = 2.293494; Bgain[12][3] = 1.98375225; Bgain[12][4] = 1.78258061; Bgain[12][5] = 1.63429224; Bgain[12][6] = 1.53504336; Bgain[12][7] = 1.476219; Bgain[12][8] = 1.45520091; Bgain[12][9] = 1.47230053; Bgain[12][10] = 1.53835821; Bgain[12][11] = 1.63606775; Bgain[12][12] = 1.78734374; Bgain[12][13] = 1.98851943; Bgain[12][14] = 2.287967; Bgain[12][15] = 2.707317; Bgain[12][16] = 3.12480426;
}

ISPResult LensShadingCorrection(int* data, bool enable) {
	ISPResult result = ISPSuccess;
	if (enable == true) {

		float R_lsc[13][17], Gr_lsc[13][17], Gb_lsc[13][17], B_lsc[13][17];
		float** ppR = NULL;
		float** ppGr = NULL;
		float** ppGb = NULL;
		float** ppB = NULL;
		int i, j;

		ppR = (float**)malloc(sizeof(float*) * 13);
		ppGr = (float**)malloc(sizeof(float*) * 13);
		ppGb = (float**)malloc(sizeof(float*) * 13);
		ppB = (float**)malloc(sizeof(float*) * 13);

		for (i = 0; i < 13; i++) {
			ppR[i] = R_lsc[i];
			ppGr[i] = Gr_lsc[i];
			ppGb[i] = Gb_lsc[i];
			ppB[i] = B_lsc[i];
		}

		GenerateLSCGainTable(ppR, ppGr, ppGb, ppB);

		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				if (i % 2 == 0 && j % 2 == 0) {
					data[i * WIDTH + j] = (int)(data[i * WIDTH + j] *
						LSCinterpolation(B_lsc[i * 12 / HEIGHT][j * 16 / WIDTH],
							B_lsc[i * 12 / HEIGHT][j * 16 / WIDTH + 1],
							B_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH],
							B_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH + 1], i, j));
				}
				if (i % 2 == 0 && j % 2 == 1) {
					data[i * WIDTH + j] = (int)(data[i * WIDTH + j] *
						LSCinterpolation(Gb_lsc[i * 12 / HEIGHT][j * 16 / WIDTH],
							Gb_lsc[i * 12 / HEIGHT][j * 16 / WIDTH + 1],
							Gb_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH],
							Gb_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH + 1], i, j));
				}
				if (i % 2 == 1 && j % 2 == 0) {
					data[i * WIDTH + j] = (int)(data[i * WIDTH + j] *
						LSCinterpolation(Gr_lsc[i * 12 / HEIGHT][j * 16 / WIDTH],
							Gr_lsc[i * 12 / HEIGHT][j * 16 / WIDTH + 1],
							Gr_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH],
							Gr_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH + 1], i, j));
				}
				if (i % 2 == 1 && j % 2 == 1) {
					data[i * WIDTH + j] = (int)(data[i * WIDTH + j] *
						LSCinterpolation(R_lsc[i * 12 / HEIGHT][j * 16 / WIDTH],
							R_lsc[i * 12 / HEIGHT][j * 16 / WIDTH + 1],
							R_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH],
							R_lsc[i * 12 / HEIGHT + 1][j * 16 / WIDTH + 1], i, j));
				}
			}
		}
		free(ppR);
		free(ppGr);
		free(ppGb);
		free(ppB);
		cout << " LSC finished" << endl;
	}
	return result;
}




//生成不同类型的小波
void wavelet(const string _wname, Mat& _lowFilter, Mat& _highFilter)
{

	if (_wname == "haar" || _wname == "db1")
	{
		int N = 2;
		_lowFilter = Mat::zeros(1, N, CV_32F);
		_highFilter = Mat::zeros(1, N, CV_32F);

		_lowFilter.at<float>(0, 0) = 1 / sqrtf(N);
		_lowFilter.at<float>(0, 1) = 1 / sqrtf(N);

		_highFilter.at<float>(0, 0) = -1 / sqrtf(N);
		_highFilter.at<float>(0, 1) = 1 / sqrtf(N);
	}
	if (_wname == "sym2")
	{
		int N = 4;
		float h[] = { -0.483, 0.836, -0.224, -0.129 };
		float l[] = { -0.129, 0.224,    0.837, 0.483 };

		_lowFilter = Mat::zeros(1, N, CV_32F);
		_highFilter = Mat::zeros(1, N, CV_32F);

		for (int i = 0; i < N; i++)
		{
			_lowFilter.at<float>(0, i) = l[i];
			_highFilter.at<float>(0, i) = h[i];
		}
	}
}

//小波分解
Mat waveletDecompose(Mat _src, Mat _lowFilter, Mat _highFilter)
{
	//assert(_src.rows == 1 && _lowFilter.rows == 1 && _highFilter.rows == 1);
	//assert(_src.cols >= _lowFilter.cols && _src.cols >= _highFilter.cols);
	Mat& src = _src;

	int D = src.cols;

	Mat& lowFilter = _lowFilter;
	Mat& highFilter = _highFilter;

	//频域滤波或时域卷积；ifft( fft(x) * fft(filter)) = cov(x,filter) 
	Mat dst1 = Mat::zeros(1, D, src.type());
	Mat dst2 = Mat::zeros(1, D, src.type());

	filter2D(src, dst1, -1, lowFilter);
	filter2D(src, dst2, -1, highFilter);

	//下采样
	Mat downDst1 = Mat::zeros(1, D / 2, src.type());
	Mat downDst2 = Mat::zeros(1, D / 2, src.type());

	resize(dst1, downDst1, downDst1.size());
	resize(dst2, downDst2, downDst2.size());

	//数据拼接
	for (int i = 0; i < D / 2; i++)
	{
		src.at<float>(0, i) = downDst1.at<float>(0, i);
		src.at<float>(0, i + D / 2) = downDst2.at<float>(0, i);

	}
	return src;
}

Mat WDT(const Mat& _src, const string _wname, const int _level)
{
	Mat src = Mat_<float>(_src);
	Mat dst = Mat::zeros(src.rows, src.cols, src.type());
	int N = src.rows;
	int D = src.cols;
	//高通低通滤波器
	Mat lowFilter;
	Mat highFilter;
	wavelet(_wname, lowFilter, highFilter);
	//小波变换
	int t = 1;
	int row = N;
	int col = D;
	while (t <= _level)
	{
		//先进行 行小波变换
		for (int i = 0; i < row; i++)
		{
			//取出src中要处理的数据的一行
			Mat oneRow = Mat::zeros(1, col, src.type());
			for (int j = 0; j < col; j++)
			{
				oneRow.at<float>(0, j) = src.at<float>(i, j);
			}
			oneRow = waveletDecompose(oneRow, lowFilter, highFilter);
			for (int j = 0; j < col; j++)
			{
				dst.at<float>(i, j) = oneRow.at<float>(0, j);
			}
		}
		char s[10];
		_itoa_s(t, s, 10);
		//imshow(s, dst);
		//waitKey();
#if 0
		//    normalize(dst,dst,0,255,NORM_MINMAX);
		IplImage dstImg1 = IplImage(dst);
		cvSaveImage("dst1.jpg", &dstImg1);
#endif

		//小波列变换
		for (int j = 0; j < col; j++)
		{
			Mat oneCol = Mat::zeros(row, 1, src.type());
			for (int i = 0; i < row; i++)
			{
				oneCol.at<float>(i, 0) = dst.at<float>(i, j);//dst,not src
			}
			oneCol = (waveletDecompose(oneCol.t(), lowFilter, highFilter)).t();
			for (int i = 0; i < row; i++)
			{
				dst.at<float>(i, j) = oneCol.at<float>(i, 0);
			}
		}
#if 0
		//    normalize(dst,dst,0,255,NORM_MINMAX);
		IplImage dstImg2 = IplImage(dst);
		cvSaveImage("dst2.jpg", &dstImg2);
#endif
		//更新 
		row /= 2;
		col /= 2;
		t++;
		src = dst;

	}
	return dst;
}



Mat getim(Mat src, int depth, int Imgsizey, int Imgsizex, int channel, int strength1, int strength2, int strength3)
{
	int i, j;
	Mat im, im1, im2, im3, im4, im5, im6, temp, im11, im12, im13, im14, imi, imd, imr, tempHL;
	imr = Mat::zeros(src.rows, src.cols, CV_32F);
	imd = Mat::zeros(src.rows, src.cols, CV_32F);
	Mat imL11, imL12, imL13, imL21, imL22, imL23;

	float a, b, c, d;

	im = src.clone(); //Load image in Gray Scale
	imi = Mat::zeros(im.rows, im.cols, CV_8U);
	im.copyTo(imi);

	im.convertTo(im, CV_32F, 1.0, 0.0);

	int redepth = 1;

	//--------------Decomposition-------------------
	while (redepth - 1 < depth) {


		im1 = Mat::zeros(im.rows / 2, im.cols, CV_32F);
		im2 = Mat::zeros(im.rows / 2, im.cols, CV_32F);
		im3 = Mat::zeros(im.rows / 2, im.cols / 2, CV_32F);
		im4 = Mat::zeros(im.rows / 2, im.cols / 2, CV_32F);
		im5 = Mat::zeros(im.rows / 2, im.cols / 2, CV_32F);
		im6 = Mat::zeros(im.rows / 2, im.cols / 2, CV_32F);


		//L1 H and L
		for (int rcnt = 0; rcnt < im.rows; rcnt += 2)
		{
			for (int ccnt = 0; ccnt < im.cols; ccnt++)
			{

				a = im.at<float>(rcnt, ccnt);
				b = im.at<float>(rcnt + 1, ccnt);
				c = (a + b) * 0.707;
				d = (a - b) * 0.707;
				int _rcnt = rcnt / 2;
				im1.at<float>(_rcnt, ccnt) = c;
				im2.at<float>(_rcnt, ccnt) = d;
			}
		}


		//L1 HH and HL
		for (int rcnt = 0; rcnt < im.rows / 2; rcnt++)
		{
			for (int ccnt = 0; ccnt < im.cols; ccnt += 2)
			{

				a = im1.at<float>(rcnt, ccnt);
				b = im1.at<float>(rcnt, ccnt + 1);
				c = (a + b) * 0.707;
				d = (a - b) * 0.707;
				int _ccnt = ccnt / 2;
				im3.at<float>(rcnt, _ccnt) = c;
				im4.at<float>(rcnt, _ccnt) = d;
			}
		}

		//L1 LH and LL
		for (int rcnt = 0; rcnt < im.rows / 2; rcnt++)
		{
			for (int ccnt = 0; ccnt < im.cols; ccnt += 2)
			{

				a = im2.at<float>(rcnt, ccnt);
				b = im2.at<float>(rcnt, ccnt + 1);
				c = (a + b) * 0.707;
				d = (a - b) * 0.707;
				int _ccnt = ccnt / 2;
				im5.at<float>(rcnt, _ccnt) = c;
				im6.at<float>(rcnt, _ccnt) = d;
			}
		}

		for (i = 0; i < src.rows / (pow(2, redepth)); i++) {
			for (j = 0; j < src.cols / (pow(2, redepth)); j++) {
				imd.at<float>(i, j) = im3.at<float>(i, j);
				imd.at<float>(i, j + src.cols / (pow(2, redepth))) = im4.at<float>(i, j);
				imd.at<float>(i + src.rows / (pow(2, redepth)), j) = im5.at<float>(i, j);
				imd.at<float>(i + src.rows / (pow(2, redepth)), j + src.cols / (pow(2, redepth))) = im6.at<float>(i, j);
			}
		}



		im = im3.clone();
		if (redepth == 1) {
			imL11 = im4.clone();
			imL12 = im5.clone();
			imL13 = im6.clone();
		}
		if (redepth == 2) {
			imL21 = im4.clone();
			imL22 = im5.clone();
			imL23 = im6.clone();
		}
		redepth += 1;
	}

	//---------------------------------NR-------------------------------------
	// 高频降噪
	// 1/4 

	for (i = 0; i < imL11.rows; i++) {
		for (j = 0; j < imL11.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength1)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL11.at<float>(i, j) < strength1)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
		}
	}

	for (i = 0; i < imL12.rows; i++) {
		for (j = 0; j < imL12.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength2)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL12.at<float>(i, j) < strength2)
					imL12.at<float>(i, j) = imL12.at<float>(i, j) / 2;
			}
		}
	}

	for (i = 0; i < imL13.rows; i++) {
		for (j = 0; j < imL13.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength3)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL13.at<float>(i, j) < strength3)
					imL13.at<float>(i, j) = imL13.at<float>(i, j) / 5;
			}
		}
	}
	// 1/16

	for (i = 0; i < imL21.rows; i++) {
		for (j = 0; j < imL21.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength1)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL21.at<float>(i, j) < strength1)
					imL21.at<float>(i, j) = imL21.at<float>(i, j) / 2;
			}
		}
	}

	for (i = 0; i < imL22.rows; i++) {
		for (j = 0; j < imL22.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength2)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL21.at<float>(i, j) < strength2)
					imL22.at<float>(i, j) = imL22.at<float>(i, j) / 2;
			}
		}
	}

	for (i = 0; i < imL23.rows; i++) {
		for (j = 0; j < imL23.cols; j++) {
			if (channel == 0) {//Lunma noise
				if (imL11.at<float>(i, j) < strength3)
					imL11.at<float>(i, j) = imL11.at<float>(i, j) / 2;
			}
			else {//Color noise
				if (imL21.at<float>(i, j) < strength3)
					imL23.at<float>(i, j) = imL23.at<float>(i, j) / 5;
			}
		}
	}

	// 低频降噪
	// 1/64
	for (i = 0; i < im.rows; i++) {
		for (j = 0; j < im.cols; j++) {
			if (im6.at<float>(i, j) < strength3)
				im6.at<float>(i, j) = im6.at<float>(i, j) / 5;
		}
	}
	/*
	for (i = 0; i < im.rows; i++) {
		for (j = 0; j < im.cols; j++) {
			if (im5.at<float>(i, j) < 2)
				im5.at<float>(i, j) = im6.at<float>(i, j) / 2;
		}
	}
	for (i = 0; i < im.rows; i++) {
		for (j = 0; j < im.cols; j++) {
			if (im4.at<float>(i, j) < 2)
				im4.at<float>(i, j) = im6.at<float>(i, j) / 2;
		}
	}*/

	//---------------------------------Reconstruction-------------------------------------
	redepth = 1;
	while (redepth - 1 < depth) {

		im11 = Mat::zeros(im.rows, im.cols * 2, CV_32F);
		im12 = Mat::zeros(im.rows, im.cols * 2, CV_32F);
		im13 = Mat::zeros(im.rows, im.cols * 2, CV_32F);
		im14 = Mat::zeros(im.rows, im.cols * 2, CV_32F);

		for (int rcnt = 0; rcnt < im.rows; rcnt++)
		{
			for (int ccnt = 0; ccnt < im.cols; ccnt++)
			{
				int _ccnt = ccnt * 2;
				if (redepth == 1) {
					im11.at<float>(rcnt, _ccnt) = im.at<float>(rcnt, ccnt);     //Upsampling of stage I
					im12.at<float>(rcnt, _ccnt) = im4.at<float>(rcnt, ccnt);
					im13.at<float>(rcnt, _ccnt) = im5.at<float>(rcnt, ccnt);
					im14.at<float>(rcnt, _ccnt) = im6.at<float>(rcnt, ccnt);
				}
				if (redepth == 2) {
					im11.at<float>(rcnt, _ccnt) = im.at<float>(rcnt, ccnt);     //Upsampling of stage I
					im12.at<float>(rcnt, _ccnt) = imL21.at<float>(rcnt, ccnt);
					im13.at<float>(rcnt, _ccnt) = imL22.at<float>(rcnt, ccnt);
					im14.at<float>(rcnt, _ccnt) = imL23.at<float>(rcnt, ccnt);
				}
				if (redepth == 3) {
					im11.at<float>(rcnt, _ccnt) = im.at<float>(rcnt, ccnt);     //Upsampling of stage I
					im12.at<float>(rcnt, _ccnt) = imL11.at<float>(rcnt, ccnt);
					im13.at<float>(rcnt, _ccnt) = imL12.at<float>(rcnt, ccnt);
					im14.at<float>(rcnt, _ccnt) = imL13.at<float>(rcnt, ccnt);
				}
			}
		}

		for (int rcnt = 0; rcnt < im.rows; rcnt++)
		{
			for (int ccnt = 0; ccnt < im.cols * 2; ccnt += 2)
			{
				a = im11.at<float>(rcnt, ccnt);
				b = im12.at<float>(rcnt, ccnt);
				c = (a + b) * 0.707;
				im11.at<float>(rcnt, ccnt) = c;
				d = (a - b) * 0.707;                           //Filtering at Stage I
				im11.at<float>(rcnt, ccnt + 1) = d;
				a = im13.at<float>(rcnt, ccnt);
				b = im14.at<float>(rcnt, ccnt);
				c = (a + b) * 0.707;
				im13.at<float>(rcnt, ccnt) = c;
				d = (a - b) * 0.707;
				im13.at<float>(rcnt, ccnt + 1) = d;
			}
		}

		temp = Mat::zeros(im.rows * 2, im.cols * 2, CV_32F);
		tempHL = Mat::zeros(im.rows * 2, im.cols * 2, CV_32F);

		for (int rcnt = 0; rcnt < im.rows; rcnt++)
		{
			for (int ccnt = 0; ccnt < im.cols * 2; ccnt++)
			{

				int _rcnt = rcnt * 2;
				tempHL.at<float>(_rcnt, ccnt) = im11.at<float>(rcnt, ccnt);     //Upsampling at stage II
				temp.at<float>(_rcnt, ccnt) = im13.at<float>(rcnt, ccnt);
			}
		}

		for (int rcnt = 0; rcnt < im.rows * 2; rcnt += 2)
		{
			for (int ccnt = 0; ccnt < im.cols * 2; ccnt++)
			{

				a = tempHL.at<float>(rcnt, ccnt);
				b = temp.at<float>(rcnt, ccnt);
				c = (a + b) * 0.707;
				tempHL.at<float>(rcnt, ccnt) = c;                                      //Filtering at Stage II
				d = (a - b) * 0.707;
				tempHL.at<float>(rcnt + 1, ccnt) = d;
			}
		}

		im = tempHL.clone();
		redepth += 1;

	}

	imr = im.clone();
	imr.convertTo(imr, CV_8U);
	imd.convertTo(imd, CV_8U);
	//namedWindow("Wavelet Decomposition",0);
	//resizeWindow("Wavelet Decomposition", Imgsizex, Imgsizey);
	//imshow("Wavelet Decomposition", imd);

	Mat dst(HEIGHT, WIDTH, CV_8UC3, Scalar(0, 0, 0));
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			dst.data[i * 3 * WIDTH + 3 * j] = 0;
			dst.data[i * 3 * WIDTH + 3 * j + 1] = imd.data[i * WIDTH + j];
			dst.data[i * 3 * WIDTH + 3 * j + 2] = 0;
		}
	}
	//BYTE *BMPdata = new BYTE[WIDTH * HEIGHT * dst.channels()];
	//setBMP(BMPdata, dst);
	//setBMP(BMPdata, WIDTH, HEIGHT, Rdata, Gdata, Bdata);

	//saveBMP(BMPdata, "C:\\Users\\penghao6\\Desktop\\小波分解.BMP");
	//delete BMPdata;
	return imr;
}

ISPResult SWNR(Mat YUV, int Imgsizey, int Imgsizex, int strength1, int strength2, int strength3, bool enable) {
	ISPResult result = ISPSuccess;
	int i, j;
	Mat onechannel(HEIGHT, WIDTH, CV_8U);
	Mat onechannel2(HEIGHT, WIDTH, CV_8U);

	if (enable == true) {
		//会产生严重格子现象
		//Y通道小波
		//for (i = 0; i < HEIGHT; i++) {
		//	for (j = 0; j < WIDTH; j++) {
		//		onechannel.data[i*WIDTH + j] = YUV.data[i * 3 * WIDTH + 3 * j];
		//	}
		//}
		//onechannel2 = getim(onechannel, 3, Imgsizey, Imgsizex, 0);
		//for (i = 0; i < HEIGHT; i++) {
		//	for (j = 0; j < WIDTH; j++) {
		//		YUV.data[i * 3 * WIDTH + 3 * j ]= onechannel2.data[i*WIDTH + j];
		//		//YUV.data[i * 3 * WIDTH + 3 * j + 1] = onechannel2.data[i*WIDTH + j];
		//		//YUV.data[i * 3 * WIDTH + 3 * j + 2] = onechannel2.data[i*WIDTH + j];
		//	}
		//}

		//U通道小波
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				onechannel.data[i * WIDTH + j] = YUV.data[i * 3 * WIDTH + 3 * j + 1];
			}
		}
		onechannel2 = getim(onechannel, 3, Imgsizey, Imgsizex, 1, strength1, strength2, strength3);
		//onechannel2 = onechannel.clone();
		onechannel = onechannel2.clone();
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				//YUV.data[i * 3 * WIDTH + 3 * j ]= onechannel2.data[i*WIDTH + j];
				YUV.data[i * 3 * WIDTH + 3 * j + 1] = onechannel2.data[i * WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 2] = onechannel2.data[i*WIDTH + j];
			}
		}

		//V通道小波
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				onechannel.data[i * WIDTH + j] = YUV.data[i * 3 * WIDTH + 3 * j + 2];
			}
		}
		onechannel2 = getim(onechannel, 3, Imgsizey, Imgsizex, 2, strength1, strength2, strength3);
		//onechannel2 = onechannel.clone();
		onechannel = onechannel2.clone();
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				//YUV.data[i * 3 * WIDTH + 3 * j ]= onechannel2.data[i*WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 1] = onechannel2.data[i*WIDTH + j];
				YUV.data[i * 3 * WIDTH + 3 * j + 2] = onechannel2.data[i * WIDTH + j];
			}
		}
		cout << " SW finished" << endl;
	}
	return result;
}

ISPResult Sharpness(Mat YUV, bool enable) {
	ISPResult result = ISPSuccess;
	int i, j;
	Mat onechannel(HEIGHT, WIDTH, CV_8U);

	if (enable == true) {

		//NRYUV = YUV.clone();
		//EdgePreservedNR(YUV, NRYUV, 0.7, true);
		//YUV = NRYUV.clone();

		Mat kernel(3, 3, CV_32F, cv::Scalar(0));
		kernel.at<float>(1, 1) = 5.0;
		kernel.at<float>(0, 1) = -1.0;
		kernel.at<float>(1, 0) = -1.0;
		kernel.at<float>(1, 2) = -1.0;
		kernel.at<float>(2, 1) = -1.0;

		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				onechannel.data[i * WIDTH + j] = YUV.data[i * 3 * WIDTH + 3 * j];
			}
		}

		Mat blurried, sharped;
		blurried = onechannel.clone();
		sharped = onechannel.clone();
		bilateralFilter(onechannel, blurried, 5, 11, 15, BORDER_DEFAULT);
		//medianBlur(onechannel, blurried, 5);
		filter2D(blurried, sharped, blurried.depth(), kernel);
		//Canny(sharped, edgedetect, 20, 60, 3, false);
		//Mat Sobel_Mat_x, Sobel_Mat_y;
		//Sobel(sharped, Sobel_Mat_x, sharped.depth(), 1, 0);
		//Sobel(sharped, Sobel_Mat_y, sharped.depth(), 0, 1);
		//convertScaleAbs(Sobel_Mat_x, Sobel_Mat_x);
		//convertScaleAbs(Sobel_Mat_y, Sobel_Mat_y);
		//addWeighted(Sobel_Mat_x, 0.5, Sobel_Mat_y, 0.5, 0, edgedetect);

		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				YUV.data[i * 3 * WIDTH + 3 * j] = sharped.data[i * WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 1] = onechannel2.data[i*WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 2] = onechannel2.data[i*WIDTH + j];
			}
		}
		/*
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				onechannel.data[i*WIDTH + j] = YUV.data[i * 3 * WIDTH + 3 * j];
			}
		}
		fastNlMeansDenoising(onechannel, onechannel2, 3, 3, 9);
		onechannel = onechannel2.clone();

		//filter2D(onechannel, onechannel2, onechannel.depth(), kernel);

		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < WIDTH; j++) {
				YUV.data[i * 3 * WIDTH + 3 * j ]= onechannel2.data[i*WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 1] = onechannel2.data[i*WIDTH + j];
				//YUV.data[i * 3 * WIDTH + 3 * j + 2] = onechannel2.data[i*WIDTH + j];
			}
		}
		*/
		//fastNlMeansDenoisingColored(dst, result, 3, 3, 3, 9);
		cout << " Sharpness finished" << endl;
	}
	return result;
}