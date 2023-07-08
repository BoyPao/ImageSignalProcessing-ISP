// License: GPL-3.0-or-later
/*
 * BoZhi algorithm head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once

#include "BZInterface.h"

#define CHECK_PACKAGED(format)                  \
	(((format) == BZ_UNPACKAGED_RAW10_LSB) ||   \
	 ((format) == BZ_UNPACKAGED_RAW10_MSB)) ? 0 : 1

#define PIXELS2WORDS_MIPI_PACKAGED(pixelNum, bitspp)	\
	(((bitspp) < BITS_PER_WORD) ? 0 :					\
	 (((bitspp) == BITS_PER_WORD) ? (pixelNum) :		\
	  ((pixelNum) * (bitspp) / BITS_PER_WORD)))

#define	PIXELS2WORDS_UNPACKAGED(pixelNum, bitspp)		\
	(((bitspp) < BITS_PER_WORD) ? 0 :					\
	 (((bitspp) == BITS_PER_WORD) ? (pixelNum) :		\
	  ((pixelNum) * 2)))

#define PIXELS2WORDS(pixelNum, bitspp, packaged)							\
	(((bitspp) % 2 || (bitspp) > 2 * BITS_PER_WORD || (bitspp) <= 0) ?	0 : \
	 ((packaged) ? PIXELS2WORDS_MIPI_PACKAGED(pixelNum, bitspp) :			\
	  PIXELS2WORDS_UNPACKAGED(pixelNum, bitspp)))

#define ALIGN(x, align) \
	(align) ? (((x) + (align) - 1) & (~((align) - 1))) : (x)

#define ALIGNx(pixelNum, bitspp, packaged, align) \
	ALIGN(PIXELS2WORDS(pixelNum, bitspp, packaged), align)

struct Buffer {
	size_t size;
	void *pAddr;
};

struct DataInfo {
	size_t w;
	size_t h;
	int32_t fmt;
	int32_t order;
	int32_t bpp;
	Buffer buf;
};

struct AlgInfo {
	int32_t type;
	bool en;
	DataInfo srcInfo;
	DataInfo dstInfo;
	Buffer param;
};

enum RGBOrder {
	RO_RGB = 0,
	RO_BGR,
	RO_NUM
};

enum YUVOrder {
	YO_YUV = 0,
	YO_YVU,
	YO_NUM
};

enum YUVStruct {
	YS_444 = 0,
	YS_422,
	YS_420,
	YS_420_NV,
	YS_GREY,
	YS_NUM
};

/* Bayer Process */
int32_t WrapBlackLevelCorrection(AlgInfo *info);
int32_t WrapLensShadingCorrection(AlgInfo *info);

/* RGB Process */
int32_t WrapDemosaic(AlgInfo *info);
int32_t WrapWhiteBalance(AlgInfo *info);
int32_t WrapColorCorrection(AlgInfo *info);
int32_t WrapGammaCorrection(AlgInfo *info);

/* YUVProcess */
int32_t WrapWaveletNR(AlgInfo *info);
int32_t WrapEdgeEnhancement(AlgInfo *info);

/* CST */
int32_t WrapCST_RAW2RGB(AlgInfo *info);
int32_t WrapCST_RGB2YUV(AlgInfo *info);
int32_t WrapCST_YUV2RGB(AlgInfo *info);

