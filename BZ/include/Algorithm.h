//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Algorithm.h
// @brief: ISP algorithm head file.
//////////////////////////////////////////////////////////////////////////////////////

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
void Lib_BlackLevelCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void Lib_LensShadingCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

/* RGB Process */
void Lib_Demosaic(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void Lib_WhiteBalance(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void Lib_ColorCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void Lib_GammaCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

/* YUVProcess */
void Lib_WaveletNR(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void Lib_EdgeEnhancement(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

/* CST */
void Lib_CST_RAW2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void Lib_CST_RGB2YUV(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void Lib_CST_YUV2RGB(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

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




