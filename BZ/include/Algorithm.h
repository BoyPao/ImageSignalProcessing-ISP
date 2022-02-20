// License: GPL-3.0-or-later
/*
 * BoZhi algorithm head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once

#include "BZUtils.h"
#include "BZLog.h"
#include "LibInterface.h"

#define CHECK_PACKAGED(format)                          (((format) == LIB_UNPACKAGED_RAW10_LSB) ||                          \
														((format) == LIB_UNPACKAGED_RAW10_MSB)) ? 0 : 1

#define PIXELS2WORDS_MIPI_PACKAGED(pixelNum, bitspp)	(((bitspp) < BITS_PER_WORD) ?										\
														0 :																	\
														(((bitspp) == BITS_PER_WORD) ?										\
															(pixelNum) :													\
															((pixelNum) * (bitspp) / BITS_PER_WORD)))
#define	PIXELS2WORDS_UNPACKAGED(pixelNum, bitspp)		(((bitspp) < BITS_PER_WORD) ?										\
														0 :																	\
														(((bitspp) == BITS_PER_WORD) ?										\
															(pixelNum) :													\
															((pixelNum) * 2)))
#define PIXELS2WORDS(pixelNum, bitspp, packaged)		(((bitspp) % 2 || (bitspp) > 2 * BITS_PER_WORD || (bitspp) <= 0) ?	\
														0 :																	\
														((packaged) ?														\
															PIXELS2WORDS_MIPI_PACKAGED(pixelNum, bitspp) :					\
															PIXELS2WORDS_UNPACKAGED(pixelNum, bitspp)))

#define ALIGN(x, align)									(align) ? (((x) + (align) - 1) & (~((align) - 1))) : (x)
#define ALIGNx(pixelNum, bitspp, packaged, align)		ALIGN(PIXELS2WORDS(pixelNum, bitspp, packaged), align)

/* Bayer Process */
void WrapBlackLevelCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void WrapLensShadingCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

/* RGB Process */
void WrapDemosaic(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void WrapWhiteBalance(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void WrapColorCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void WrapGammaCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

/* YUVProcess */
void WrapWaveletNR(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void WrapEdgeEnhancement(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

/* CST */
void WrapCST_RAW2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void WrapCST_RGB2YUV(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void WrapCST_YUV2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

