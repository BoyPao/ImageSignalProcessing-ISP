// License: GPL-3.0-or-later
/*
 * BoZhi algorithm functions implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "Algorithm.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo.hpp"

#define DBG_SHOW_ON false
#define DBG_DUMP_ON false
#ifdef LINUX_SYSTEM
#define DUMP_PATH "~/HAO/test_project/ISP/ISP/res/out/output.txt"
#elif defined WIN32_SYSTEM
#define DUMP_PATH "D:\\test_project\\ISP\\local\\output\\output.txt"
#endif

using namespace cv;

#define VMASK_UCHAR(v) ((v) & 0xff)
#define VFIX_UCHAR(v) ((v) > 255 ? 255 : ((v < 0) ? 0 : (v)))

/* Bayer Process */
BZResult BZ_BlackLevelCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_LensShadingCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

/* RGB Process */
BZResult BZ_Demosaic(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_WhiteBalance(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_ColorCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_GammaCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

/* YUVProcess */
BZResult BZ_WaveletNR(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_EdgeEnhancement(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

/* CST */
BZResult BZ_CST_RAW2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_CST_RGB2YUV(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_CST_YUV2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

/* Bayer Process */
void WrapBlackLevelCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt= BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = false;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		if (enable) {
			rt = BZ_BlackLevelCorrection(data, pParams, CBs);
			if (!SUCCESS(rt)) {
				BLOGE("Failed! rt:%d", rt);
			}
		}
	}
}

void WrapLensShadingCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = false;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		if (enable) {
			rt = BZ_LensShadingCorrection(data, pParams, CBs);
			if (!SUCCESS(rt)) {
				BLOGE("Failed! rt:%d", rt);
			}
		}
	}
}

/* RGB Process */
void WrapDemosaic(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = false;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		if (enable) {
			rt = BZ_Demosaic(data, pParams, CBs);
			if (!SUCCESS(rt)) {
				BLOGE("Failed! rt:%d", rt);
			}
		}
	}
}

void WrapWhiteBalance(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = false;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		if (enable) {
			rt = BZ_WhiteBalance(data, pParams, CBs);
			if (!SUCCESS(rt)) {
				BLOGE("Failed! rt:%d", rt);
			}
		}
	}
}

void WrapColorCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = false;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		if (enable) {
			rt = BZ_ColorCorrection(data, pParams, CBs);
			if (!SUCCESS(rt)) {
				BLOGE("Failed! rt:%d", rt);
			}
		}
	}
}

void WrapGammaCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = false;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		if (enable) {
			rt = BZ_GammaCorrection(data, pParams, CBs);
			if (!SUCCESS(rt)) {
				BLOGE("Failed! rt:%d", rt);
			}
		}
	}
}

/* YUVProcess */
void WrapWaveletNR(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = false;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		if (enable) {
			rt = BZ_WaveletNR(data, pParams, CBs);
			if (!SUCCESS(rt)) {
				BLOGE("Failed! rt:%d", rt);
			}
		}
	}
}

void WrapEdgeEnhancement(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = false;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		if (enable) {
			rt = BZ_EdgeEnhancement(data, pParams, CBs);
			if (!SUCCESS(rt)) {
				BLOGE("Failed! rt:%d", rt);
			}
		}
	}
}

/* CST */
void WrapCST_RAW2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = true;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		rt = BZ_CST_RAW2RGB(src, dst, pParams, CBs, enable);
		if (!SUCCESS(rt)) {
			BLOGE("Failed! rt:%d", rt);
		}
	}
}

void WrapCST_RGB2YUV(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = true;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		rt = BZ_CST_RGB2YUV(src, dst, pParams, CBs, enable);
		if (!SUCCESS(rt)) {
			BLOGE("Failed! rt:%d", rt);
		}
	}
}

void WrapCST_YUV2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("pParams is null!");
	}

	if (SUCCESS(rt)) {
		bool enable = true;
		va_list va;
		va_start(va, CBs);
		enable = static_cast<bool>(va_arg(va, int32_t));
		va_end(va);
		rt = BZ_CST_YUV2RGB(src, dst, pParams, CBs, enable);
		if (!SUCCESS(rt)) {
			BLOGE("Failed! rt:%d", rt);
		}
	}
}

enum PixelDataType {
	DT_UCHAR = 0,
	DT_FLOAT,
	DT_NUM
};

void WrapIMGShow(void* pData, int32_t w, int32_t h, int32_t chNum, PixelDataType DT)
{
	int32_t type = chNum == 1 ? CV_8U: CV_8UC3;

	if (!pData) {
		BLOGE("Input is null");
		return;
	}

	if (chNum != 1 && chNum != 3) {
		BLOGE("Invalid ch number:%d", chNum);
		return;
	}

	if (DT >= DT_NUM) {
		BLOGE("Invalid data type:%d", DT);
		return;
	}

	Mat img(h, w, type);
	for (int32_t i = 0; i < w * h; i++) {
		if (chNum == 1) {
			if (DT == DT_UCHAR) {
				img.data[i] = static_cast<uchar*>(pData)[i];
			} else if (DT == DT_FLOAT) {
				img.data[i] = (int)static_cast<float*>(pData)[i];
			}
		} else if (chNum == 3) {
			if (DT == DT_UCHAR) {
				img.data[i * 3] = static_cast<uchar*>(pData)[i];
				img.data[i * 3 + 1] = static_cast<uchar*>(pData)[w * h + i];
				img.data[i * 3 + 2] = static_cast<uchar*>(pData)[w * h * 2 + i];
			} else if (DT == DT_FLOAT) {
				img.data[i * 3] = (int)static_cast<float*>(pData)[i];
				img.data[i * 3 + 1] = (int)static_cast<float*>(pData)[w * h + i];
				img.data[i * 3 + 2] = (int)static_cast<float*>(pData)[w * h * 2 + i];
			}
		}
	}

	namedWindow("Debug", 0);
	resizeWindow("Debug", 640, 480);
	imshow("Debug", img);
	waitKey(0); /* for the possibility of interacting with window, keep the value as 0 */
}

BZResult BZ_BlackLevelCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data || !pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		int32_t width, height;
		int32_t offset;
		int32_t temp;
		width = pParams->info.width;
		height = pParams->info.height;
		offset = pParams->BLC_param.offset;
		BLOGDA("width:%d height:%d", width, height);
		BLOGDA("offset:%d", offset);
		for (int32_t i = 0; i < width * height; i++) {
			temp = static_cast<uint16_t*>(data)[i];
			temp -= offset;
			temp = (temp < 0) ? 0 : temp;
			static_cast<uint16_t*>(data)[i] = (uint16_t)temp;
		}
	}

	return rt;
}

float LSCInterpolation(int32_t w, int32_t h,
	float vLT, float vRT, float vLB, float vRB,
	int32_t row, int32_t col)
{
	float vTempT, vTempB, rt;
	vTempT = vLT - (vLT - vRT) * (col % (w / (LSC_LUT_WIDTH - 1))) * (LSC_LUT_WIDTH - 1) / w;
	vTempB = vLB - (vLB - vRB) * (col % (w / (LSC_LUT_WIDTH - 1))) * (LSC_LUT_WIDTH - 1) / w;
	rt = vTempT - (vTempT - vTempB) * (row % (h / (LSC_LUT_HEIGHT - 1))) * (LSC_LUT_HEIGHT - 1) / h;
	return rt;
}

BZResult BZ_LensShadingCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data || !pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input! rt:%d", rt);
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
		BLOGDA("width:%d height:%d type:%d", width, height, bayerOrder);
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
			BLOGE("Unsupported bayer order:%d rt:%d", bayerOrder, rt);
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
		BLOGE("Invalid input %d", rt);
	}

	if (width % 2 || height % 2) {
		rt = BZ_FAILED;
		BLOGE("Unsupported ord width/height %dx%d", width, height);
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
				BLOGE("Unsupported bayer order:%d", rt);
				break;
		}
	}

	return rt;
}

BZResult BZ_Demosaic(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data || !pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input! rt:%d", rt);
	}

	int32_t width, height;
	LIB_BAYER_ORDER bayerOrder = LIB_BAYER_ORDER_NUM;
	width = pParams->info.width;
	height = pParams->info.height;
	bayerOrder = pParams->info.bayerOrder;
	BLOGDA("width:%d height:%d type:%d", width, height, bayerOrder);
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
		BLOGE("Unsupported bayer order:%d rt:%d", bayerOrder, rt);
		break;
	}
	if (SUCCESS(rt)) {
		rt = DemosaicInterpolation(pChannel1, pChannel2, pChannel3,
				width, height, bayerOrder);
		/* Preserve original method for understanding */
		/* FirstPixelInsertProcess(pChannel1, pChannel1, width, height);*/
		/* TwoGPixelInsertProcess(pChannel2, pChannel2, width, height); */
		/* LastPixelInsertProcess(pChannel3, pChannel3, width, height); */
	}

	return rt;
}

BZResult BZ_WhiteBalance(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data || !pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		int32_t width, height;
		float rGain, gGain, bGain;
		width = pParams->info.width;
		height = pParams->info.height;
		rGain = pParams->WB_param.rGain;
		gGain = pParams->WB_param.gGain;
		bGain = pParams->WB_param.bGain;
		BLOGDA("width:%d height:%d", width, height);
		BLOGDA("Gain(r/g/b):%f %f %f", rGain, gGain, bGain);

		uint16_t* B = static_cast<uint16_t*>(data);
		uint16_t* G = B + width * height;
		uint16_t* R = G + width * height;

		if (SUCCESS(rt)) {
			for (int i = 0; i < width * height; i++) {
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
	int32_t w, h;
	float* pCCM = NULL;
	uint16_t* srcCh1 = NULL;
	uint16_t* srcCh2 = NULL;
	uint16_t* srcCh3 = NULL;
	int16_t* pTmp = NULL;
	int16_t* dstCh1 = NULL;
	int16_t* dstCh2 = NULL;
	int16_t* dstCh3 = NULL;
	int32_t bpp = 0;
	(void)CBs;

	if (!data || !pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		w = pParams->info.width;
		h = pParams->info.height;
		bpp = pParams->info.bitspp;
		if (!w || !h || !bpp) {
			rt = BZ_INVALID_PARAM;
			BLOGE("Invalid param:%dx%d %db %d", w, h, bpp, rt);
		}
		pCCM = pParams->CC_param.CCM;
		if (!pCCM) {
			rt = BZ_INVALID_PARAM;
			BLOGE("Faild to get CCM %d", rt);
		}
	}

	if (SUCCESS(rt)) {
		BLOGDA("width:%d height:%d bitwidth:%d", w, h, bpp);
		for (int32_t i = 0; i < CCM_HEIGHT; i++) {
			BLOGDA("CCM[%d]:%f CCM[%d]:%f CCM[%d]:%f", i * CCM_WIDTH, pCCM[i * CCM_WIDTH],
					i * CCM_WIDTH + 1, pCCM[i * CCM_WIDTH + 1],
					i * CCM_WIDTH + 2, pCCM[i * CCM_WIDTH + 2]);
		}

		srcCh1 = static_cast<uint16_t*>(data);
		srcCh2 = srcCh1 + w * h;
		srcCh3 = srcCh2 + w * h;

		pTmp = new int16_t [w * h * 3];
		memset(pTmp, 0, w * h * 3 * sizeof(int16_t));
		dstCh1 = pTmp;
		dstCh2 = dstCh1 + w * h;
		dstCh3 = dstCh2 + w * h;

		for (int32_t i = 0; i < w * h; i++) {
			dstCh1[i] = srcCh1[i] * pCCM[0] +
				srcCh2[i] * pCCM[CCM_WIDTH] +
				srcCh3[i] * pCCM[2 * CCM_WIDTH];
			dstCh2[i] = srcCh1[i] * pCCM[1] +
				srcCh2[i] * pCCM[CCM_WIDTH + 1] +
				srcCh3[i] * pCCM[2 * CCM_WIDTH + 1];
			dstCh3[i] = srcCh1[i] * pCCM[2] +
				srcCh2[i] * pCCM[CCM_WIDTH + 2] +
				srcCh3[i] * pCCM[2 * CCM_WIDTH + 2];
			dstCh1[i] = (dstCh1[i] < 0) ? 0 : ((dstCh1[i] > pow(2, bpp) - 1) ? pow(2, bpp) - 1 : dstCh1[i]);
			dstCh2[i] = (dstCh2[i] < 0) ? 0 : ((dstCh2[i] > pow(2, bpp) - 1) ? pow(2, bpp) - 1 : dstCh2[i]);
			dstCh3[i] = (dstCh3[i] < 0) ? 0 : ((dstCh3[i] > pow(2, bpp) - 1) ? pow(2, bpp) - 1 : dstCh3[i]);
		}

		memcpy(data, pTmp, w * h * 3 * sizeof(uint16_t));
		delete[] pTmp;
	}
	return rt;
}

BZResult BZ_GammaCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;

	if (!data || !pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		int32_t width, height;
		uint16_t* plut = nullptr;
		width = pParams->info.width;
		height = pParams->info.height;
		plut = pParams->Gamma_param.lut;
		BLOGDA("width:%d height:%d", width, height);

		uint16_t* B = static_cast<uint16_t*>(data);
		uint16_t* G = B + width * height;
		uint16_t* R = G + width * height;

		for (int32_t i = 0; i < width * height; i++) {
			B[i] = plut[B[i]];
			G[i] = plut[G[i]];
			R[i] = plut[R[i]];
		}
	}

	return rt;
}

BZResult WaveletDecomposition(void* src, void* dst, int32_t bufW, int32_t bufH, int32_t depth)
{
	BZResult rt = BZ_SUCCESS;

	float *pT = NULL, *pB = NULL;
	float *pLT = NULL, *pRT = NULL, *pLB = NULL, *pRB = NULL;
	float *pSrc = static_cast<float*>(src);
	float *pDst = static_cast<float*>(dst);
	int32_t quartW, quartH, ROIW, ROIH;

	if (!src || !dst) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input");
	}

	if (SUCCESS(rt)) {
		quartW = bufW / pow(2, depth);
		quartH = bufH / pow(2, depth);
		ROIW = quartW * 2;
		ROIH = quartH * 2;
		if (ROIW > bufW || ROIH > bufH) {
			rt = BZ_INVALID_PARAM;
			BLOGE("Oversize: ROI(%dx%d) > buffer(%dx%d) depth:%d ", ROIW, ROIH, bufW, bufH, depth);
		}
	}

	if (SUCCESS(rt)) {
		BLOGDA("ROI(%dx%d) QUART(%dx%d) depth:%d", ROIW, ROIH, quartW, quartH, depth);
		pT = static_cast<float*>(WrapAlloc(ROIW * ROIH / 2, sizeof(float)));
		pB = static_cast<float*>(WrapAlloc(ROIW * ROIH / 2, sizeof(float)));
		pLT = static_cast<float*>(WrapAlloc(quartW * quartH, sizeof(float)));
		pLB = static_cast<float*>(WrapAlloc(quartW * quartH, sizeof(float)));
		pRT = static_cast<float*>(WrapAlloc(quartW * quartH, sizeof(float)));
		pRB = static_cast<float*>(WrapAlloc(quartW * quartH, sizeof(float)));

		for (int32_t row = 0; row < ROIH; row += 2) {
			for (int32_t col = 0; col < ROIW; col++) {
				pT[row / 2 * ROIW + col] = (pSrc[row * bufW + col] + pSrc[(row + 1) * bufW + col]) * 0.707;
				pB[row / 2 * ROIW + col] = (pSrc[row * bufW + col] - pSrc[(row + 1) * bufW + col]) * 0.707;
			}
		}

		for (int32_t row = 0; row < quartH; row++) {
			for (int32_t col = 0; col < ROIW; col += 2) {
				pLT[row * quartW + col / 2] = (pT[row * ROIW + col] + pT[row * ROIW + col + 1]) * 0.707;
				pRT[row * quartW + col / 2] = (pT[row * ROIW + col] - pT[row * ROIW + col + 1]) * 0.707;

				pLB[row * quartW + col / 2] = (pB[row * ROIW + col] + pB[row * ROIW + col + 1]) * 0.707;
				pRB[row * quartW + col / 2] = (pB[row * ROIW + col] - pB[row * ROIW + col + 1]) * 0.707;
			}
		}

		for (int32_t row = 0; row < quartH; row ++) {
			memcpy(pDst + row * bufW, pLT + row * quartW, quartW * sizeof(float));
			memcpy(pDst + row * bufW + quartW, pRT + row * quartW, quartW * sizeof(float));
			memcpy(pDst + (quartH + row) * bufW, pLB + row * quartW, quartW * sizeof(float));
			memcpy(pDst + (quartH + row) * bufW + quartW, pRB + row * quartW, quartW * sizeof(float));
		}

		pT = static_cast<float*>(WrapFree((void*)pT));
		pB = static_cast<float*>(WrapFree((void*)pB));
		pLT = static_cast<float*>(WrapFree((void*)pLT));
		pLB = static_cast<float*>(WrapFree((void*)pLB));
		pRT = static_cast<float*>(WrapFree((void*)pRT));
		pRB = static_cast<float*>(WrapFree((void*)pRB));
	}

	return rt;
}

BZResult WaveletRecomposition(void* src, void* dst, int32_t bufW, int32_t bufH, int32_t depth)
{
	BZResult rt = BZ_SUCCESS;

	float *pT = NULL, *pB = NULL;
	float *pLT = NULL, *pRT = NULL, *pLB = NULL, *pRB = NULL;
	float *pSrc = static_cast<float*>(src);
	float *pDst = static_cast<float*>(dst);
	int32_t quartW, quartH, ROIW, ROIH;

	if (!src || !dst) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input");
	}

	if (SUCCESS(rt)) {
		quartW = bufW / pow(2, depth);
		quartH = bufH / pow(2, depth);
		ROIW = quartW * 2;
		ROIH = quartH * 2;
		if (ROIW > bufW || ROIH > bufH) {
			rt = BZ_INVALID_PARAM;
			BLOGE("Oversize: ROI(%dx%d) > buffer(%dx%d) depth:%d ", ROIW, ROIH, bufW, bufH, depth);
		}
	}

	if (SUCCESS(rt)) {
		BLOGDA("ROI(%dx%d) QUART(%dx%d) depth:%d", ROIW, ROIH, quartW, quartH, depth);
		pT = static_cast<float*>(WrapAlloc(ROIW * ROIH / 2, sizeof(float)));
		pB = static_cast<float*>(WrapAlloc(ROIW * ROIH / 2, sizeof(float)));
		pLT = static_cast<float*>(WrapAlloc(quartW * quartH, sizeof(float)));
		pLB = static_cast<float*>(WrapAlloc(quartW * quartH, sizeof(float)));
		pRT = static_cast<float*>(WrapAlloc(quartW * quartH, sizeof(float)));
		pRB = static_cast<float*>(WrapAlloc(quartW * quartH, sizeof(float)));

		for (int32_t row = 0; row < quartH; row ++) {
			memcpy(pLT + row * quartW, pSrc + row * bufW, quartW * sizeof(float));
			memcpy(pRT + row * quartW, pSrc + row * bufW + quartW, quartW * sizeof(float));
			memcpy(pLB + row * quartW, pSrc + (quartH + row) * bufW, quartW * sizeof(float));
			memcpy(pRB + row * quartW, pSrc + (quartH + row) * bufW + quartW, quartW * sizeof(float));
		}

		for (int32_t row = 0; row < quartH; row++) {
			for (int32_t col = 0; col < quartW; col++) {
				pT[row * ROIW + col * 2] = (pLT[row * quartW + col] + pRT[row * quartW + col]) * 0.707;
				pT[row * ROIW + col * 2 + 1] = (pLT[row * quartW + col] - pRT[row * quartW + col]) * 0.707;
				pB[row * ROIW + col * 2] = (pLB[row * quartW + col] + pRB[row * quartW + col]) * 0.707;
				pB[row * ROIW + col * 2 + 1] = (pLB[row * quartW + col] - pRB[row * quartW + col]) * 0.707;
			}
		}

		for (int32_t row = 0; row < quartH; row++) {
			for (int32_t col = 0; col < ROIW; col++) {
				pDst[row * 2 * bufW + col] = (pT[row * ROIW + col] + pB[row * ROIW + col]) * 0.707;
				pDst[(row * 2 + 1) * bufW + col] = (pT[row * ROIW + col] - pB[row * ROIW + col]) * 0.707;
			}
		}

		pT = static_cast<float*>(WrapFree((void*)pT));
		pB = static_cast<float*>(WrapFree((void*)pB));
		pLT = static_cast<float*>(WrapFree((void*)pLT));
		pLB = static_cast<float*>(WrapFree((void*)pLB));
		pRT = static_cast<float*>(WrapFree((void*)pRT));
		pRB = static_cast<float*>(WrapFree((void*)pRB));
	}

	return rt;
}

#define WNR_DEPTH 3
BZResult WaveletNoiseReduction(void* pData, int32_t bufW, int32_t bufH, int32_t *pStrength)
{
	BZResult rt = BZ_SUCCESS;
	float reductionRatio = 10.0;
	float* pFData = static_cast<float*>(pData);
	int32_t x, y, ROIW, ROIH;

	if (!pFData || !pStrength) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input");
	}

	if (SUCCESS(rt)) {
		for (int32_t depth = 1; depth <= WNR_DEPTH; depth++) {
			x = ROIW = bufW / pow(2, depth);
			y = ROIH = bufH / pow(2, depth);
			if (x + ROIW > bufW || y + ROIH > bufH) {
				rt = BZ_INVALID_PARAM;
				BLOGE("Oversize: ROI(%dx%d) > buffer(%dx%d) depth:%d ", ROIW, ROIH, bufW, bufH, depth);
				return rt;
			}

			for (int32_t row = 0; row < ROIH; row++) {
				for (int32_t col = 0; col < ROIW; col++) {
					/* HH */
					if (abs((int)pFData[(y + row) * bufW + x + col]) < pStrength[depth - 1]) {
						pFData[(y + row) * bufW + x + col] /= reductionRatio;
					}

					/* HL */
					if (abs((int)pFData[row * bufW + x + col]) < pStrength[depth - 1]) {
						pFData[row * bufW + x + col] /= reductionRatio;
					}

					/* LH */
					if (abs((int)pFData[(y + row) * bufW + col]) < pStrength[depth - 1]) {
						pFData[(y + row) * bufW + col] /= reductionRatio;
					}
				}
			}
		}
	}

#if DBG_SHOW_ON
	WrapIMGShow(pData, bufW, bufH, 1, DT_FLOAT);
#endif

	return rt;
}

BZResult BZ_WaveletNR(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;
	int32_t w, h;
	int32_t strength[3][WNR_DEPTH];

	if (!data || !pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		w= pParams->info.width;
		h= pParams->info.height;
		BLOGDA("w:%d h:%d", w, h);
		strength[0][0] = pParams->WNR_param.ch1Threshold[0];
		strength[0][1] = pParams->WNR_param.ch1Threshold[1];
		strength[0][2] = pParams->WNR_param.ch1Threshold[2];
		strength[1][0] = pParams->WNR_param.ch2Threshold[0];
		strength[1][1] = pParams->WNR_param.ch2Threshold[1];
		strength[1][2] = pParams->WNR_param.ch2Threshold[2];
		strength[2][0] = pParams->WNR_param.ch3Threshold[0];
		strength[2][1] = pParams->WNR_param.ch3Threshold[1];
		strength[2][2] = pParams->WNR_param.ch3Threshold[2];
		for (int32_t ch = 0; ch < 3; ch++) {
			BLOGDA("Ch:%d strength:%d %d %d", ch, strength[ch][0],
					strength[ch][1], strength[ch][2]);
		}
	}

	if (SUCCESS(rt)) {
		float *pIn = static_cast<float*>(WrapAlloc(w * h, sizeof(float)));
		float *pOut = static_cast<float*>(WrapAlloc(w * h, sizeof(float)));

		for (int32_t ch = 0; ch < 3; ch++) {
			for (int32_t i = 0; i < w * h; i++) {
				pIn[i] = static_cast<uchar*>(data)[w * h * ch + i] * 1.0;
			}

			for (int32_t depth = 1; depth <= WNR_DEPTH; depth++) {
				rt = WaveletDecomposition(pIn, pOut, w, h, depth);
				memcpy(pIn, pOut, w * h * sizeof(float));
			}

			rt = WaveletNoiseReduction(pIn, w, h, strength[ch]);

			for (int32_t depth = WNR_DEPTH; depth > 0; depth--) {
				rt = WaveletRecomposition(pIn, pOut, w, h, depth);
				if (depth != 1) {
					memcpy(pIn, pOut, w * h * sizeof(float));
				} else {
					for (int32_t i = 0; i < w * h; i++) {
						static_cast<uchar*>(data)[w * h * ch + i] = (int8_t)pOut[i];
					}
				}
			}
		}
		pIn = static_cast<float*>(WrapFree(pIn));
		pOut = static_cast<float*>(WrapFree(pOut));
	}

	return rt;
}

struct FilterKernel2D {
	vector<float> kx;
	vector<float> ky;
};

struct FilterKernel1DConfig {
	float k3[3];
	float k5[5];
	float k7[7];
};

static FilterKernel1DConfig gDefaultGaussionKernel {
	{0.25,		0.5,		0.25},
	{0.0625,	0.25,		0.375,		0.25,		0.0625},
	{0.03125,	0.109375,	0.21875,	0.28125,	0.21875,	0.109375,	0.03125}
};

BZResult CreatGaussion2DKernel(FilterKernel2D* pK, size_t size, int32_t sigma)
{
	BZResult rt = BZ_SUCCESS;

	if (!pK) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Input is null");
	}

	if (size / 2 == 0) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid kernel size:%u", size);
	}

	if (!pK->kx.empty() || !pK->ky.empty()) {
		vector<float>().swap(pK->kx);
		vector<float>().swap(pK->ky);
	}

	if (SUCCESS(rt)) {
		if (sigma <= 0) {
			switch(size) {
				case 3:
					for (size_t i = 0; i < size; i++) {
						pK->kx.push_back(gDefaultGaussionKernel.k3[i]);
						pK->ky.push_back(gDefaultGaussionKernel.k3[i]);
					}
					break;
				case 5:
					for (size_t i = 0; i < size; i++) {
						pK->kx.push_back(gDefaultGaussionKernel.k5[i]);
						pK->ky.push_back(gDefaultGaussionKernel.k5[i]);
					}
					break;
				case 7:
					for (size_t i = 0; i < size; i++) {
						pK->kx.push_back(gDefaultGaussionKernel.k7[i]);
						pK->ky.push_back(gDefaultGaussionKernel.k7[i]);
					}
					break;
				default:
					rt = BZ_INVALID_PARAM;
					BLOGE("Not support kernel size:%u", size);
					break;
			}
		} else {
			float sumx = 0.0, sumy= 0.0;
			for (size_t i = size / 2; i != 0; i--) {
				pK->kx.push_back(1 / (sqrt(2 * M_PI)  * sigma) * exp(-0.5 * i * i / (sigma * sigma)));
				pK->ky.push_back(1 / (sqrt(2 * M_PI)  * sigma) * exp(-0.5 * i * i / (sigma * sigma)));
			}
			pK->kx.push_back(1 / (sqrt(2 * M_PI) * sigma));
			pK->ky.push_back(1 / (sqrt(2 * M_PI) * sigma));
			for (size_t i = 0; i < size / 2; i++) {
				pK->kx.push_back(pK->kx[size / 2 - 1 - i]);
				pK->ky.push_back(pK->kx[size / 2 - 1 - i]);
			}
			for(size_t i = 0; i < pK->kx.size(); i++) {
				sumx += pK->kx[i];
				sumy += pK->ky[i];
			}
			for(size_t i = 0; i < pK->kx.size(); i++) {
				pK->kx[i] /= sumx ;
				pK->ky[i] /= sumy ;
			}
		}
	}

	for(size_t i = 0; i < pK->kx.size(); i++) {
		BLOGDA("kx[%d]:%f ky[%d]:%f", i, pK->kx[i], i,pK->ky[i]);
	}

	return rt;
}

BZResult Convolution2D(void* pData, size_t w, size_t h, FilterKernel2D* pK)
{
	BZResult rt = BZ_SUCCESS;

	if (!pData || !pK) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Input is NULL");
		return rt;
	}

	float* pHCov = static_cast<float*>(WrapAlloc(w * h, sizeof(float)));
	float* pWCov = static_cast<float*>(WrapAlloc(w * h, sizeof(float)));
	size_t index = 0;
	size_t indexMax = 0;

	for (size_t row = 0; row < h; row++) {
		for (size_t col = 0; col < w; col++) {
			if (row < (pK->ky.size() - 1) / 2) {
				index = (pK->ky.size() - 1) / 2 - row;
				indexMax = pK->ky.size();
			} else if (row >= h - (pK->ky.size() - 1) / 2) {
				index = 0;
				indexMax = pK->ky.size() - 1 - (row - (h - (pK->ky.size() - 1) / 2));
			} else {
				index = 0;
				indexMax = pK->ky.size();
			}
			while(index < indexMax) {
				pHCov[row * w + col] +=
					static_cast<uchar*>(pData)[(row + index - ((pK->ky.size() - 1) / 2)) * w + col] *
					pK->ky[index];
				index++;
			}
		}
	}

	for (size_t row = 0; row < h; row++) {
		for (size_t col = 0; col < w; col++) {
			if (col < (pK->kx.size() - 1) / 2) {
				index = (pK->kx.size() - 1) / 2 - col;
				indexMax = pK->kx.size();
			} else if (col >= w - (pK->kx.size() - 1) / 2) {
				index = 0;
				indexMax = pK->kx.size() - 1 - (col - (w - (pK->kx.size() - 1) / 2));
			} else {
				index = 0;
				indexMax = pK->kx.size();
			}
			while(index < indexMax) {
				pWCov[row * w + col] += pHCov[row * w + col + index - ((pK->kx.size() - 1) / 2)] *
					pK->kx[index];
				index++;
			}
		}
	}

	for (size_t i = 0; i < w * h; i++) {
		pWCov[i] = pWCov[i] < 0 ? 0 : pWCov[i];
		static_cast<uchar*>(pData)[i] = pWCov[i] > 255 ? 255 : (int)pWCov[i];
	}

#if DBG_SHOW_ON
	WrapIMGShow(pData, w, h, 1, DT_UCHAR);
#endif

	pHCov = static_cast<float*>(WrapFree((void*)pHCov));
	pWCov = static_cast<float*>(WrapFree((void*)pWCov));

	return rt;
}

BZResult BZ_EdgeEnhancement(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;
	int32_t w, h;
	float alpha;
	int32_t coreSzie;
	int32_t sigma;
	FilterKernel2D kernel;

	if (!data || !pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		w = pParams->info.width;
		h = pParams->info.height;
		alpha = pParams->EE_param.alpha;
		coreSzie = pParams->EE_param.coreSize;
		sigma = pParams->EE_param.sigma;
		BLOGDA("width:%d height:%d", w, h);
		BLOGDA("alpha:%f coreSzie:%d sigma:%d", alpha, coreSzie, sigma);
	}

	if (SUCCESS(rt)) {
		rt = CreatGaussion2DKernel(&kernel, coreSzie, sigma);
	}

	if (SUCCESS(rt)) {
		uchar* pTmp = static_cast<uchar*>(WrapAlloc(w * h));
		int32_t mask;
		for (int32_t ch = 0; ch < 3; ch++) {
			if (ch > 0) {
				break; /* It is better to do EE only for Y channel */
			}
			memcpy(pTmp, static_cast<uchar*>(data) + w * h * ch, w * h);

			rt = Convolution2D(pTmp, w, h, &kernel);

			for (int32_t row = 0; row < h; row++) {
				for (int32_t col = 0; col < w; col++) {
					mask = static_cast<uchar*>(data)[w * h * ch + row * w + col] - pTmp[row * w + col];
					mask = (int)(static_cast<uchar*>(data)[w * h * ch + row * w + col] + (int)(alpha * mask));
					mask = VFIX_UCHAR(mask);
					static_cast<uchar*>(data)[w * h * ch + row * w + col] = VMASK_UCHAR(mask);
				}
			}
		}
		pTmp = static_cast<uchar*>(WrapFree((void*)pTmp));
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
		BLOGE("Invalid input! rt:%d", rt);
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
				BLOGE("Not support bayer order:%d", order);
				rt = BZ_FAILED;
				break;
		}
	}

	return rt;
}

BZResult BZ_CST_RAW2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;

	if (!src || !dst || !pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input! rt:%d", rt);
	}

	int32_t width, height;
	LIB_BAYER_ORDER bayerOrder = LIB_BAYER_ORDER_NUM;
	width = pParams->info.width;
	height = pParams->info.height;
	bayerOrder = pParams->info.bayerOrder;
	BLOGDA("width:%d height:%d type:%d", width, height, bayerOrder);
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
		BLOGE("Unsupported bayer order:%d rt:%d", bayerOrder, rt);
		break;
	}

	if (SUCCESS(rt)) {
		rt = ReadChannels(static_cast<uint16_t*>(src), pChannel1, pChannel2, pChannel3,
				width, height, bayerOrder);
	}

#if DBG_DUMP_ON
	if (SUCCESS(rt)) {
		BoZhi* pBZ = static_cast<BoZhi*>(WrapGetBoZhi());
		if (pBZ->mISPCBs.UtilsFuncs.DumpDataInt) {
			BLOGDA("Raw dump as int");
			pBZ->mISPCBs.UtilsFuncs.DumpDataInt((void*)src, width, height, (int32_t)sizeof(uint16_t), DUMP_PATH);
		}
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

/* RGB to YUV fallow below formulor			*/
/* Y =  0.299 * R + 0.587 * G + 0.114 * B;	*/
/* U = -0.169 * R - 0.331 * G + 0.5 * B ;	*/
/* V =  0.5 * R - 0.419 * G - 0.081 * B;	*/
#define RGB2Y(r, g, b) VFIX_UCHAR((77 * (r) + 150 * (g) + 29 *(b)) >> 8)
#define RGB2U(r, g, b) VFIX_UCHAR(((131 * (b) - 44 * (r) - 87 * (g)) >> 8) + 128)
#define RGB2V(r, g, b) VFIX_UCHAR(((131 * (r) - 110 * (g) - 21 * (b)) >> 8) + 128)

/* YUV to RGB fallow below formulor		*/
/* R = Y + 1.4075 * V;					*/
/* G = Y - (0.3455 * U + 0.7169 * V);	*/
/* B = Y + 1.779 * U;					*/
#define YUV2R(y, u ,v) VFIX_UCHAR((y) + ((360 * ((v) - 128)) >> 8))
#define YUV2G(y, u ,v) VFIX_UCHAR((y) - ((88 * ((u) - 128) + 184 * ((v) - 128)) >> 8))
#define YUV2B(y, u ,v) VFIX_UCHAR((y) + ((455 * ((u) - 128)) >> 8))
#define YUV2R_uchar(y, u ,v) YUV2R(VMASK_UCHAR(y), VMASK_UCHAR(u), VMASK_UCHAR(v))
#define YUV2G_uchar(y, u ,v) YUV2G(VMASK_UCHAR(y), VMASK_UCHAR(u), VMASK_UCHAR(v))
#define YUV2B_uchar(y, u ,v) YUV2B(VMASK_UCHAR(y), VMASK_UCHAR(u), VMASK_UCHAR(v))


BZResult BZ_CST_RGB2YUV(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	uint32_t w, h;
	bool enable = true;
	uint32_t inOrder = RO_BGR;
	uint32_t outOrder = YO_YUV;
	uint16_t *pB16 = NULL, *pG16 = NULL, *pR16= NULL;
	uint8_t *pB8 = NULL, *pG8 = NULL, *pR8= NULL;
	uchar *pY = NULL, *pU = NULL, *pV = NULL;
	uchar* pTmp = NULL;

	if (!src || !dst || !pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Invalid input! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		va_list va_param;
		va_start(va_param, CBs);
		enable = static_cast<bool>(va_arg(va_param, int32_t));
		va_end(va_param);
		BLOGDA("%s", enable ? "ON" : "OFF");
		w = pParams->info.width;
		h = pParams->info.height;
		BLOGDA("%ux%u", w, h);
	}

	if (SUCCESS(rt)) {
		if (inOrder == RO_BGR) {
			pB16 = static_cast<uint16_t*>(src);
			pG16 = static_cast<uint16_t*>(src) + w * h;
			pR16 = static_cast<uint16_t*>(src) + 2 * w * h;
		} else if (inOrder == RO_RGB) {
			pR16 = static_cast<uint16_t*>(src);
			pG16 = static_cast<uint16_t*>(src) + w * h;
			pB16 = static_cast<uint16_t*>(src) + 2 * w * h;
		} else {
			BLOGE("Unsupported RGB order:%d", inOrder);
		}
	}

	if (SUCCESS(rt)) {
		if (outOrder == YO_YUV) {
			pY = static_cast<uchar*>(dst);
			pU = static_cast<uchar*>(dst) + w * h;
			pV = static_cast<uchar*>(dst) + 2 *w * h;
		} else if (outOrder == YO_YVU) {
			pY = static_cast<uchar*>(dst);
			pV = static_cast<uchar*>(dst) + w * h;
			pU = static_cast<uchar*>(dst) + 2 * w * h;
		} else {
			BLOGE("Unsupported YUV order:%d", inOrder);
		}
	}

	if (SUCCESS(rt)) {
		pTmp = static_cast<uchar*>(WrapAlloc(w * h * 3));
		if (pTmp) {
			pB8 = pTmp;
			pG8 = pTmp + w * h;
			pR8 = pTmp + 2 * w * h;
			Compress10to8(pB16, pB8, w * h, true);
			Compress10to8(pG16, pG8, w * h, true);
			Compress10to8(pR16, pR8, w * h, true);
			if (!enable) {
				memcpy(dst, pTmp, w * h * 3);
			} else {
				for (uint32_t row = 0; row < h; row++) {
					for (uint32_t col = 0; col < w; col++) {
						pY[row * w + col] = RGB2Y(pR8[row * w + col], pG8[row * w + col], pB8[row * w + col]);
						pU[row * w + col] = RGB2U(pR8[row * w + col], pG8[row * w + col], pB8[row * w + col]);
						pV[row * w + col] = RGB2V(pR8[row * w + col], pG8[row * w + col], pB8[row * w + col]);
					}
				}
			}
		}
		else {
			rt = BZ_MEMORY_ERROR;
			BLOGE("cannot new buffer for 8 bits data! rt:%d", rt);
		}

		pTmp = static_cast<uchar*>(WrapFree((void*)pTmp));
		pB8 = pG8 = pR8 = NULL;
	}

	return rt;
}

BZResult BZ_CST_YUV2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...)
{
	BZResult rt = BZ_SUCCESS;
	(void)CBs;
	uint32_t w = 0, h = 0;
	/* TODO: set dynamic value from param */
	uint32_t inBpp = 8, outBpp = 8;
	uint32_t inOrder = YO_YUV;
	uint32_t outOrder = RO_BGR;
	uint32_t dataStruct = YS_444;
	uint32_t planeOffset = 0, planeOffsetUorV = 0;
	uchar *pY = NULL, *pU = NULL, *pV = NULL;
	uchar u = 128, v = 128;
	uchar *pTmp = NULL;
	uchar *pR = NULL, *pG = NULL, *pB = NULL;

	if (!src || !dst || !pParams) {
		rt = BZ_INVALID_PARAM;
		BLOGE("data is null! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		bool enable = true;
		va_list va_param;
		va_start(va_param, CBs);
		enable = static_cast<bool>(va_arg(va_param, int32_t));
		BLOGDA("%s", enable ? "ON" : "OFF");
		w = pParams->info.width;
		h = pParams->info.height;
		BLOGDA("%ux%u ", w, h);
		va_end(va_param);
		if (!enable) {
			if (src != dst) {
				if (inBpp == outBpp) { /* TODO: should be in&out size checking*/
					memcpy(dst, src, w * h * 3);
				} else {
					rt = BZ_INVALID_PARAM;
					BLOGE("Invalid in&out param");
				}
			}
			return rt;
		}
	}

	if (SUCCESS(rt)) {
		pTmp = (src == dst) ? static_cast<uchar*>(WrapAlloc(w * h * 3)) : static_cast<uchar*>(dst);
	}

	if (SUCCESS(rt)) {
		switch (inBpp) {
			case 8:
				pY = static_cast<uchar*>(src);
				planeOffset = w * h;
				if (dataStruct == YS_444) {
					planeOffsetUorV = w * h;
					pU = (inOrder == YO_YUV) ? (pY + planeOffset) : (pY + 2 * planeOffset);
					pV = (inOrder == YO_YUV) ? (pY + 2 * planeOffset) : (pY + planeOffset);
				} else if (dataStruct == YS_420_NV) {
					planeOffsetUorV = w * h / 4;
					pU = (inOrder == YO_YUV) ? (pY + planeOffset) : (pY + planeOffset + 1);
					pV = (inOrder == YO_YUV) ? (pY + planeOffset + 1) : (pY + planeOffset);
				} else if (dataStruct == YS_420) {
					planeOffsetUorV = w * h / 4;
					pU = (inOrder == YO_YUV) ? (pY + planeOffset) : (pY + planeOffset + planeOffsetUorV);
					pV = (inOrder == YO_YUV) ? (pY + planeOffset + planeOffsetUorV) : (pY + planeOffset);
				} else if (dataStruct == YS_GREY) {
					pU = NULL;
					pV = NULL;
				} else {
					rt = BZ_INVALID_PARAM;
					BLOGW("TODO: not support data struct:%u", dataStruct);
				}
				break;
			case 10:
			case 12:
			case 14:
			case 16:
				BLOGW("TODO: not support bw:%u", pParams->info.bitspp);
			default:
				rt = BZ_INVALID_PARAM;
				BLOGE("Invalid input bit width:%u", pParams->info.bitspp);
				break;
		}
	}

	if (SUCCESS(rt)) {
		switch (outBpp) {
			case 8:
				planeOffset = w * h;
				if (outOrder == RO_RGB) {
					pR = pTmp;
					pG = pR + planeOffset;
					pB = pG + planeOffset;
				} else if (outOrder == RO_BGR) {
					pB = pTmp;
					pG = pB + planeOffset;
					pR = pG + planeOffset;
				} else {
					rt = BZ_INVALID_PARAM;
					BLOGE("Invalid output order:%d", outOrder);
				}
				break;
			default:
				rt = BZ_INVALID_PARAM;
				BLOGE("Invalid output bit width:%u", pParams->info.bitspp);
				break;
		}
	}

	if (SUCCESS(rt)) {
		if (dataStruct == YS_444) {
			for (uint32_t row = 0 ; row < h; row++) {
				for (uint32_t col = 0 ; col < w; col++) {
					u = pU[row * w + col];
					v = pV[row * w + col];
					pR[row * w + col] = YUV2R_uchar(pY[row * w + col], u, v);
					pG[row * w + col] = YUV2G_uchar(pY[row * w + col], u, v);
					pB[row * w + col] = YUV2B_uchar(pY[row * w + col], u, v);
				}
			}
		} else if (dataStruct == YS_420 || dataStruct == YS_420_NV || dataStruct == YS_GREY) {
			for (uint32_t row = 0 ; row < h; row += 2) {
				for (uint32_t col = 0 ; col < w; col += 2) {
					if (dataStruct == YS_420_NV) {
						u = pU[row / 2 * w + col];
						v = pV[row / 2 * w + col];
					} else if (dataStruct == YS_420) {
						u = pU[(row % 4 / 2 * w / 2) + row / 4 * w + col / 2];
						v = pV[(row % 4 / 2 * w / 2) + row / 4 * w + col / 2];
					} else if (dataStruct == YS_GREY) {
						u = 128;
						v = 128;
					}

					pR[row * w + col]			= YUV2R_uchar(pY[row * w + col], u, v);
					pR[row * w + col + 1]		= YUV2R_uchar(pY[row * w + col + 1], u, v);
					pR[(row + 1) * w + col]		= YUV2R_uchar(pY[(row + 1) * w + col], u, v);
					pR[(row + 1) * w + col + 1]	= YUV2R_uchar(pY[(row + 1) * w + col + 1], u, v);
					pG[row * w + col]			= YUV2G_uchar(pY[row * w + col], u, v);
					pG[row * w + col + 1]		= YUV2G_uchar(pY[row * w + col + 1], u, v);
					pG[(row + 1) * w + col]		= YUV2G_uchar(pY[(row + 1) * w + col], u, v);
					pG[(row + 1) * w + col + 1]	= YUV2G_uchar(pY[(row + 1) * w + col + 1], u, v);
					pB[row * w + col]			= YUV2B_uchar(pY[row * w + col], u, v);
					pB[row * w + col + 1]		= YUV2B_uchar(pY[row * w + col + 1], u, v);
					pB[(row + 1) * w + col]		= YUV2B_uchar(pY[(row + 1) * w + col], u, v);
					pB[(row + 1) * w + col + 1]	= YUV2B_uchar(pY[(row + 1) * w + col + 1], u, v);
				}
			}
		} else {
			rt = BZ_INVALID_PARAM;
			BLOGE("Invalid data struct:%u", dataStruct);
		}
	}

	if (src == dst) {
		memcpy(dst, pTmp, w * h * 3);
		pTmp = static_cast<uchar*>(WrapFree((void*)pTmp));
	}

	return rt;
}
