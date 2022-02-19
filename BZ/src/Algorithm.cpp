// License: GPL-3.0-or-later
/*
 * BoZhi algorithm functions implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "../include/Algorithm.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo.hpp"

#define DUMP_NEEDED false
#define DUMP_PATH "D:\\test_project\\ISP\\local\\output\\output.txt"

using namespace cv;

/* Bayer Process */
void Lib_BlackLevelCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt= BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BZLoge("pParams is null!");
	}

	if (SUCCESS(rt)) {
		rt = BZ_BlackLevelCorrection(data, pParams, CBs);
		if (!SUCCESS(rt)) {
			BZLoge("Failed! rt:%d", rt);
		}
	}
}

void Lib_LensShadingCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BZLoge("pParams is null!");
	}

	if (SUCCESS(rt)) {
		rt = BZ_LensShadingCorrection(data, pParams, CBs);
		if (!SUCCESS(rt)) {
			BZLoge("Failed! rt:%d", rt);
		}
	}
}

/* RGB Process */
void Lib_Demosaic(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BZLoge("pParams is null!");
	}

	if (SUCCESS(rt)) {
		rt = BZ_Demosaic(data, pParams, CBs);
		if (!SUCCESS(rt)) {
			BZLoge("Failed! rt:%d", rt);
		}
	}
}

void Lib_WhiteBalance(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BZLoge("pParams is null!");
	}

	if (SUCCESS(rt)) {
		rt = BZ_WhiteBalance(data, pParams, CBs);
		if (!SUCCESS(rt)) {
			BZLoge("Failed! rt:%d", rt);
		}
	}
}

void Lib_ColorCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BZLoge("pParams is null!");
	}

	if (SUCCESS(rt)) {
		rt = BZ_ColorCorrection(data, pParams, CBs);
		if (!SUCCESS(rt)) {
			BZLoge("Failed! rt:%d", rt);
		}
	}
}

void Lib_GammaCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BZLoge("pParams is null!");
	}

	if (SUCCESS(rt)) {
		rt = BZ_GammaCorrection(data, pParams, CBs);
		if (!SUCCESS(rt)) {
			BZLoge("Failed! rt:%d", rt);
		}
	}
}

/* YUVProcess */
void Lib_WaveletNR(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BZLoge("pParams is null!");
	}

	if (SUCCESS(rt)) {
		rt = BZ_WaveletNR(data, pParams, CBs);
		if (!SUCCESS(rt)) {
			BZLoge("Failed! rt:%d", rt);
		}
	}
}

void Lib_EdgeEnhancement(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BZLoge("pParams is null!");
	}

	if (SUCCESS(rt)) {
		rt = BZ_EdgeEnhancement(data, pParams, CBs);
		if (!SUCCESS(rt)) {
			BZLoge("Failed! rt:%d", rt);
		}
	}
}

/* CST */
void Lib_CST_RAW2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BZLoge("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = true;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		rt = BZ_CST_RAW2RGB(src, dst, pParams, CBs, enable);
		if (!SUCCESS(rt)) {
			BZLoge("Failed! rt:%d", rt);
		}
	}
}

void Lib_CST_RGB2YUV(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BZLoge("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = true;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		rt = BZ_CST_RGB2YUV(src, dst, pParams, CBs, enable);
		if (!SUCCESS(rt)) {
			BZLoge("Failed! rt:%d", rt);
		}
	}
}

void Lib_CST_YUV2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BZLoge("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = true;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		rt = BZ_CST_YUV2RGB(src, dst, pParams, CBs, enable);
		if (!SUCCESS(rt)) {
			BZLoge("Failed! rt:%d", rt);
		}
	}
}

//EdgePreservedNR not used, it should be redeveloped
/*ISPrt EdgePreservedNR(Mat YUV, Mat NRYUV, float arph, bool enable) {
	ISPrt rt = ISP_SUCCESS;

	if (enable == true) {
		int32_t WIDTH;
		int32_t HEIGHT;
		rt = gParamManager.GetIMGDimension(&WIDTH, &HEIGHT);

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

	return rt;
}*/

//BF not used, it use opencvlib. it should be develop with self alg
/*void BF(unsigned char* b, unsigned char* g, unsigned char* r, int dec, int Colorsigma, int Spacesigma, bool enable)
{
	if (enable == true) {
		int32_t WIDTH;
		int32_t HEIGHT;
		gParamManager.GetIMGDimension(&WIDTH, &HEIGHT);
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
		ISPLogi("finished");
	}
}*/

BZResult BZ_BlackLevelCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data) {
		rt = BZ_INVALID_PARAM;
		BZLoge("data is null! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		int32_t width, height;
		int32_t offset;
		int32_t temp;
		width = pParams->info.width;
		height = pParams->info.height;
		offset = pParams->BLC_param.offset;
		BZLogd("width:%d height:%d", width, height);
		BZLogd("offset:%d", offset);
		for (int32_t i = 0; i < width * height; i++) {
			temp = static_cast<uint16_t*>(data)[i];
			temp -= offset;
			temp = (temp < 0) ? 0 : temp;
			static_cast<uint16_t*>(data)[i] = (uint16_t)temp;
		}
	}

	return rt;
}

float LSCInterpolation(int32_t WIDTH, int32_t HEIGHT,
	float LT, float RT, float LB, float RB,
	int32_t row, int32_t col)
{
	float TempT, TempB, rt;
	TempT = LT - (LT - RT) * (col % (WIDTH / (LSC_LUT_WIDTH - 1))) * (LSC_LUT_WIDTH - 1) / WIDTH;
	TempB = LB - (LB - RB) * (col % (WIDTH / (LSC_LUT_WIDTH - 1))) * (LSC_LUT_WIDTH - 1) / WIDTH;
	rt = TempT - (TempT - TempB) * (row % (HEIGHT / (LSC_LUT_HEIGHT - 1))) * (LSC_LUT_HEIGHT - 1) / HEIGHT;
	return rt;
}

BZResult BZ_LensShadingCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data) {
		rt = BZ_INVALID_PARAM;
		BZLoge("data is null! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		int32_t width, height;
		LIB_BAYER_ORDER bayerOrder = LIB_BAYER_ORDER_NUM;
		int32_t i, j;
		float* pR = nullptr;
		float* pGr = nullptr;
		float* pGb = nullptr;
		float* pB = nullptr;
		float* pLut = nullptr;
		width = pParams->info.width;
		height = pParams->info.height;
		bayerOrder = pParams->info.bayerOrder;
		BZLogd("width:%d height:%d type:%d", width, height, bayerOrder);
		switch (bayerOrder)
		{
		case LIB_RGGB:
			pR = pParams->LSC_param.GainCh1;
			pGr = pParams->LSC_param.GainCh2;
			pGb = pParams->LSC_param.GainCh3;
			pB = pParams->LSC_param.GainCh4;
			break;
		case LIB_BGGR:
			pB = pParams->LSC_param.GainCh1;
			pGb = pParams->LSC_param.GainCh2;
			pGr = pParams->LSC_param.GainCh3;
			pR = pParams->LSC_param.GainCh4;
			break;
		case LIB_GRBG:
			pGr = pParams->LSC_param.GainCh1;
			pR = pParams->LSC_param.GainCh2;
			pB = pParams->LSC_param.GainCh3;
			pGb = pParams->LSC_param.GainCh4;
			break;
		case LIB_GBRG:
			pGb = pParams->LSC_param.GainCh1;
			pB = pParams->LSC_param.GainCh2;
			pR = pParams->LSC_param.GainCh3;
			pGr = pParams->LSC_param.GainCh4;
			break;
		default:
			rt = BZ_INVALID_PARAM;
			BZLoge("Unsupported bayer order:%d rt:%d", bayerOrder, rt);
			break;
		}

		if (SUCCESS(rt)) {
			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					if (i % 2 == 0 && j % 2 == 0) {
						pLut = pB;
					}
					if (i % 2 == 0 && j % 2 == 1) {
						pLut = pGb;
					}
					if (i % 2 == 1 && j % 2 == 0) {
						pLut = pGr;
					}
					if (i % 2 == 1 && j % 2 == 1) {
						pLut = pR;
					}
					static_cast<uint16_t*>(data)[i * width + j] = static_cast<uint16_t*>(data)[i * width + j] *
						LSCInterpolation(width, height,
								*(pLut + (LSC_LUT_HEIGHT - 1) * i / height * LSC_LUT_WIDTH + (LSC_LUT_WIDTH - 1) * j / width),
								*(pLut + (LSC_LUT_HEIGHT - 1) * i / height * LSC_LUT_WIDTH + (LSC_LUT_WIDTH - 1) * j / width + 1),
								*(pLut + ((LSC_LUT_HEIGHT - 1) * i / height + 1) * LSC_LUT_WIDTH + (LSC_LUT_WIDTH - 1) * j / width),
								*(pLut + ((LSC_LUT_HEIGHT - 1) * i / height + 1) * LSC_LUT_WIDTH + (LSC_LUT_WIDTH - 1) * j / width + 1),
								i,
								j);
				}
			}
		}
	}

	return rt;
}

//GCC now is too simple, it should be considered more
/*ISPrt GreenChannelsCorrection(void* gdata, int32_t argNum, ...)
{
	ISPrt rt = ISP_SUCCESS;

	int32_t WIDTH, HEIGHT;
	double GCCWeight;

	rt = gParamManager.GetIMGDimension(&WIDTH, &HEIGHT);
	if (SUCCESS(rt)) {
		rt = gParamManager.GetGCCParam(&GCCWeight);
		if(!SUCCESS(rt)) {
			ISPLoge("get GCCWeight failed. rt:%d", rt);
		}
	}
	else {
		ISPLoge("get IMG Dimension failed. rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		float temp = 1.0;
		for (int32_t i = 0; i < HEIGHT; i++) {
			for (int32_t j = 1; j < WIDTH; j++) {
				if (i % 2 == 0 && j % 2 == 1 && i > 0 &&
					i < HEIGHT - 1 && j > 0 && j < WIDTH - 1) {
					temp = (static_cast<uint16_t*>(gdata)[(i - 1) * WIDTH + j - 1] +
						static_cast<uint16_t*>(gdata)[(i - 1) * WIDTH + j + 1] +
						static_cast<uint16_t*>(gdata)[(i + 1) * WIDTH + j - 1] +
						static_cast<uint16_t*>(gdata)[(i + 1) * WIDTH + j + 1]) << 2;
					temp = (static_cast<uint16_t*>(gdata)[i * WIDTH + j] << 2) - temp / 4.0;
					if (temp >= 0) {
						static_cast<uint16_t*>(gdata)[i * WIDTH + j] =
							((static_cast<uint16_t*>(gdata)[i * WIDTH + j] << 2) - (uint16_t)(temp * GCCWeight)) >> 2 & 0xffff;
					}
					else {
						temp = -temp;
						static_cast<uint16_t*>(gdata)[i * WIDTH + j] =
							((static_cast<uint16_t*>(gdata)[i * WIDTH + j] << 2) + (uint16_t)(temp * GCCWeight)) >> 2 & 0xffff;
					}
				}
			}
		}
		ISPLogi("finished");
	}

	return rt;
}*/

void FirstPixelInsertProcess(uint16_t* src, uint16_t* dst, int32_t width, int32_t height)
{
	int32_t i, j;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (i % 2 == 0 && j % 2 == 1) {
				if (j == width - 1) {
					dst[i * width + j] = src[i * width + j - 1];
				}
				else {
					dst[i * width + j] = (src[i * width + j - 1] +
						src[i * width + j + 1]) / 2;
				}
			}
			if (i % 2 == 1 && j % 2 == 0) {
				if (i == height - 1) {
					dst[i * width + j] = src[(i - 1) * width + j];
				}
				else {
					dst[i * width + j] = (src[(i - 1) * width + j] +
						src[(i + 1) * width + j]) / 2;
				}
			}
		}
	}
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (i % 2 == 1 && j % 2 == 1) {
				if (j < width - 1 && i < height - 1) {
					dst[i * width + j] = (src[(i - 1) * width + j - 1] +
						src[(i - 1) * width + j + 1] +
						src[(i + 1) * width + j - 1] +
						src[(i + 1) * width + j + 1]) / 4;
				}
				else if (i == height - 1 && j < width - 1) {
					dst[i * width + j] = (src[(i - 1) * width + j - 1] +
						src[(i - 1) * width + j + 1]) / 2;
				}
				else if (i < height - 1 && j == width - 1) {
					dst[i * width + j] = (src[(i - 1) * width + j - 1] +
						src[(i + 1) * width + j - 1]) / 2;
				}
				else {
					dst[i * width + j] = src[(i - 1) * width + j - 1];
				}

			}
		}
	}
}

void TwoGPixelInsertProcess(uint16_t* src, uint16_t* dst, int32_t width, int32_t height)
{
	int32_t i, j;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (i == 0 && j == 0) {
				dst[i * width + j] = (src[(i + 1) * width + j] +
					src[i * width + j + 1]) / 2;
			}
			else if (i == 0 && j > 0 && j % 2 == 0) {
				dst[i * width + j] = (src[i * width + j - 1] +
					src[(i + 1) * width + j] +
					src[i * width + j + 1]) / 3;
			}
			else if (i > 0 && j == 0 && i % 2 == 0) {
				dst[i * width + j] = (src[(i - 1) * width + j] +
					src[i * width + j + 1] +
					src[(i + 1) * width + j]) / 3;
			}
			else if (i == height - 1 && j < width - 1 && j % 2 == 1) {
				dst[i * width + j] = (src[i * width + j - 1] +
					src[(i - 1) * width + j] +
					src[i * width + j + 1]) / 3;
			}
			else if (i < height - 1 && j == width - 1 && i % 2 == 1) {
				dst[i * width + j] = (src[(i - 1) * width + j] +
					src[i * width + j - 1] +
					src[(i + 1) * width + j]) / 3;
			}
			else if (i == height - 1 && j == width - 1) {
				dst[i * width + j] = (src[(i - 1) * width + j] +
					src[i * width + j - 1]) / 2;
			}
			else if ((i % 2 == 0 && j % 2 == 0) ||
				(i % 2 == 1 && j % 2 == 1)) {
				dst[i * width + j] = (src[i * width + j - 1] +
					src[i * width + j + 1] +
					src[(i - 1) * width + j] +
					src[(i + 1) * width + j]) / 4;
			}
		}
	}
}

void LastPixelInsertProcess(uint16_t* src, uint16_t* dst , int32_t width, int32_t height) 
{
	int32_t i, j;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (i % 2 == 1 && j % 2 == 0) {
				if (j == 0) {
					dst[i * width + j] = src[i * width + j + 1];
				}
				else {
					dst[i * width + j] = (src[i * width + j - 1] +
						src[i * width + j + 1]) / 2;
				}
			}
			if (i % 2 == 0 && j % 2 == 1) {
				if (i == 0) {
					dst[i * width + j] = src[(i + 1) * width + j];
				}
				else {
					dst[i * width + j] = (src[(i - 1) * width + j] +
						src[(i + 1) * width + j]) / 2;
				}
			}
		}
	}
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (i % 2 == 0 && j % 2 == 0) {
				if (i > 0 && j > 0) {
					dst[i * width + j] = (src[(i - 1) * width + j - 1] +
						src[(i - 1) * width + j + 1] +
						src[(i + 1) * width + j - 1] +
						src[(i + 1) * width + j + 1]) / 4;
				}
				else if (i == 0 && j > 0) {
					dst[i * width + j] = (src[(i + 1) * width + j - 1] +
						src[(i + 1) * width + j + 1]) / 2;
				}
				else if (i > 0 && j == 0) {
					dst[i * width + j] = (src[(i - 1) * width + j + 1] +
						src[(i + 1) * width + j + 1]) / 2;
				}
				else {
					dst[i * width + j] = src[(i + 1) * width + j + 1];
				}
			}
		}
	}
}

BZResult DemosaicInterpolation(uint16_t* pch1, uint16_t* pch2, uint16_t* pch3,
		int32_t width, int32_t height, LIB_BAYER_ORDER order)
{
	BZResult rt = BZ_SUCCESS;

	if (!pch1 || !pch2 || !pch3) {
		rt = BZ_FAILED;
		BZLoge("Invalid input %d", rt);
	}

	if (width % 2 || height % 2) {
		rt = BZ_FAILED;
		BZLoge("Unsupported ord width/height %dx%d", width, height);
	}

	if (SUCCESS(rt)) {
		switch(order) {
			case LIB_RGGB:
			case LIB_BGGR:
				for(int32_t row = 0; row < height; row++) {
					for(int32_t col = 0; col < width; col++) {
						if (row % 2 == 0 && col % 2 == 0) {
							if (row == 0 && col == 0) {
								pch2[row * width + col] = (pch2[(row + 1) * width + col] +
										pch2[row * width + col + 1]) / 2;
							}
							else if (row == 0 && col > 0 && col % 2 == 0) {
								pch2[row * width + col] = (pch2[row * width + col - 1] +
										pch2[(row + 1) * width + col] +
										pch2[row * width + col + 1]) / 3;
							}
							else if (row > 0 && col == 0 && row % 2 == 0) {
								pch2[row * width + col] = (pch2[(row - 1) * width + col] +
										pch2[row * width + col + 1] +
										pch2[(row + 1) * width + col]) / 3;
							}
							else {
								pch2[row * width + col] = (pch2[row * width + col - 1] +
										pch2[row * width + col + 1] +
										pch2[(row - 1) * width + col] +
										pch2[(row + 1) * width + col]) / 4;
							}

							if (row > 0 && col > 0) {
								pch3[row * width + col] = (pch3[(row - 1) * width + col - 1] +
										pch3[(row - 1) * width + col + 1] +
										pch3[(row + 1) * width + col - 1] +
										pch3[(row + 1) * width + col + 1]) / 4;
							}
							else if (row == 0 && col > 0) {
								pch3[row * width + col] = (pch3[(row + 1) * width + col - 1] +
										pch3[(row + 1) * width + col + 1]) / 2;
							}
							else if (row > 0 && col == 0) {
								pch3[row * width + col] = (pch3[(row - 1) * width + col + 1] +
										pch3[(row + 1) * width + col + 1]) / 2;
							}
							else {
								pch3[row * width + col] = pch3[(row + 1) * width + col + 1];
							}
						}
						else if (row % 2 == 1 && col % 2 == 1) {
							if (row == height - 1 && col < width - 1 && col % 2 == 1) {
								pch2[row * width + col] = (pch2[row * width + col - 1] +
										pch2[(row - 1) * width + col] +
										pch2[row * width + col + 1]) / 3;
							}
							else if (row < height - 1 && row % 2 == 1 && col == width - 1) {
								pch2[row * width + col] = (pch2[(row - 1) * width + col] +
										pch2[row * width + col - 1] +
										pch2[(row + 1) * width + col]) / 3;
							}
							else if (row == height - 1 && col  == width - 1) {
								pch2[row * width + col] = (pch2[(row - 1) * width + col] +
										pch2[row * width + col - 1]) / 2;
							}
							else {
								pch2[row * width + col] = (pch2[row * width + col - 1] +
										pch2[row * width + col + 1] +
										pch2[(row - 1) * width + col] +
										pch2[(row + 1) * width + col]) / 4;
							}

							if (row < height - 1 && col < width - 1) {
								pch1[row * width + col] = (pch1[(row - 1) * width + col - 1] +
										pch1[(row - 1) * width + col + 1] +
										pch1[(row + 1) * width + col - 1] +
										pch1[(row + 1) * width + col + 1]) / 4;
							}
							else if (row == height - 1 && col < width - 1) {
								pch1[row * width + col] = (pch1[(row - 1) * width + col - 1] +
										pch1[(row - 1) * width + col + 1]) / 2;
							}
							else if (row < height - 1 && col == width - 1) {
								pch1[row * width + col] = (pch1[(row - 1) * width + col - 1] +
										pch1[(row + 1) * width + col - 1]) / 2;
							}
							else {
								pch1[row * width + col] = pch1[(row - 1) * width + col - 1];
							}
						}
						else if (row % 2 == 0 && col % 2 == 1) {
							if (col == width - 1) {
								pch1[row * width + col] = pch1[row * width + col - 1];
							}
							else {
								pch1[row * width + col] = (pch1[row * width + col - 1] +
										pch1[row * width + col + 1]) / 2;
							}
							if (row == 0) {
								pch3[row * width + col] = pch3[(row + 1) * width + col];
							}
							else {
								pch3[row * width + col] = (pch3[(row - 1) * width + col] +
										pch3[(row + 1) * width + col]) / 2;
							}
						}
						else if (row % 2 == 1 && col % 2 == 0) {
							if (row == height - 1) {
								pch1[row * width + col] = pch1[(row - 1) * width + col];
							}
							else {
								pch1[row * width + col] = (pch1[(row - 1) * width + col] +
										pch1[(row + 1) * width + col]) / 2;
							}
							if (col == 0) {
								pch3[row * width + col] = pch3[row * width + col + 1];
							}
							else {
								pch3[row * width + col] = (pch3[row * width + col - 1] +
										pch3[row * width + col + 1]) / 2;
							}
						}
					}
				}
				break;
			case LIB_GRBG:
			case LIB_GBRG:
				for(int32_t row = 0; row < height; row++) {
					for(int32_t col = 0; col < width; col++) {
						if (row % 2 == 0 && col % 2 == 0) {
							if (col == 0) {
								pch1[row * width + col] = pch1[row * width + col + 1];
							}
							else {
								pch1[row * width + col] = (pch1[row * width + col - 1] +
										pch1[row * width + col + 1]) / 2;
							}
							if (row == 0) {
								pch3[row * width + col] = pch3[(row + 1) * width + col];
							}
							else {
								pch3[row * width + col] = (pch3[(row - 1) * width + col] +
										pch3[(row + 1) * width + col]) / 2;
							}
						}
						else if (row % 2 == 1 && col % 2 == 1) {
							if (row == height - 1) {
								pch1[row * width + col] = pch1[(row - 1) * width + col];
							}
							else {
								pch1[row * width + col] = (pch1[(row - 1) * width + col] +
										pch1[(row + 1) * width + col]) / 2;
							}
							if (col == width - 1) {
								pch3[row * width + col] = pch3[row * width + col - 1];
							}
							else {
								pch3[row * width + col] = (pch3[row * width + col - 1] +
										pch3[row * width + col + 1]) / 2;
							}
						}
						else if (row % 2 == 0 && col % 2 == 1) {
							if (row == 0 && col == width - 1) {
								pch2[row * width + col] = (pch2[(row + 1) * width + col] +
										pch2[row * width + col - 1]) / 2;
							}
							else if (row == 0 && col < width - 1 && col % 2 == 1) {
								pch2[row * width + col] = (pch2[row * width + col - 1] +
										pch2[(row + 1) * width + col] +
										pch2[row * width + col + 1]) / 3;
							}
							else if (row > 0 && row % 2 == 0 && col == width - 1) {
								pch2[row * width + col] = (pch2[(row - 1) * width + col] +
										pch2[row * width + col - 1] +
										pch2[(row + 1) * width + col]) / 3;
							}
							else {
								pch2[row * width + col] = (pch2[row * width + col - 1] +
										pch2[row * width + col + 1] +
										pch2[(row - 1) * width + col] +
										pch2[(row + 1) * width + col]) / 4;
							}

							if (row > 0 && col < width - 1) {
								pch3[row * width + col] = (pch3[(row - 1) * width + col - 1] +
										pch3[(row - 1) * width + col + 1] +
										pch3[(row + 1) * width + col - 1] +
										pch3[(row + 1) * width + col + 1]) / 4;
							}
							else if (row == 0 && col < width - 1) {
								pch3[row * width + col] = (pch3[(row + 1) * width + col - 1] +
										pch3[(row + 1) * width + col + 1])/ 2;
							}
							else if (row > 0 && col == width - 1) {
								pch3[row * width + col] = (pch3[(row - 1) * width + col - 1] +
										pch3[(row + 1) * width + col - 1]) / 2;
							}
							else {
								pch3[row * width + col] = pch3[(row - 1) * width + col - 1];
							}
						}
						else if (row % 2 == 1 && col % 2 == 0) {
							if (row == height - 1 && col > 0 && col % 2 == 0) {
								pch2[row * width + col] = (pch2[row * width + col - 1] +
										pch2[(row - 1) * width + col] +
										pch2[row * width + col + 1]) / 3;
							}
							else if (row < height - 1 && row % 2 == 1 && col == 0) {
								pch2[row * width + col] = (pch2[(row - 1) * width + col] +
										pch2[row * width + col + 1] +
										pch2[(row + 1) * width + col]) / 3;
							}
							else if (row == height - 1 && col  == 0) {
								pch2[row * width + col] = (pch2[(row - 1) * width + col] +
										pch2[row * width + col + 1]) / 2;
							}
							else {
								pch2[row * width + col] = (pch2[row * width + col - 1] +
										pch2[row * width + col + 1] +
										pch2[(row - 1) * width + col] +
										pch2[(row + 1) * width + col]) / 4;
							}

							if (row < height - 1 && col > 0) {
								pch1[row * width + col] = (pch1[(row - 1) * width + col - 1] +
										pch1[(row - 1) * width + col + 1] +
										pch1[(row + 1) * width + col - 1] +
										pch1[(row + 1) * width + col + 1]) / 4;
							}
							else if (row == height - 1 && col > 0) {
								pch1[row * width + col] = (pch1[(row - 1) * width + col - 1] +
										pch1[(row - 1) * width + col + 1]) / 2;
							}
							else if (row < height - 1 && col == 0) {
								pch1[row * width + col] = (pch1[(row - 1) * width + col + 1] +
										pch1[(row + 1) * width + col + 1]) / 2;
							}
							else {
								pch1[row * width + col] = pch1[(row + 1) * width + col + 1];
							}
						}
					}
				}
				break;
			case LIB_BAYER_ORDER_NUM:
			default:
				BZLoge("Unsupported bayer order:%d", rt);
				break;
		}
	}

	return rt;
}

BZResult BZ_Demosaic(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data) {
		rt = BZ_INVALID_PARAM;
		BZLoge("data is null! rt:%d", rt);
	}

	int32_t width, height;
	LIB_BAYER_ORDER bayerOrder = LIB_BAYER_ORDER_NUM;
	width = pParams->info.width;
	height = pParams->info.height;
	bayerOrder = pParams->info.bayerOrder;
	BZLogd("width:%d height:%d type:%d", width, height, bayerOrder);
	uint16_t* pChannel1 = nullptr;
	uint16_t* pChannel2 = nullptr;
	uint16_t* pChannel3 = nullptr;
	switch (bayerOrder) {
	case LIB_RGGB:
	case LIB_GRBG:
		pChannel3 = static_cast<uint16_t*>(data);
		pChannel2 = static_cast<uint16_t*>(data) + width * height;
		pChannel1 = static_cast<uint16_t*>(data) + 2 * width * height;
		break;
	case LIB_BGGR:
	case LIB_GBRG:
		pChannel1 = static_cast<uint16_t*>(data);
		pChannel2 = static_cast<uint16_t*>(data) + width * height;
		pChannel3 = static_cast<uint16_t*>(data) + 2 * width * height;
		break;
	default:
		rt = BZ_INVALID_PARAM;
		BZLoge("Unsupported bayer order:%d rt:%d", bayerOrder, rt);
		break;
	}
	if (SUCCESS(rt)) {
		rt = DemosaicInterpolation(pChannel1, pChannel2, pChannel3,
				width, height, bayerOrder);
//		FirstPixelInsertProcess(pChannel1, pChannel1, width, height);
//		TwoGPixelInsertProcess(pChannel2, pChannel2, width, height);
//		LastPixelInsertProcess(pChannel3, pChannel3, width, height);
	}

	return rt;
}

BZResult BZ_WhiteBalance(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data) {
		rt = BZ_INVALID_PARAM;
		BZLoge("data is null! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		int32_t width, height;
		float rGain, gGain, bGain;
		width = pParams->info.width;
		height = pParams->info.height;
		rGain = pParams->WB_param.rGain;
		gGain = pParams->WB_param.gGain;
		bGain = pParams->WB_param.bGain;
		BZLogd("width:%d height:%d", width, height);
		BZLogd("Gain(r/g/b):%f %f %f", rGain, gGain, bGain);

		uint16_t* B = static_cast<uint16_t*>(data);
		uint16_t* G = B + width * height;
		uint16_t* R = G + width * height;

		if (SUCCESS(rt)) {
			for (int i = 0; i < width * height; i++)
			{
				B[i] = B[i] * bGain;
				G[i] = G[i] * gGain;
				R[i] = R[i] * rGain;
			}
		}
	}

	return rt;
}

BZResult BZ_ColorCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data) {
		rt = BZ_INVALID_PARAM;
		BZLoge("data is null! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		int32_t width, height;
		float* pCcm;
		int32_t i, j;
		width = pParams->info.width;
		height = pParams->info.height;
		pCcm = pParams->CC_param.CCM;
		BZLogd("width:%d height:%d", width, height);
		if (width && height) {
			for (i = 0; i < CCM_HEIGHT; i++) {
				BZLogd("CCM[%d]:%f CCM[%d]:%f CCM[%d]:%f", i * CCM_WIDTH, pCcm[i * CCM_WIDTH],
						i * CCM_WIDTH + 1, pCcm[i * CCM_WIDTH + 1],
						i * CCM_WIDTH + 2, pCcm[i * CCM_WIDTH + 2]);
			}

			Mat CCM = Mat::zeros(3, 3, CV_32FC1);
			for (i = 0; i < 3; i++) {
				for (j = 0; j < 3; j++) {
					CCM.at<float>(i, j) = *(pCcm + i * CCM_WIDTH + j);
				}
			}

			uint16_t* B = static_cast<uint16_t*>(data);
			uint16_t* G = B + width * height;
			uint16_t* R = G + width * height;

			Mat outMatric;
			Mat inMatric(width * height, 3, CV_32FC1);

			for (i = 0; i < height * width; i++) {
				inMatric.at<float>(i, 0) = (float)B[i];
				inMatric.at<float>(i, 1) = (float)G[i];
				inMatric.at<float>(i, 2) = (float)R[i];
			}

			//ofstream OutFile2("C:\\Users\\penghao6\\Desktop\\变形.txt");
			//OutFile2 << inMatric;
			//OutFile2.close();

			outMatric = inMatric * CCM;

			//ofstream OutFile3("C:\\Users\\penghao6\\Desktop\\计算结果.txt");
			//OutFile3 << outMatric;
			//OutFile3.close();

			for (i = 0; i < height * width; i++) {
				if (outMatric.at<float>(i, 0) > 1023)
					outMatric.at<float>(i, 0) = 1023;
				if (outMatric.at<float>(i, 0) < 0)
					outMatric.at<float>(i, 0) = 0;
				if (outMatric.at<float>(i, 1) > 1023)
					outMatric.at<float>(i, 1) = 1023;
				if (outMatric.at<float>(i, 1) < 0)
					outMatric.at<float>(i, 1) = 0;
				if (outMatric.at<float>(i, 2) > 1023)
					outMatric.at<float>(i, 2) = 1023;
				if (outMatric.at<float>(i, 2) < 0)
					outMatric.at<float>(i, 2) = 0;
				B[i] = (uint16_t)outMatric.at<float>(i, 0);
				G[i] = (uint16_t)outMatric.at<float>(i, 1);
				R[i] = (uint16_t)outMatric.at<float>(i, 2);
			}

			//ofstream OutFile4("C:\\Users\\penghao6\\Desktop\\反变形.txt");
			//OutFile4 << Ori;
			//OutFile4.close();

			//ofstream OutFile5("C:\\Users\\penghao6\\Desktop\\反归一化.txt");
			//OutFile5 <<income;
			//OutFile5.close();
			//CharDataSaveAsText(dst.data, "C:\\Users\\penghao6\\Desktop\\output2.txt");
		}
	}
	return rt;
}

BZResult BZ_GammaCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data) {
		rt = BZ_INVALID_PARAM;
		BZLoge("data is null! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		int32_t width, height;
		uint16_t* plut = nullptr;
		width = pParams->info.width;
		height = pParams->info.height;
		plut = pParams->Gamma_param.lut;
		BZLogd("width:%d height:%d", width, height);

		uint16_t* B = static_cast<uint16_t*>(data);
		uint16_t* G = B + width * height;
		uint16_t* R = G + width * height;

		for (int32_t i = 0; i < width * height; i++)
		{
			B[i] = plut[B[i]];
			G[i] = plut[G[i]];
			R[i] = plut[R[i]];
		}
	}

	return rt;
}

enum WAVELET_TYPE {
	haar = 0,
	dbl,
	sym2
};

//生成不同类型的小波
void wavelet(WAVELET_TYPE type, Mat& _lowFilter, Mat& _highFilter)
{
	if (type == haar || type == dbl)
	{
		int N = 2;
		_lowFilter = Mat::zeros(1, N, CV_32F);
		_highFilter = Mat::zeros(1, N, CV_32F);

		_lowFilter.at<float>(0, 0) = 1 / sqrtf(N);
		_lowFilter.at<float>(0, 1) = 1 / sqrtf(N);

		_highFilter.at<float>(0, 0) = -1 / sqrtf(N);
		_highFilter.at<float>(0, 1) = 1 / sqrtf(N);
	}
	if (type == sym2)
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

Mat WDT(const Mat& _src, WAVELET_TYPE type, const int _level)
{
	Mat src = Mat_<float>(_src);
	Mat dst = Mat::zeros(src.rows, src.cols, src.type());
	int N = src.rows;
	int D = src.cols;
	//高通低通滤波器
	Mat lowFilter;
	Mat highFilter;
	wavelet(type, lowFilter, highFilter);
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
		//char s[10];
		//_itoa_s(t, s, 10);
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



Mat getim(Mat src, int32_t WIDTH, int32_t HEIGHT,
	int depth, int channel,
	int strength1, int strength2, int strength3)
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

BZResult BZ_WaveletNR(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data) {
		rt = BZ_INVALID_PARAM;
		BZLoge("data is null! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		int32_t width, height;
		int32_t strength1;
		int32_t strength2;
		int32_t strength3;
		width = pParams->info.width;
		height = pParams->info.height;
		strength1 = pParams->WNR_param.L1_threshold;
		strength2 = pParams->WNR_param.L2_threshold;
		strength3 = pParams->WNR_param.L3_threshold;
		BZLogd("width:%d height:%d", width, height);
		BZLogd("strength:%d %d %d", strength1, strength2, strength3);
		if (width && height) {
			int32_t i, j;
			Mat onechannel(height, width, CV_8U);
			Mat onechannel2(height, width, CV_8U);

			if (SUCCESS(rt)) {
				//会产生严重格子现象
				//Y通道小波
				/*for (i = 0; i < height; i++) {
				  for (j = 0; j < width; j++) {
				  onechannel.data[i*width + j] = static_cast<uchar*>(data)[i * 3 * width + 3 * j];
				  }
				  }
				  onechannel2 = getim(onechannel, width, height, 3, Imgsizey, Imgsizex, 1, 50, 30, 20);
				  for (i = 0; i < height; i++) {
				  for (j = 0; j < width; j++) {
				  static_cast<uchar*>(data)[i * 3 * width + 3 * j ]= onechannel2.data[i*width + j];
				//YUV.data[i * 3 * width + 3 * j + 1] = onechannel2.data[i*width + j];
				//YUV.data[i * 3 * width + 3 * j + 2] = onechannel2.data[i*width + j];
				}
				}*/

				//U通道小波
				for (i = 0; i < height; i++) {
					for (j = 0; j < width; j++) {
						onechannel.data[i * width + j] = static_cast<uchar*>(data)[i * 3 * width + 3 * j + 1];
					}
				}
				onechannel2 = getim(onechannel, width, height, 3, 1, strength1, strength2, strength3);
				//onechannel2 = onechannel.clone();
				onechannel = onechannel2.clone();
				for (i = 0; i < height; i++) {
					for (j = 0; j < width; j++) {
						//YUV.data[i * 3 * width + 3 * j ]= onechannel2.data[i*width + j];
						static_cast<uchar*>(data)[i * 3 * width + 3 * j + 1] = onechannel2.data[i * width + j];
						//YUV.data[i * 3 * width + 3 * j + 2] = onechannel2.data[i*width + j];
					}
				}

				//V通道小波
				for (i = 0; i < height; i++) {
					for (j = 0; j < width; j++) {
						onechannel.data[i * width + j] = static_cast<uchar*>(data)[i * 3 * width + 3 * j + 2];
					}
				}
				onechannel2 = getim(onechannel, width, height, 3, 2, strength1, strength2, strength3);
				//onechannel2 = onechannel.clone();
				onechannel = onechannel2.clone();
				for (i = 0; i < height; i++) {
					for (j = 0; j < width; j++) {
						//YUV.data[i * 3 * width + 3 * j ]= onechannel2.data[i*width + j];
						//YUV.data[i * 3 * width + 3 * j + 1] = onechannel2.data[i*width + j];
						static_cast<uchar*>(data)[i * 3 * width + 3 * j + 2] = onechannel2.data[i * width + j];
					}
				}
			}
		}
	}
	return rt;
}


BZResult BZ_EdgeEnhancement(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data) {
		rt = BZ_INVALID_PARAM;
		BZLoge("data is null! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		int32_t width, height;
		float alpha;
		int32_t coreSzie;
		int32_t delta;
		width = pParams->info.width;
		height = pParams->info.height;
		alpha = pParams->EE_param.alpha;
		coreSzie = pParams->EE_param.coreSize;
		delta = pParams->EE_param.delta;
		BZLogd("width:%d height:%d", width, height);
		BZLogd("alpha:%f coreSzie:%d delta:%d", alpha, coreSzie, delta);
		if (width && height) {
			Mat blurred;
			Mat Y(height, width, CV_8UC1, Scalar(0));
			int32_t i, j;
			int32_t mask;
			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					Y.data[i * width + j] = static_cast<uchar*>(data)[i * 3 * width + 3 * j];
				}
			}

			GaussianBlur(Y, blurred, Size(coreSzie, coreSzie), delta, delta);

			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					mask = Y.data[i * width + j] - blurred.data[i * width + j];
					mask = (int)(static_cast<uchar*>(data)[i * 3 * width + 3 * j] + (int)(alpha * mask));
					if (mask > 255) {
						mask = 255;
					}
					else if (mask < 0) {
						mask = 0;
					}
					static_cast<uchar*>(data)[i * 3 * width + 3 * j] = mask & 255;
				}
			}
		}
	}

	return rt;
}

BZResult ReadChannels(uint16_t* data, uint16_t* pChannel1, uint16_t* pChannel2, uint16_t* pChannel3,
		int32_t width, int32_t height, LIB_BAYER_ORDER order)
{
	BZResult rt = BZ_SUCCESS;
	int32_t i, j;

	if (!data || !pChannel1 || !pChannel2 || !pChannel3 || width < 0 || height < 0) {
		rt = BZ_INVALID_PARAM;
	}

	if (SUCCESS(rt)) {
		switch (order) {
			case LIB_RGGB:
			case LIB_BGGR:
				for (i = 0; i < height; i++) {
					for (j = 0; j < width; j++) {
						if (i % 2 == 0 && j % 2 == 0) {
							pChannel1[i * width + j] = data[i * width + j];
							pChannel2[i * width + j] = 0;
							pChannel3[i * width + j] = 0;
						}
						else if ((i % 2 == 0 && j % 2 == 1) || (i % 2 == 1 && j % 2 == 0)) {
							pChannel1[i * width + j] = 0;
							pChannel2[i * width + j] = data[i * width + j];
							pChannel3[i * width + j] = 0;
						}
						else {
							pChannel1[i * width + j] = 0;
							pChannel2[i * width + j] = 0;
							pChannel3[i * width + j] = data[i * width + j];
						}
					}
				}
				break;
			case LIB_GRBG:
			case LIB_GBRG:
				for (i = 0; i < height; i++) {
					for (j = 0; j < width; j++) {
						if (i % 2 == 0 && j % 2 == 1) {
							pChannel1[i * width + j] = data[i * width + j];
							pChannel2[i * width + j] = 0;
							pChannel3[i * width + j] = 0;
						}
						else if ((i % 2 == 0 && j % 2 == 0) || (i % 2 == 1 && j % 2 == 1)) {
							pChannel1[i * width + j] = 0;
							pChannel2[i * width + j] = data[i * width + j];
							pChannel3[i * width + j] = 0;
						}
						else {
							pChannel1[i * width + j] = 0;
							pChannel2[i * width + j] = 0;
							pChannel3[i * width + j] = data[i * width + j];
						}
					}
				}
				break;
			case LIB_BAYER_ORDER_NUM:
			default:
				BZLoge("Not support bayer order:%d", order);
				rt = BZ_FAILED;
				break;
		}
	}

	return rt;
}

BZResult BZ_CST_RAW2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!src || !dst) {
		rt = BZ_INVALID_PARAM;
		BZLoge("data is null! rt:%d", rt);
	}

	int32_t width, height;
	LIB_BAYER_ORDER bayerOrder = LIB_BAYER_ORDER_NUM;
	width = pParams->info.width;
	height = pParams->info.height;
	bayerOrder = pParams->info.bayerOrder;
	BZLogd("width:%d height:%d type:%d", width, height, bayerOrder);
	uint16_t* pChannel1 = nullptr;
	uint16_t* pChannel2 = nullptr;
	uint16_t* pChannel3 = nullptr;
	switch (bayerOrder) {
	case LIB_RGGB:
	case LIB_GRBG:
		pChannel3 = static_cast<uint16_t*>(dst);
		pChannel2 = static_cast<uint16_t*>(dst) + width * height;
		pChannel1 = static_cast<uint16_t*>(dst) + 2 * width * height;
		break;
	case LIB_GBRG:
	case LIB_BGGR:
		pChannel1 = static_cast<uint16_t*>(dst);
		pChannel2 = static_cast<uint16_t*>(dst) + width * height;
		pChannel3 = static_cast<uint16_t*>(dst) + 2 * width * height;
		break;
	default:
		rt = BZ_INVALID_PARAM;
		BZLoge("Unsupported bayer order:%d rt:%d", bayerOrder, rt);
		break;
	}

	if (SUCCESS(rt)) {
		rt = ReadChannels(static_cast<uint16_t*>(src), pChannel1, pChannel2, pChannel3,
				width, height, bayerOrder);
	}

#if  DUMP_NEEDED
	if (SUCCESS(rt)) {
		DumpImgDataAsText((void*)gData, height, width, sizeof(uint16_t), DUMP_PATH);
	}
#endif

	return rt;
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

BZResult BZ_CST_RGB2YUV(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!src || !dst) {
		rt = BZ_INVALID_PARAM;
		BZLoge("data is null! rt:%d", rt);
	}

	int32_t width, height;
	bool enable = true;
	width = pParams->info.width;
	height = pParams->info.height;
	BZLogd("width:%d height:%d", width, height);
	if (width && height) {
		va_list va_param;
		va_start(va_param, CBs);
		enable = static_cast<bool>(va_arg(va_param, int32_t));
		va_end(va_param);
		BZLogd("enable:%d", enable);
		uint16_t* bSrc = static_cast<uint16_t*>(src);
		uint16_t* gSrc = static_cast<uint16_t*>(src) + width * height;
		uint16_t* rSrc = static_cast<uint16_t*>(src) + 2 * width * height;
		uint8_t* data8bit = nullptr;
		uint8_t* b8bit = nullptr;
		uint8_t* g8bit = nullptr;
		uint8_t* r8bit = nullptr;

		if (SUCCESS(rt)) {
			data8bit = new uint8_t[3 * width * height];
			if (data8bit) {
				memset(data8bit, 0x0, 3 * width * height);
				b8bit = data8bit;
				g8bit = b8bit + width * height;
				r8bit = g8bit + width * height;
				Compress10to8(bSrc, b8bit, width * height, true);
				Compress10to8(gSrc, g8bit, width * height, true);
				Compress10to8(rSrc, r8bit, width * height, true);
			}
			else {
				rt = BZ_MEMORY_ERROR;
				BZLoge("cannot new buffer for 8 bits data! rt:%d", rt);
			}
		}

		if (SUCCESS(rt)) {
			int32_t i, j;
			Mat YUVdst(height, width, CV_8UC3, Scalar(0, 0, 0));
			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					YUVdst.data[i * width * 3 + 3 * j] = (unsigned int)b8bit[i * width + j];
					YUVdst.data[i * width * 3 + 3 * j + 1] = (unsigned int)g8bit[i * width + j];
					YUVdst.data[i * width * 3 + 3 * j + 2] = (unsigned int)r8bit[i * width + j];
				}
			}
			if (enable) {
				cvtColor(YUVdst, YUVdst, COLOR_BGR2YCrCb, 0);
			}
			memcpy(dst, YUVdst.data, 3 * width * height);
			YUVdst.release();
		}

		if (data8bit) {
			delete data8bit;
		}
	}

	return rt;
}

BZResult BZ_CST_YUV2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!src) {
		rt = BZ_INVALID_PARAM;
		BZLoge("data is null! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		int32_t width, height;
		width = pParams->info.width;
		height = pParams->info.height;
		BZLogd("width:%d height:%d", width, height);
		if (width && height) {
			Mat container(height, width, CV_8UC3, Scalar(0, 0, 0));
			memcpy(container.data, src, width * height * container.channels());
			cvtColor(container, container, COLOR_YCrCb2BGR, 0);
			memcpy(dst, container.data, width * height * container.channels());
		}
	}

	return rt;
}

